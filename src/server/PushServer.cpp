#include "PushServer.hpp"
#include "logger/Logger.hpp"
#include "wns/WnsClient.hpp"
#include <cassert>
#include <spdlog/spdlog.h>

namespace server {
PushServer::PushServer(const storage::Configuration& config) : wnsClient(config.wns) {}

PushServer::~PushServer() {
    stop();
}

void PushServer::start() {
    assert(state == PushServerState::NOT_RUNNING);
    SPDLOG_DEBUG("Starting the push server thread...");
    state = PushServerState::STARTING;
    serverThread = std::make_optional<std::thread>(&PushServer::threadRun, this);
}

void PushServer::stop() {
    if (state == PushServerState::STARTING || state == PushServerState::RUNNING) {
        SPDLOG_DEBUG("Stopping the push server thread...");
        state = PushServerState::STOP_REQUESTED;
        SPDLOG_DEBUG("Joining the push server thread...");
        serverThread->join();
        state = PushServerState::NOT_RUNNING;
        serverThread = std::nullopt;
        SPDLOG_INFO("Push server thread stopped.");
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

    // WNS:
    wnsClient.loadTokenFromDb();

    while (state == PushServerState::RUNNING) {
        check_setup_wns();
    }
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