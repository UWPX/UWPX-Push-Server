#include "SslServer.hpp"
#include "ClientSslSession.hpp"
#include "logger/Logger.hpp"
#include <server/asio/ssl_server.h>

namespace tcp {
SslServer::SslServer(const std::shared_ptr<CppServer::Asio::Service>& asioService, const std::shared_ptr<CppServer::Asio::SSLContext>& sslCtx, CppServer::Asio::InternetProtocol protocol, uint16_t port, ClientSslSession::messageHandlerFunc&& messageHandler) : CppServer::Asio::SSLServer(asioService, sslCtx, port, protocol), messageHandler(std::move(messageHandler)) {}

void SslServer::onError(int error, const std::string& category, const std::string& message) {
    LOG_ERROR << "[" << id().string() << "] SSL server caught an error (code " << error << ") with category " << category << " and message: " << message;
}

void SslServer::onStopped() {
    LOG_ERROR << "[" << id().string() << "] SSL server stopped.";
}

std::shared_ptr<CppServer::Asio::SSLSession> SslServer::CreateSession(const std::shared_ptr<CppServer::Asio::SSLServer>& server) {
    ClientSslSession::messageHandlerFunc handler(messageHandler);
    return std::make_shared<ClientSslSession>(server, std::move(handler));
}
}  // namespace tcp