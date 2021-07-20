#pragma once

#include "storage/ConfigurationStorage.hpp"
#include <optional>
#include <string>
#include <thread>

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

 public:
    explicit XmppClient(const storage::XmppConfiguration& config);
    XmppClient(XmppClient&&) = delete;
    XmppClient(const XmppClient&) = delete;
    XmppClient& operator=(XmppClient&&) = delete;
    XmppClient& operator=(const XmppClient&) = delete;
    ~XmppClient() override;

    XmppClientState getState();

    void start();
    void stop();

 private:
    void thread_run();
};
}  // namespace xmpp