#pragma once
#include "storage/ConfigurationStorage.hpp"
#include "tcp/TcpServer.hpp"
#include "wns/WnsClient.hpp"
#include <optional>
#include <thread>

namespace server {
class PushServer {
 public:
    enum class PushServerState {
        NOT_RUNNING,
        STARTING,
        RUNNING,
        STOP_REQUESTED,
        WAITING_FOR_JOIN,
    };

 private:
    wns::WnsClient wnsClient;
    tcp::TcpServer tcpServer;
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
    void threadRun();
    void check_setup_wns();
};
}  // namespace server