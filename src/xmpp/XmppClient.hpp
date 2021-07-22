#pragma once

#include "storage/ConfigurationStorage.hpp"
#include <optional>
#include <string>
#include <thread>
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
    const std::string pubSubServerJid;
    const uint16_t port;
    const std::string host;

 public:
    explicit XmppClient(const storage::XmppConfiguration& config);
    XmppClient(XmppClient&&) = delete;
    XmppClient(const XmppClient&) = delete;
    XmppClient& operator=(XmppClient&&) = delete;
    XmppClient& operator=(const XmppClient&) = delete;
    ~XmppClient();

    [[nodiscard]] XmppClientState get_state() const;
    [[nodiscard]] const std::string& get_jid() const;
    [[nodiscard]] xmpp_ctx_t* get_ctx() const;

    void start();
    void stop();

    /**
     * Creates the given PubSub node and subscribes to it.
     * The node will be configured in a way, that others can publish to it.
     *
     * Note: When using prosody, the user has to be an admin to create nodes.
     * https://prosody.im/doc/modules/mod_pubsub
     * 
     * Returns true on success.
     **/
    bool setup_push_node(const std::string& node);

 private:
    void thread_run();
    void setup_xmpp();
    void cleanup_xmpp();

    xmpp_stanza_t* xmpp_pub_sub_create_new(const char* node, const char* id);
    xmpp_stanza_t* xmpp_pub_sub_create_config_new();
    xmpp_stanza_t* xmpp_field_new(const char* var, const char* type, const char* value);
};
}  // namespace xmpp