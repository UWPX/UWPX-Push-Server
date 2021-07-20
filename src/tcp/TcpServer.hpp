#pragma once

#include "SslServer.hpp"
#include "storage/ConfigurationStorage.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <thread>
#include <server/asio/service.h>
#include <server/asio/ssl_server.h>

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

    std::shared_ptr<CppServer::Asio::Service> asioService = std::make_shared<CppServer::Asio::Service>();
    std::shared_ptr<CppServer::Asio::SSLContext> sslCtx;
    std::shared_ptr<SslServer> server;

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