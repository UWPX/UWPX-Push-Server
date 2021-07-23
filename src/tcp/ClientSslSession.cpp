#include "ClientSslSession.hpp"
#include "logger/Logger.hpp"
#include "messages/ErrorResponseMessage.hpp"
#include "messages/SuccessResponseMessage.hpp"
#include "tcp/messages/SuccessResponseMessage.hpp"
#include <algorithm>
#include <cassert>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <server/asio/ssl_session.h>

namespace tcp {
ClientSslSession::ClientSslSession(const std::shared_ptr<CppServer::Asio::SSLServer>& server, messageHandlerFunc&& messageHandler) : CppServer::Asio::SSLSession(server), messageHandler(std::move(messageHandler)) {}

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
    assert(messageHandler);
    messageHandler(msg, this);
}

void ClientSslSession::onError(int error, const std::string& category, const std::string& message) {
    LOG_ERROR << '[' << id().string() << "] SSL session caught an error (code " << error << ") with category " << category << " and message: " << message;
}

void ClientSslSession::respond_with_success() {
    messages::SuccessResponseMessage msg;
    nlohmann::json j;
    msg.to_json(j);
    send(std::move(j));
}

void ClientSslSession::respond_with_error(std::string&& msg) {
    messages::ErrorResponseMessage error(std::move(msg));
    nlohmann::json j;
    error.to_json(j);
    send(std::move(j));
}

void ClientSslSession::send(std::string&& msg) {
    // Send the '\0' as well to indicate the end of a message:
    Send(msg.c_str(), msg.size() + 1);
    LOG_DEBUG << '[' << id().string() << "] SSL session send " << msg.length() + 1 << " bytes: " << msg;
}

void ClientSslSession::send(nlohmann::json&& j) {
    std::string s = j.dump();
    send(std::move(s));
}

}  // namespace tcp
