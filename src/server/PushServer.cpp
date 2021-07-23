#include "PushServer.hpp"
#include "logger/Logger.hpp"
#include "tcp/ClientSslSession.hpp"
#include "tcp/TcpServer.hpp"
#include "tcp/messages/AbstractMessage.hpp"
#include "tcp/messages/ErrorResponseMessage.hpp"
#include "tcp/messages/Parser.hpp"
#include "tcp/messages/RequestTestPushMessage.hpp"
#include "tcp/messages/SetChannelUriMessage.hpp"
#include "tcp/messages/SetPushAccountsMessage.hpp"
#include "tcp/messages/SuccessSetPushAccountsMessage.hpp"
#include "wns/WnsClient.hpp"
#include <cassert>
#include <functional>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>

namespace server {
PushServer::PushServer(const storage::Configuration& config) : redisClient(config.db),
                                                               wnsClient(config.wns),
                                                               tcpServer(config.tcp, [this](const std::string& s, tcp::ClientSslSession* session) { this->on_message_received(s, session); }),
                                                               xmppClient(config.xmpp, [this](const std::string& node, const std::string& msg) { this->on_message_for_node(node, msg); }) {}

PushServer::~PushServer() {
    assert(state == PushServerState::NOT_RUNNING);
}

void PushServer::start() {
    assert(state == PushServerState::NOT_RUNNING);
    LOG_DEBUG << "Starting the push server thread...";
    state = PushServerState::STARTING;
    serverThread = std::make_optional<std::thread>(&PushServer::thread_run, this);
}

void PushServer::stop() {
    if (state == PushServerState::STARTING || state == PushServerState::RUNNING || state == PushServerState::WAITING_FOR_JOIN) {
        if (state != PushServerState::WAITING_FOR_JOIN) {
            LOG_DEBUG << "Stopping the push server thread...";
            state = PushServerState::STOP_REQUESTED;
        }
        LOG_DEBUG << "Joining the push server thread...";
        serverThread->join();
        state = PushServerState::NOT_RUNNING;
        serverThread = std::nullopt;
        LOG_INFO << "Push server thread joined.";
    } else {
        LOG_DEBUG << "No need to stop the push server thread - not running (state: " << static_cast<int>(state) << ")!";
    }
}

void PushServer::thread_run() {
    assert(state == PushServerState::STARTING || state == PushServerState::STOP_REQUESTED);
    if (state != PushServerState::STARTING) {
        return;
    }
    state = PushServerState::RUNNING;
    LOG_INFO << "Push server thread started.";

    redisClient.init();
    wnsClient.set_redis_client(&redisClient);
    wnsClient.load_token_from_db();
    tcpServer.start();
    xmppClient.start();

    while (state == PushServerState::RUNNING) {
        check_setup_wns();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    tcpServer.stop();
    xmppClient.stop();
    state = PushServerState::WAITING_FOR_JOIN;
    LOG_DEBUG << "Push server thread ready to be joined.";
}

void PushServer::check_setup_wns() {
    if (!wnsClient.is_token_valid()) {
        if (!wnsClient.request_new_token()) {
            LOG_INFO << "Retrying to request a new WNS token in 10 seconds...";
            std::this_thread::sleep_for(std::chrono::seconds(10));
            if (!wnsClient.request_new_token()) {
                LOG_ERROR << "Failed to request a new WNS token for the second time! Exiting...";
            }
        }
    }
}

void PushServer::on_message_received(const std::string& s, tcp::ClientSslSession* session) {
    std::shared_ptr<tcp::messages::AbstractMessage> msg = tcp::messages::parse(s);
    if (!msg) {
        session->respond_with_error("Malformed JSON.");
        return;
    }
    if (!msg->is_valid()) {
        session->respond_with_error("Invalid JSON message.");
        return;
    }

    try {
        on_message_received(msg, session);
    } catch (const std::exception& e) {
        LOG_ERROR << "Failed to process received JSON message '" << s << "' with: " << e.what();
        session->respond_with_error("Internal server error... Failed to process received JSON message.");
        return;
    }
}

void PushServer::on_message_received(const std::shared_ptr<tcp::messages::AbstractMessage>& msg, tcp::ClientSslSession* session) {
    assert(msg);
    assert(msg->is_valid());

    switch (msg->get_type()) {
        case tcp::messages::AbstractMessage::MessageType::SET_CHANNEL_URI_MESSAGE: {
            const tcp::messages::SetChannelUriMessage* actMsg = static_cast<tcp::messages::SetChannelUriMessage*>(msg.get());
            set_channel_uri(actMsg->get_device_id(), actMsg->get_channel_uri(), session);
        } break;
        case tcp::messages::AbstractMessage::MessageType::SET_PUSH_ACCOUNT_MESSAGE: {
            const tcp::messages::SetPushAccountsMessage* actMsg = static_cast<tcp::messages::SetPushAccountsMessage*>(msg.get());
            set_push_accounts(actMsg->get_device_id(), actMsg->get_accounts(), session);
        } break;
        case tcp::messages::AbstractMessage::MessageType::REQUEST_TEST_PUSH_MESSAGE: {
            const tcp::messages::RequestTestPushMessage* actMsg = static_cast<tcp::messages::RequestTestPushMessage*>(msg.get());
            send_test_push(actMsg->get_device_id(), session);
        } break;
        default:
            session->respond_with_error("Unsupported message type.");
            break;
    }
}

void PushServer::send_test_push(const std::string& deviceId, tcp::ClientSslSession* session) {
    const std::optional<std::string> channelUri = redisClient.get_channel_uri(deviceId);
    if (!channelUri) {
        session->respond_with_error("Device id unknown.");
        return;
    }
    bool result = wnsClient.send_raw_notification(*channelUri, "Test push notification from your push server.");
    if (result) {
        session->respond_with_success();
        LOG_INFO << "Test push send to device id: " << deviceId;
    } else {
        session->respond_with_error("Failed to send test push. Something went wrong...");
        LOG_WARNING << "Failed to send test push for device: " << deviceId;
    }
}

void PushServer::set_push_accounts(const std::string& deviceId, const std::vector<std::string>& accounts, tcp::ClientSslSession* session) {
    const std::optional<std::string> channelUri = redisClient.get_channel_uri(deviceId);
    if (!channelUri) {
        session->respond_with_error("Device id unknown.");
        return;
    }

    // Delete all existing XMPP PubSub nodes:
    std::vector<std::string> oldPushNodes = redisClient.get_push_nodes(deviceId);
    for (const std::string& node : oldPushNodes) {
        xmppClient.delete_push_node(node);
        LOG_DEBUG << "Deleted node '" << node << "' for device: " << deviceId;
    }

    // Store the new accounts:
    std::vector<tcp::messages::SuccessSetPushAccountsMessage::PushAccount> pushAccounts;
    pushAccounts.reserve(accounts.size());
    for (const std::string& bareJid : accounts) {
        pushAccounts.push_back(tcp::messages::SuccessSetPushAccountsMessage::PushAccount::create(bareJid));
        LOG_DEBUG << "New Push account with node '" << pushAccounts[pushAccounts.size() - 1].node << "' for device: " << deviceId;
    }

    // Create and subscribe to XMPP PubSub nodes:
    for (tcp::messages::SuccessSetPushAccountsMessage::PushAccount& account : pushAccounts) {
        account.success = xmppClient.setup_push_node(account.node);
    }

    redisClient.set_push_accounts(deviceId, *channelUri, pushAccounts);
    tcp::messages::SuccessSetPushAccountsMessage resultMsg(std::move(pushAccounts), std::string{xmppClient.get_jid()});
    nlohmann::json j;
    resultMsg.to_json(j);
    session->send(std::move(j));
}

void PushServer::set_channel_uri(const std::string& deviceId, const std::string& channelUri, tcp::ClientSslSession* session) {
    redisClient.set_channel_uri(deviceId, channelUri);
    session->respond_with_success();
    LOG_INFO << "Channel URI set for 'device' " << deviceId << " to: " << channelUri;
}

void PushServer::on_message_for_node(const std::string& node, const std::string& msg) {
    const std::optional<std::string> deviceId = redisClient.get_device_id(node);
    if (!deviceId) {
        LOG_WARNING << "Received message for an unknown node '" << node << "'. 'deviceId' not found. Dropping it...";
        return;
    }
    const std::optional<std::string> channelUri = redisClient.get_channel_uri(*deviceId);
    if (!channelUri) {
        LOG_WARNING << "Received message for an unknown node '" << node << "'. 'channelUri' not found. Dropping it...";
        return;
    }
    bool result = wnsClient.send_raw_notification(*channelUri, std::string{msg});
    if (result) {
        LOG_INFO << "Push send to node: " << node;
    } else {
        LOG_WARNING << "Failed to send push for node: " << node;
    }
}

}  // namespace server