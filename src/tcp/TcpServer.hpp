#pragma once
#include "storage/ConfigurationStorage.hpp"
#include <cstdint>
#include <optional>
#include <thread>

namespace tcp {
class TcpServer {
 public:
    enum class TcpServerState {
        NOT_RUNNING,
        STARTING,
        RUNNING,
        STOP_REQUESTED,
        WAITING_FOR_JOIN,
    };

 private:
    std::optional<std::thread> thread{std::nullopt};
    TcpServerState state{TcpServerState::NOT_RUNNING};

    uint16_t port;

 public:
    explicit TcpServer(const storage::TcpConfiguration& config);
    TcpServer(TcpServer&&) = default;
    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(TcpServer&&) = default;
    TcpServer& operator=(const TcpServer&) = delete;
    ~TcpServer();

    TcpServerState getState();

    void start();
    void stop();

 private:
    void threadRun();
};
}  // namespace tcp