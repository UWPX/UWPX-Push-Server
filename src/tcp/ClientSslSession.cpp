#include "ClientSslSession.hpp"
#include "logger/Logger.hpp"
#include <string>
#include <server/asio/ssl_session.h>

namespace tcp {
ClientSslSession::ClientSslSession(const std::shared_ptr<CppServer::Asio::SSLServer>& server) : CppServer::Asio::SSLSession(server) {}

void ClientSslSession::onConnected() {
    LOG_DEBUG << '[' << id().string() << "] SSL session Connected.";
}

void ClientSslSession::onHandshaked() {
    LOG_DEBUG << '[' << id().string() << "] SSL session Handshake.";
}

void ClientSslSession::onDisconnected() {
    LOG_DEBUG << '[' << id().string() << "] SSL session Disconnected.";
}

void ClientSslSession::onReceived(const void* buffer, size_t size) {
    std::string msg;
    msg.assign(static_cast<const char*>(buffer), size);
    LOG_DEBUG << '[' << id().string() << "] SSL session received " << size << " bytes: " << msg;
}

void ClientSslSession::onError(int error, const std::string& category, const std::string& message) {
    LOG_ERROR << '[' << id().string() << "] SSL session caught an error (code " << error << ") with category " << category << " and message: " << message;
}

}  // namespace tcp