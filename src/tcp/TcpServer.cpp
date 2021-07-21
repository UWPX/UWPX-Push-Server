#include "TcpServer.hpp"
#include "logger/Logger.hpp"
#include "tcp/SslServer.hpp"
#include <asio/ssl/context.hpp>
#include <chrono>
#include <memory>
#include <server/asio/asio.h>

namespace tcp {
TcpServer::TcpServer(const storage::TcpConfiguration& config, ClientSslSession::messageHandlerFunc&& messageHandler) {
    sslCtx = std::make_shared<CppServer::Asio::SSLContext>(asio::ssl::context::tlsv12);
    sslCtx->use_certificate_file(config.tls.serverCertPath, asio::ssl::context::pem);
    sslCtx->use_private_key_file(config.tls.serverKeyPath, asio::ssl::context::pem);
    server = std::make_shared<SslServer>(asioService, sslCtx, CppServer::Asio::InternetProtocol::IPv4, config.port, std::move(messageHandler));
}

TcpServer::~TcpServer() {
    assert(state == TcpServerState::NOT_RUNNING);
}

TcpServer::TcpServerState TcpServer::getState() {
    return state;
}

void TcpServer::start() {
    assert(state == TcpServerState::NOT_RUNNING);
    LOG_DEBUG << "Starting the TCP thread...";
    state = TcpServerState::STARTING;
    thread = std::make_optional<std::thread>(&TcpServer::threadRun, this);
}

void TcpServer::stop() {
    if (state == TcpServerState::STARTING || state == TcpServerState::RUNNING || state == TcpServerState::WAITING_FOR_JOIN) {
        if (state != TcpServerState::WAITING_FOR_JOIN) {
            LOG_DEBUG << "Stopping the TCP thread...";
            state = TcpServerState::STOP_REQUESTED;
        }
        LOG_DEBUG << "Joining the TCP thread...";
        thread->join();
        state = TcpServerState::NOT_RUNNING;
        thread = std::nullopt;
        LOG_INFO << "TCP thread joined.";
    } else {
        LOG_DEBUG << "No need to stop the TCP thread - not running (state: " << static_cast<int>(state) << ")!";
    }
}

void TcpServer::threadRun() {
    assert(state == TcpServerState::STARTING || state == TcpServerState::STOP_REQUESTED);
    if (state != TcpServerState::STARTING) {
        return;
    }
    state = TcpServerState::RUNNING;
    LOG_INFO << "TCP thread started.";
    asioService->Start();
    server->Start();

    while (state == TcpServerState::RUNNING) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    server->Stop();
    asioService->Stop();
    state = TcpServerState::WAITING_FOR_JOIN;
    LOG_DEBUG << "TCP thread ready to be joined.";
}

}  // namespace tcp