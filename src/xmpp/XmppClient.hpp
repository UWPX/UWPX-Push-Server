#pragma once

#include "storage/ConfigurationStorage.hpp"
#include <optional>
#include <storage/redis/RedisClient.hpp>
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

    using nodev1MessageHandlerFunc = std::function<void(const std::string& node, const std::string& msg)>;
    using nodev2MessageHandlerFunc = std::function<void(const std::string& node, const std::string& accountId, int messageCount, int pendingSubscriptionCount)>;

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
    nodev1MessageHandlerFunc nodev1MessageHandler;
    nodev2MessageHandlerFunc nodev2MessageHandler;

    storage::redis::RedisClient* redisClient{nullptr};

 public:
    explicit XmppClient(const storage::XmppConfiguration& config, nodev1MessageHandlerFunc&& nodev1MessageHandler, nodev2MessageHandlerFunc&& nodev2MessageHandler);
    XmppClient(XmppClient&&) = delete;
    XmppClient(const XmppClient&) = delete;
    XmppClient& operator=(XmppClient&&) = delete;
    XmppClient& operator=(const XmppClient&) = delete;
    ~XmppClient();

    [[nodiscard]] XmppClientState get_state() const;
    [[nodiscard]] const std::string& get_jid() const;
    [[nodiscard]] xmpp_ctx_t* get_ctx() const;
    [[nodiscard]] storage::redis::RedisClient* get_redis_client() const;
    void set_redis_client(storage::redis::RedisClient* redisClient);

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
    void delete_push_node(const std::string& node);
    void send_presence_online();
    void on_v1_node_message(const std::string& node, const std::string& msg);
    void on_v2_node_message(const std::string& node, const std::string& accountId, int messageCount, int pendingSubscriptionCount);
    static xmpp_stanza_t* get_items_node(xmpp_stanza_t* stanza);
    static xmpp_stanza_t* get_notification_node(xmpp_stanza_t* itemsNode);
    void send_v1_push(const std::string& accountId, const std::string& node, xmpp_stanza_t* notificationNode);
    void send_v2_push(const std::string& accountId, const std::string& node, xmpp_stanza_t* notificationNode);

 private:
    void thread_run();
    void setup_xmpp();
    void cleanup_xmpp();

    xmpp_stanza_t* xmpp_pep_delete_new(const char* node, const char* id);
    xmpp_stanza_t* xmpp_pep_subscribe_new(const char* node, const char* id);
    xmpp_stanza_t* xmpp_pep_unsubscribe_new(const char* node, const char* id);
    xmpp_stanza_t* xmpp_pep_publish_empty_new(const char* node, const char* id);
    xmpp_stanza_t* xmpp_pep_publish_options_new();
    xmpp_stanza_t* xmpp_field_new(const char* var, const char* type, const char* value);
};
}  // namespace xmpp