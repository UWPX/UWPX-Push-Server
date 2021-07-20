#include "XmppClient.hpp"
#include "storage/Serializer.hpp"
#include <cassert>
#include <chrono>

namespace xmpp {
XmppClient::XmppClient(const storage::XmppConfiguration& /*config*/) {}

XmppClient::~XmppClient() {
    assert(state == XmppClientState::NOT_RUNNING);
}

XmppClient::XmppClientState XmppClient::getState() {
    return state;
}

void XmppClient::start() {
    assert(state == XmppClientState::NOT_RUNNING);
    SPDLOG_DEBUG("Starting the XMPP thread...");
    state = XmppClientState::STARTING;
    thread = std::make_optional<std::thread>(&XmppClient::thread_run, this);
}

void XmppClient::stop() {
    if (state == XmppClientState::STARTING || state == XmppClientState::RUNNING || state == XmppClientState::WAITING_FOR_JOIN) {
        if (state != XmppClientState::WAITING_FOR_JOIN) {
            SPDLOG_DEBUG("Stopping the XMPP thread...");
            state = XmppClientState::STOP_REQUESTED;
        }
        SPDLOG_DEBUG("Joining the XMPP thread...");
        thread->join();
        state = XmppClientState::NOT_RUNNING;
        thread = std::nullopt;
        SPDLOG_INFO("XMPP thread joined.");
    } else {
        SPDLOG_DEBUG("No need to stop the XMPP thread - not running (state: {})!", state);
    }
}

void XmppClient::thread_run() {
    assert(state == XmppClientState::STARTING || state == XmppClientState::STOP_REQUESTED);
    if (state != XmppClientState::STARTING) {
        return;
    }
    state = XmppClientState::RUNNING;
    SPDLOG_INFO("XMPP thread started.");
    SPDLOG_DEBUG("Connecting the XMPP client...");
    SPDLOG_DEBUG("XMPP client connected.");

    while (state == XmppClientState::RUNNING) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    SPDLOG_DEBUG("Disconnecting the XMPP client...");
    SPDLOG_DEBUG("XMPP client disconnected.");
    state = XmppClientState::WAITING_FOR_JOIN;
    SPDLOG_DEBUG("XMPP thread ready to be joined.");
}
}  // namespace xmpp