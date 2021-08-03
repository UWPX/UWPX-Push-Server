#pragma once
#include "storage/ConfigurationStorage.hpp"
#include "storage/redis/RedisClient.hpp"
#include "tcp/ClientSslSession.hpp"
#include "tcp/TcpServer.hpp"
#include "tcp/messages/AbstractMessage.hpp"
#include "tcp/messages/SetPushAccountsMessage.hpp"
#include "wns/WnsClient.hpp"
#include "xmpp/XmppClient.hpp"
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace server {
class PushServer {
 public:
    enum class PushServerState {
        NOT_RUNNING,
        STARTING,
        RUNNING,
        STOP_REQUESTED,
        WAITING_FOR_JOIN
    };

 private:
    storage::redis::RedisClient redisClient;
    wns::WnsClient wnsClient;
    tcp::TcpServer tcpServer;
    xmpp::XmppClient xmppClient;
    std::optional<std::thread> serverThread{std::nullopt};
    PushServerState state{PushServerState::NOT_RUNNING};

 public:
    explicit PushServer(const storage::Configuration& config);
    PushServer(PushServer&&) = delete;
    PushServer(const PushServer&) = delete;
    PushServer& operator=(PushServer&&) = delete;
    PushServer& operator=(const PushServer&) = delete;
    ~PushServer();

    void start();
    void stop();

 private:
    void thread_run();
    void check_setup_wns();
    void on_message_received(const std::string& s, tcp::ClientSslSession* session);
    void on_message_received(const std::shared_ptr<tcp::messages::AbstractMessage>& msg, tcp::ClientSslSession* session);
    void send_test_push(const std::string& deviceId, tcp::ClientSslSession* session);
    void set_push_accounts(const std::string& deviceId, const std::vector<std::string>& accounts, tcp::ClientSslSession* session);
    void set_channel_uri(const std::string& deviceId, const std::string& channelUri, tcp::ClientSslSession* session);
    void on_message_for_node(const std::string& node, const std::string& msg);
    /**
     * Validates if the given channel URI is valid and publishes to: notify.windows.com
     * Reference: https://docs.microsoft.com/en-us/windows/apps/design/shell/tiles-and-notifications/windows-push-notification-services--wns--overview#requesting-a-notification-channel
     **/
    static constexpr bool is_valid_channel_uri(const std::string& uri);
};
}  // namespace server