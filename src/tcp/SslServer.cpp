#include "SslServer.hpp"
#include "logger/Logger.hpp"

namespace tcp {
SslServer::SslServer(const std::shared_ptr<CppServer::Asio::Service>& asioService, const std::shared_ptr<CppServer::Asio::SSLContext>& sslCtx, CppServer::Asio::InternetProtocol protocol, uint16_t port) : CppServer::Asio::SSLServer(asioService, sslCtx, port, protocol) {}

void SslServer::onError(int error, const std::string& category, const std::string& message) {
    LOG_ERROR << "[" << id().string() << "] SSL session caught an error (code " << error << ") with category " << category << " and message: " << message;
}
}  // namespace tcp