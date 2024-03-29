#pragma once

#include "ClientSslSession.hpp"
#include <memory>
#include <server/asio/ssl_server.h>

namespace tcp {
class SslServer : public CppServer::Asio::SSLServer {
 private:
    ClientSslSession::messageHandlerFunc messageHandler;

 public:
    SslServer(const std::shared_ptr<CppServer::Asio::Service>& asioService, const std::shared_ptr<CppServer::Asio::SSLContext>& sslCtx, CppServer::Asio::InternetProtocol protocol, uint16_t port, ClientSslSession::messageHandlerFunc&& messageHandler);
    SslServer(SslServer&&) = delete;
    SslServer(const SslServer&) = delete;
    SslServer& operator=(SslServer&&) = delete;
    SslServer& operator=(const SslServer&) = delete;
    ~SslServer() override = default;

 protected:
    void onError(int error, const std::string& category, const std::string& message) override;
    void onStopped() override;
    std::shared_ptr<CppServer::Asio::SSLSession> CreateSession(const std::shared_ptr<CppServer::Asio::SSLServer>& server) override;
};
}  // namespace tcp
