#include "XmppClient.hpp"
#include "storage/Serializer.hpp"
#include <cassert>
#include <chrono>
#include <cstdint>
#include <spdlog/spdlog.h>
#include <strophe.h>

namespace xmpp {
XmppClient::XmppClient(const storage::XmppConfiguration& config) : jid(config.bareJid), password(config.password), port(config.port), host(config.host) {}

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

void conn_handler(xmpp_conn_t* const conn, const xmpp_conn_event_t status, const int error, xmpp_stream_error_t* const stream_error, void* const userdata) {
    xmpp_ctx_t* ctx = static_cast<xmpp_ctx_t*>(userdata);
    int secured = 0;

    (void) error;
    (void) stream_error;

    if (status == XMPP_CONN_CONNECT) {
        SPDLOG_DEBUG("[XMPP]: Connected.");
        secured = xmpp_conn_is_secured(conn);
        if (secured) {
            SPDLOG_DEBUG("[XMPP]: Connection is secured.");
        } else {
            SPDLOG_DEBUG("[XMPP]: Connection is NOT secured!");
        }
        SPDLOG_DEBUG("[XMPP]: Connected");
        // xmpp_disconnect(conn);
    } else {
        SPDLOG_DEBUG("[XMPP]: Disconnected.");
        xmpp_stop(ctx);
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
    setup_xmpp();
    SPDLOG_DEBUG("XMPP client connected.");

    while (state == XmppClientState::RUNNING) {
        xmpp_connect_client(conn, host.c_str(), port, conn_handler, ctx);
        xmpp_run(ctx);
    }
    SPDLOG_DEBUG("Disconnecting the XMPP client...");
    cleanup_xmpp();
    SPDLOG_DEBUG("XMPP client disconnected.");
    state = XmppClientState::WAITING_FOR_JOIN;
    SPDLOG_DEBUG("XMPP thread ready to be joined.");
}

void XmppClient::setup_xmpp() {
    SPDLOG_DEBUG("XMPP Password: {}", password.c_str());
    xmpp_initialize();
    log = xmpp_get_default_logger(XMPP_LEVEL_DEBUG);
    ctx = xmpp_ctx_new(nullptr, log);
    conn = xmpp_conn_new(ctx);
    xmpp_conn_set_flags(conn, XMPP_CONN_FLAG_MANDATORY_TLS | XMPP_CONN_FLAG_TRUST_TLS);
    xmpp_conn_set_keepalive(conn, 60, 1);  // Once per minute
    xmpp_conn_set_jid(conn, jid.c_str());
    xmpp_conn_set_pass(conn, password.c_str());
}

void XmppClient::cleanup_xmpp() {
    assert(conn);
    assert(ctx);
    xmpp_conn_release(conn);
    xmpp_ctx_free(ctx);
    xmpp_shutdown();
    conn = nullptr;
    ctx = nullptr;
}
}  // namespace xmpp