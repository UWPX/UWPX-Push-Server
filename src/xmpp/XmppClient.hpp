#pragma once

#include "storage/ConfigurationStorage.hpp"
#include <optional>
#include <string>
#include <thread>
#include <bits/stdint-uintn.h>
#include <strophe.h>

namespace xmpp {
class XmppClient {
 public:
    enum class XmppClientState {
        NOT_RUNNING,
        STARTING,
        RUNNING,
        STOP_REQUESTED,
        WAITING_FOR_JOIN,
    };

 private:
    std::optional<std::thread> thread{std::nullopt};
    XmppClientState state{XmppClientState::NOT_RUNNING};

    xmpp_log_t* log{nullptr};
    xmpp_ctx_t* ctx{nullptr};
    xmpp_conn_t* conn{nullptr};

    const std::string jid;
    const std::string password;
    const uint16_t port;
    const std::string host;

 public:
    explicit XmppClient(const storage::XmppConfiguration& config);
    XmppClient(XmppClient&&) = delete;
    XmppClient(const XmppClient&) = delete;
    XmppClient& operator=(XmppClient&&) = delete;
    XmppClient& operator=(const XmppClient&) = delete;
    ~XmppClient();

    XmppClientState getState();

    void start();
    void stop();

 private:
    void thread_run();
    void setup_xmpp();
    void cleanup_xmpp();
};
}  // namespace xmpp