#include "PushServer.hpp"
#include "logger/Logger.hpp"
#include "wns/WnsClient.hpp"
#include <cassert>
#include <spdlog/spdlog.h>

namespace server {
PushServer::PushServer(const storage::Configuration& config) : wnsClient(config.wns), tcpServer(config.tcp) {}

PushServer::~PushServer() {
    assert(state == PushServerState::NOT_RUNNING);
}

void PushServer::start() {
    assert(state == PushServerState::NOT_RUNNING);
    SPDLOG_DEBUG("Starting the push server thread...");
    state = PushServerState::STARTING;
    serverThread = std::make_optional<std::thread>(&PushServer::threadRun, this);
}

void PushServer::stop() {
    if (state == PushServerState::STARTING || state == PushServerState::RUNNING || state == PushServerState::WAITING_FOR_JOIN) {
        if (state != PushServerState::WAITING_FOR_JOIN) {
            SPDLOG_DEBUG("Stopping the push server thread...");
            state = PushServerState::STOP_REQUESTED;
        }
        SPDLOG_DEBUG("Joining the push server thread...");
        serverThread->join();
        state = PushServerState::NOT_RUNNING;
        serverThread = std::nullopt;
        SPDLOG_INFO("Push server thread joined.");
    } else {
        SPDLOG_DEBUG("No need to stop the push server thread - not running (state: {})!", state);
    }
}

void PushServer::threadRun() {
    assert(state == PushServerState::STARTING || state == PushServerState::STOP_REQUESTED);
    if (state != PushServerState::STARTING) {
        return;
    }
    state = PushServerState::RUNNING;
    SPDLOG_INFO("Push server thread started.");

    wnsClient.loadTokenFromDb();
    tcpServer.start();

    while (state == PushServerState::RUNNING) {
        check_setup_wns();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    tcpServer.stop();
    state = PushServerState::WAITING_FOR_JOIN;
    SPDLOG_INFO("Push server thread ready to be joined.");
}

void PushServer::check_setup_wns() {
    if (!wnsClient.isTokenValid()) {
        if (!wnsClient.requestToken()) {
            SPDLOG_INFO("Retrying to request a new WNS token...");
            if (!wnsClient.requestToken()) {
                SPDLOG_ERROR("Failed to request a new WNS token for te second time! Exiting...");
            }
        }
    }
}
}  // namespace server