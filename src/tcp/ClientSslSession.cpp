#include "ClientSslSession.hpp"
#include "logger/Logger.hpp"

namespace tcp {
void ClientSslSession::onConnected() {}

void ClientSslSession::onHandshaked() {}

void ClientSslSession::onDisconnected() {}

void ClientSslSession::onReceived(const void* /*buffer*/, size_t /*size*/) {}

void ClientSslSession::onError(int error, const std::string& category, const std::string& message) {
    LOG_ERROR << "[" << id().string() << "] SSL session caught an error (code " << error << ") with category " << category << " and message: " << message;
}

}  // namespace tcp