#include "PushServer.hpp"
#include "logger/Logger.hpp"
#include "tcp/TcpServer.hpp"
#include "tcp/messages/AbstractMessage.hpp"
#include "tcp/messages/RequestTestPushMessage.hpp"
#include "tcp/messages/SetChannelUriMessage.hpp"
#include "tcp/messages/SetPushAccountsMessage.hpp"
#include "tcp/messages/SuccessSetPushAccountsMessage.hpp"
#include "wns/WnsClient.hpp"
#include <cassert>
#include <optional>
#include <string>
#include <spdlog/spdlog.h>

namespace server {
PushServer::PushServer(const storage::Configuration& config) : wnsClient(config.wns), tcpServer(config.tcp), redisClient(config.db), xmppClient(config.xmpp) {}

PushServer::~PushServer() {
    assert(state == PushServerState::NOT_RUNNING);
}

void PushServer::start() {
    assert(state == PushServerState::NOT_RUNNING);
    SPDLOG_DEBUG("Starting the push server thread...");
    state = PushServerState::STARTING;
    serverThread = std::make_optional<std::thread>(&PushServer::thread_run, this);
}

void PushServer::stop() {
    if (state == PushServerState::STARTING || state == PushServerState::RUNNING || state == PushServerState::WAITING_FOR_JOIN) {
        if (state != PushServerState::WAITING_FOR_JOIN) {
            SPDLOG_DEBUG("Stopping the push server thread...");
            state = PushServerState::STOP_REQUESTED;
        }
        SPDLOG_DEBUG("Joining the push server thread...");
        serverThread->join();
        state = PushServerState::NOT_RUNNING;
        serverThread = std::nullopt;
        SPDLOG_INFO("Push server thread joined.");
    } else {
        SPDLOG_DEBUG("No need to stop the push server thread - not running (state: {})!", state);
    }
}

void PushServer::thread_run() {
    assert(state == PushServerState::STARTING || state == PushServerState::STOP_REQUESTED);
    if (state != PushServerState::STARTING) {
        return;
    }
    state = PushServerState::RUNNING;
    SPDLOG_INFO("Push server thread started.");

    wnsClient.loadTokenFromDb();
    tcpServer.start();
    redisClient.init();

    while (state == PushServerState::RUNNING) {
        check_setup_wns();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    tcpServer.stop();
    state = PushServerState::WAITING_FOR_JOIN;
    SPDLOG_DEBUG("Push server thread ready to be joined.");
}

void PushServer::check_setup_wns() {
    if (!wnsClient.isTokenValid()) {
        if (!wnsClient.requestToken()) {
            SPDLOG_INFO("Retrying to request a new WNS token in 10 seconds...");
            std::this_thread::sleep_for(std::chrono::seconds(10));
            if (!wnsClient.requestToken()) {
                SPDLOG_ERROR("Failed to request a new WNS token for the second time! Exiting...");
            }
        }
    }
}

void PushServer::on_message_received(const std::shared_ptr<tcp::messages::AbstractMessage>& msg) {
    assert(msg);
    assert(msg->is_valid());

    switch (msg->get_type()) {
        case tcp::messages::AbstractMessage::MessageType::SET_CHANNEL_URI_MESSAGE: {
            const tcp::messages::SetChannelUriMessage* actMsg = static_cast<tcp::messages::SetChannelUriMessage*>(msg.get());
            set_channel_uri(actMsg->get_device_id(), actMsg->get_channel_uri());
        } break;
        case tcp::messages::AbstractMessage::MessageType::SET_PUSH_ACCOUNT_MESSAGE: {
            const tcp::messages::SetPushAccountsMessage* actMsg = static_cast<tcp::messages::SetPushAccountsMessage*>(msg.get());
            set_push_accounts(actMsg->get_device_id(), actMsg->get_accounts());
        } break;
        case tcp::messages::AbstractMessage::MessageType::REQUEST_TEST_PUSH_MESSAGE: {
            const tcp::messages::RequestTestPushMessage* actMsg = static_cast<tcp::messages::RequestTestPushMessage*>(msg.get());
            send_test_push(actMsg->get_device_id());
        } break;
        default:
            // TODO: Respond with: "Unsupported message type."
            break;
    }
}

void PushServer::send_test_push(const std::string& deviceId) {
    const std::optional<std::string> channelUri = redisClient.get_channel_uri(deviceId);
    if (!channelUri) {
        // TODO: Respond with: "Device id unknown."
    }
    wnsClient.sendRawNotification(*channelUri, "Test push notification from your push server.");
    SPDLOG_INFO("Test push send to device id: {}", deviceId);
}

void PushServer::set_push_accounts(const std::string& deviceId, const std::vector<std::string>& accounts) {
    const std::optional<std::string> channelUri = redisClient.get_channel_uri(deviceId);
    if (!channelUri) {
        // TODO: Respond with: "Device id unknown."
    }
    redisClient.set_push_accounts(*channelUri, accounts);
    std::vector<tcp::messages::SuccessSetPushAccountsMessage::PushAccount> pushAccounts;
    pushAccounts.reserve(accounts.size());
    for (const std::string& bareJid : accounts) {
        pushAccounts.push_back(tcp::messages::SuccessSetPushAccountsMessage::PushAccount::create(deviceId, bareJid));
    }

    tcp::messages::SuccessSetPushAccountsMessage resultMsg(std::move(pushAccounts), "REPLACE WITH BARE JID FROM XMPP CONFIG");
}

void PushServer::set_channel_uri(const std::string& /*deviceId*/, const std::string& /*channelUri*/) {
}

}  // namespace server