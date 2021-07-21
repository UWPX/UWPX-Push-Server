#pragma once

#include <functional>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <server/asio/ssl_server.h>
#include <server/asio/ssl_session.h>

namespace tcp {
class ClientSslSession : public CppServer::Asio::SSLSession {
 public:
    using messageHandlerFunc = std::function<void(const std::string& s, tcp::ClientSslSession* session)>;

 protected:
    messageHandlerFunc messageHandler;

 public:
    ClientSslSession(const std::shared_ptr<CppServer::Asio::SSLServer>& server, messageHandlerFunc&& messageHandler);
    ClientSslSession(ClientSslSession&&) = delete;
    ClientSslSession(const ClientSslSession&) = delete;
    ClientSslSession& operator=(ClientSslSession&&) = delete;
    ClientSslSession& operator=(const ClientSslSession&) = delete;
    ~ClientSslSession() override = default;

    void respond_with_error(std::string&& msg);
    void send(std::string&& msg);
    void send(nlohmann::json&& j);

 protected:
    void onConnected() override;
    void onHandshaked() override;
    void onDisconnected() override;
    void onReceived(const void* buffer, size_t size) override;
    void onError(int error, const std::string& category, const std::string& message) override;
};
}  // namespace tcp