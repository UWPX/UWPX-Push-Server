#include "TcpServer.hpp"
#include "logger/Logger.hpp"
#include <chrono>

namespace tcp {
TcpServer::TcpServer(const storage::TcpConfiguration& config) : port(config.port) {}

TcpServer::~TcpServer() {
    assert(state == TcpServerState::NOT_RUNNING);
}

TcpServer::TcpServerState TcpServer::getState() {
    return state;
}

void TcpServer::start() {
    assert(state == TcpServerState::NOT_RUNNING);
    SPDLOG_DEBUG("Starting the TCP thread...");
    state = TcpServerState::STARTING;
    thread = std::make_optional<std::thread>(&TcpServer::threadRun, this);
}

void TcpServer::stop() {
    if (state == TcpServerState::STARTING || state == TcpServerState::RUNNING || state == TcpServerState::WAITING_FOR_JOIN) {
        if (state != TcpServerState::WAITING_FOR_JOIN) {
            SPDLOG_DEBUG("Stopping the TCP thread...");
            state = TcpServerState::STOP_REQUESTED;
        }
        SPDLOG_DEBUG("Joining the TCP thread...");
        thread->join();
        state = TcpServerState::NOT_RUNNING;
        thread = std::nullopt;
        SPDLOG_INFO("TCP thread joined.");
    } else {
        SPDLOG_DEBUG("No need to stop the TCP thread - not running (state: {})!", state);
    }
}

void TcpServer::threadRun() {
    assert(state == TcpServerState::STARTING || state == TcpServerState::STOP_REQUESTED);
    if (state != TcpServerState::STARTING) {
        return;
    }
    state = TcpServerState::RUNNING;
    SPDLOG_INFO("TCP thread started.");

    while (state == TcpServerState::RUNNING) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    state = TcpServerState::WAITING_FOR_JOIN;
    SPDLOG_DEBUG("TCP thread ready to be joined.");
}

}  // namespace tcp