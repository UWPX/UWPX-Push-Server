#pragma once

#include <server/asio/ssl_server.h>
#include <server/asio/ssl_session.h>

namespace tcp {
class ClientSslSession : public CppServer::Asio::SSLSession {
 public:
    explicit ClientSslSession(const std::shared_ptr<CppServer::Asio::SSLServer>& server);
    ClientSslSession(ClientSslSession&&) = delete;
    ClientSslSession(const ClientSslSession&) = delete;
    ClientSslSession& operator=(ClientSslSession&&) = delete;
    ClientSslSession& operator=(const ClientSslSession&) = delete;
    ~ClientSslSession() override = default;

 protected:
    void onConnected() override;
    void onHandshaked() override;
    void onDisconnected() override;
    void onReceived(const void* buffer, size_t size) override;
    void onError(int error, const std::string& category, const std::string& message) override;
};
}  // namespace tcp