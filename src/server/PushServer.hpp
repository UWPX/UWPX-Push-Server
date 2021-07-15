#pragma once
#include "storage/ConfigurationStorage.hpp"
#include "storage/redis/RedisClient.hpp"
#include "tcp/TcpServer.hpp"
#include "tcp/messages/AbstractMessage.hpp"
#include "tcp/messages/SetPushAccountsMessage.hpp"
#include "wns/WnsClient.hpp"
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
    wns::WnsClient wnsClient;
    tcp::TcpServer tcpServer;
    storage::redis::RedisClient redisClient;
    std::optional<std::thread> serverThread{std::nullopt};
    PushServerState state{PushServerState::NOT_RUNNING};

 public:
    explicit PushServer(const storage::Configuration& config);
    PushServer(PushServer&&) = default;
    PushServer(const PushServer&) = delete;
    PushServer& operator=(PushServer&&) = default;
    PushServer& operator=(const PushServer&) = delete;
    ~PushServer();

    void start();
    void stop();

 private:
    void thread_run();
    void check_setup_wns();
    void on_message_received(const std::shared_ptr<tcp::messages::AbstractMessage>& msg);
    void send_test_push(const std::string& deviceId);
    void set_push_accounts(const std::string& deviceId, const std::vector<std::string>& accounts);
    void set_channel_uri(const std::string& deviceId, const std::string& channelUri);
};
}  // namespace server