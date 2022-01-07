#include "XmppClient.hpp"
#include "logger/Logger.hpp"
#include "storage/Serializer.hpp"
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <optional>
#include <strophe.h>

namespace xmpp {
XmppClient::XmppClient(const storage::XmppConfiguration& config, nodev1MessageHandlerFunc&& nodev1MessageHandler, nodev2MessageHandlerFunc&& nodev2MessageHandler) : jid(config.bareJid),
                                                                                                                                                                     password(config.password),
                                                                                                                                                                     port(config.port),
                                                                                                                                                                     host(config.host),
                                                                                                                                                                     nodev1MessageHandler(std::move(nodev1MessageHandler)),
                                                                                                                                                                     nodev2MessageHandler(std::move(nodev2MessageHandler)) {}

XmppClient::~XmppClient() { assert(state == XmppClientState::NOT_RUNNING); }

void XmppClient::set_redis_client(storage::redis::RedisClient* redisClient) {
    this->redisClient = redisClient;
}

storage::redis::RedisClient* XmppClient::get_redis_client() const { return redisClient; }

XmppClient::XmppClientState XmppClient::get_state() const { return state; }

const std::string& XmppClient::get_jid() const { return jid; }

xmpp_ctx_t* XmppClient::get_ctx() const { return ctx; }

void XmppClient::start() {
    assert(state == XmppClientState::NOT_RUNNING);
    LOG_DEBUG << "Starting the XMPP thread...";
    state = XmppClientState::STARTING;
    thread = std::make_optional<std::thread>(&XmppClient::thread_run, this);
}

void XmppClient::stop() {
    if (state == XmppClientState::STARTING || state == XmppClientState::RUNNING || state == XmppClientState::WAITING_FOR_JOIN) {
        if (state != XmppClientState::WAITING_FOR_JOIN) {
            LOG_DEBUG << "Stopping the XMPP thread...";
            state = XmppClientState::STOP_REQUESTED;
            xmpp_stop(ctx);
        }
        LOG_DEBUG << "Joining the XMPP thread...";
        thread->join();
        state = XmppClientState::NOT_RUNNING;
        thread = std::nullopt;
        LOG_INFO << "XMPP thread joined.";
    } else {
        LOG_DEBUG << "No need to stop the XMPP thread - not running (state: " << static_cast<int>(state) << ")!";
    }
}

int iq_handler(xmpp_conn_t* const /*conn*/, xmpp_stanza_t* const stanza, void* const userdata) {
    XmppClient* const client = static_cast<XmppClient*>(userdata);
    char* buf = nullptr;
    size_t len = 0;
    if (xmpp_stanza_to_text(stanza, &buf, &len) == 0) {
        LOG_DEBUG << "Received: " << buf;
        xmpp_free(client->get_ctx(), buf);
    }

    // Return 0 to be removed:
    return 1;
}

void XmppClient::send_v2_push(const std::string& accountId, const std::string& node, xmpp_stanza_t* notificationNode) {
    // Parse message and pending subscription count:
    int messageCount = 0;
    int pendingSubscriptionCount = 0;
    xmpp_stanza_t* xNode = xmpp_stanza_get_child_by_name(notificationNode, "x");
    if (xNode) {
        for (xmpp_stanza_t* fieldNode = xmpp_stanza_get_children(xNode); fieldNode; fieldNode = xmpp_stanza_get_next(fieldNode)) {
            std::string name = xmpp_stanza_get_name(fieldNode);
            if (std::strcmp(xmpp_stanza_get_name(fieldNode), "field") == 0) {
                const char* var = xmpp_stanza_get_attribute(fieldNode, "var");
                if (var) {
                    if (std::strcmp(var, "message-count") == 0) {
                        xmpp_stanza_t* valueNode = xmpp_stanza_get_child_by_name(fieldNode, "value");
                        if (valueNode) {
                            char* valueStr = xmpp_stanza_get_text(valueNode);
                            if (valueStr) {
                                try {
                                    messageCount = std::stoi(valueStr);
                                } catch (const std::invalid_argument&) {}
                                xmpp_free(ctx, valueStr);
                            }
                        }
                    } else if (std::strcmp(var, "pending-subscription-count") == 0) {
                        xmpp_stanza_t* valueNode = xmpp_stanza_get_child_by_name(fieldNode, "value");
                        if (valueNode) {
                            char* valueStr = xmpp_stanza_get_text(valueNode);
                            if (valueStr) {
                                try {
                                    pendingSubscriptionCount = std::stoi(valueStr);
                                } catch (const std::invalid_argument&) {}
                                xmpp_free(ctx, valueStr);
                            }
                        }
                    }
                }
            }
        }
    }

    // Forward result:
    on_v2_node_message(node, accountId, messageCount, pendingSubscriptionCount);
}

void XmppClient::send_v1_push(const std::string& accountId, const std::string& node, xmpp_stanza_t* notificationNode) {
    // Add the 'account' attribute node:
    xmpp_stanza_t* accountNode = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(accountNode, "account");
    xmpp_stanza_set_attribute(accountNode, "id", accountId.c_str());
    xmpp_stanza_add_child(notificationNode, accountNode);

    // Convert the 'notification' node to a string:
    char* msg = nullptr;
    size_t len = 0;
    if (xmpp_stanza_to_text(notificationNode, &msg, &len) != 0) {
        LOG_WARNING << "Failed to convert the 'notification' node to a string...";
    }
    /**
     * Raw notifications can have only a size of less than 5 KB.
     * Reference: https://docs.microsoft.com/en-us/previous-versions/windows/apps/jj676791(v=win.10)#creating-a-raw-notification
     **/
    else if (len > 4096) {
        on_v1_node_message(node, "New message received!");
    } else {
        // Trigger the new message for node event:
        on_v1_node_message(node, msg);
    }
    xmpp_free(ctx, msg);
}

int message_handler(xmpp_conn_t* const /*conn*/, xmpp_stanza_t* const stanza, void* const userdata) {
    XmppClient* const client = static_cast<XmppClient*>(userdata);

    const char* from = xmpp_stanza_get_attribute(stanza, "from");
    if (std::strcmp(from, client->get_jid().c_str()) == 0) {
        xmpp_stanza_t* itemsNode = XmppClient::get_items_node(stanza);
        if (itemsNode) {
            xmpp_stanza_t* notificationNode = XmppClient::get_notification_node(itemsNode);
            if (notificationNode) {
                const char* node = xmpp_stanza_get_attribute(itemsNode, "node");
                LOG_DEBUG << "Received valid event for node: " << node;

                // Get the accountId:
                storage::redis::RedisClient* redisClient = client->get_redis_client();
                assert(redisClient);
                std::optional<std::string> accountId = redisClient->get_account_id(node);
                if (accountId) {
                    std::optional<std::string> deviceId = redisClient->get_device_id(node);
                    std::optional<std::string> version = std::nullopt;
                    if (deviceId) {
                        version = redisClient->get_version(*deviceId);
                    }
                    if (version && version == "2") {
                        LOG_DEBUG << "Sending v2 push message.";
                        client->send_v2_push(*accountId, node, notificationNode);
                    } else {
                        LOG_DEBUG << "Sending v1 push message.";
                        client->send_v1_push(*accountId, node, notificationNode);
                    }
                } else {
                    LOG_WARNING << "No account for node (" << node << ") not found!";
                }
            } else {
                LOG_WARNING << "Invalid message node. 'notification' node not found!";
            }
        } else {
            LOG_WARNING << "Invalid message node. 'items' node not found!";
        }
    } else {
        LOG_WARNING << "Invalid message node. 'from' does not match!";
        LOG_DEBUG << "Expected '" << client->get_jid() << "', but received '" << from << "'.";
    }

    // Return 0 to be removed:
    return 1;
}

xmpp_stanza_t* XmppClient::get_notification_node(xmpp_stanza_t* itemsNode) {
    xmpp_stanza_t* itemNode = xmpp_stanza_get_child_by_name(itemsNode, "item");
    if (itemNode) {
        xmpp_stanza_t* notificationNode = xmpp_stanza_get_child_by_name(itemNode, "notification");
        if (notificationNode) {
            const char* notificationNs = xmpp_stanza_get_ns(notificationNode);
            if (std::strcmp(notificationNs, "urn:xmpp:push:0") == 0) {
                return notificationNode;
            }
        }
    }
    return nullptr;
}

xmpp_stanza_t* XmppClient::get_items_node(xmpp_stanza_t* stanza) {
    xmpp_stanza_t* event = xmpp_stanza_get_child_by_name(stanza, "event");
    if (event) {
        const char* eventNs = xmpp_stanza_get_ns(event);
        if (std::strcmp(eventNs, "http://jabber.org/protocol/pubsub#event") == 0) {
            return xmpp_stanza_get_child_by_name(event, "items");
        }
    }
    return nullptr;
}

void XmppClient::on_v1_node_message(const std::string& node, const std::string& msg) {
    assert(nodev1MessageHandler);
    nodev1MessageHandler(node, msg);
}

void XmppClient::on_v2_node_message(const std::string& node, const std::string& accountId, int messageCount, int pendingSubscriptionCount) {
    assert(nodev2MessageHandler);
    nodev2MessageHandler(node, accountId, messageCount, pendingSubscriptionCount);
}

void conn_handler(xmpp_conn_t* const conn, const xmpp_conn_event_t status, const int /*error*/, xmpp_stream_error_t* const /*stream_error*/, void* const userdata) {
    XmppClient* client = static_cast<XmppClient*>(userdata);

    if (status == XMPP_CONN_CONNECT) {
        if (xmpp_conn_is_secured(conn)) {
            LOG_DEBUG << "XMPP connection is secured.";
        } else {
            LOG_DEBUG << "XMPP connection is NOT secured!";
        }
        client->send_presence_online();
        // xmpp_disconnect(conn);
    } else {
        xmpp_stop(client->get_ctx());
    }
}

void XmppClient::send_presence_online() {
    xmpp_stanza_t* presence = xmpp_presence_new(ctx);
    xmpp_stanza_set_from(presence, xmpp_conn_get_bound_jid(conn));
    xmpp_send(conn, presence);
}

void XmppClient::thread_run() {
    assert(state == XmppClientState::STARTING || state == XmppClientState::STOP_REQUESTED);
    if (state != XmppClientState::STARTING) {
        return;
    }
    state = XmppClientState::RUNNING;
    LOG_INFO << "XMPP thread started.";
    LOG_DEBUG << "Connecting the XMPP client...";
    setup_xmpp();
    LOG_DEBUG << "XMPP client connected.";

    while (state == XmppClientState::RUNNING) {
        xmpp_connect_client(conn, host.c_str(), port, conn_handler, this);
        xmpp_handler_add(conn, iq_handler, nullptr, "iq", nullptr, this);
        xmpp_handler_add(conn, message_handler, nullptr, "message", nullptr, this);
        xmpp_run(ctx);
        // Ensure we do not register the same handler multiple times:
        xmpp_handler_delete(conn, &iq_handler);
        xmpp_handler_delete(conn, &message_handler);
        if (state == XmppClientState::RUNNING) { LOG_INFO << "XMPP client disconnected. Reconnecting in 10 seconds..."; }
        for (size_t i = 0; i < 20 && state == XmppClientState::RUNNING; i++) { std::this_thread::sleep_for(std::chrono::milliseconds(500)); }
    }
    LOG_DEBUG << "Disconnecting the XMPP client...";
    cleanup_xmpp();
    LOG_DEBUG << "XMPP client disconnected.";
    state = XmppClientState::WAITING_FOR_JOIN;
    LOG_DEBUG << "XMPP thread ready to be joined.";
}

void XmppClient::setup_xmpp() {
    xmpp_initialize();
    log = xmpp_get_default_logger(XMPP_LEVEL_INFO);
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

xmpp_stanza_t* XmppClient::xmpp_field_new(const char* var, const char* type, const char* value) {
    xmpp_stanza_t* field = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(field, "field");

    assert(var);
    xmpp_stanza_set_attribute(field, "var", var);
    if (type) {
        xmpp_stanza_set_attribute(field, "type", type);
    }

    xmpp_stanza_t* valueNode = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(valueNode, "value");
    xmpp_stanza_t* valueText = xmpp_stanza_new(ctx);
    assert(value);
    xmpp_stanza_set_text(valueText, value);
    xmpp_stanza_add_child(valueNode, valueText);
    xmpp_stanza_release(valueText);
    xmpp_stanza_add_child(field, valueNode);

    xmpp_stanza_release(valueNode);

    return field;
}

xmpp_stanza_t* XmppClient::xmpp_pep_delete_new(const char* node, const char* id) {
    assert(ctx);

    xmpp_stanza_t* iq = xmpp_iq_new(ctx, "set", id);
    xmpp_stanza_set_from(iq, xmpp_conn_get_bound_jid(conn));

    xmpp_stanza_t* pubSub = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(pubSub, "pubsub");
    xmpp_stanza_set_ns(pubSub, "http://jabber.org/protocol/pubsub");

    xmpp_stanza_t* deleteNode = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(deleteNode, "delete");
    xmpp_stanza_set_attribute(deleteNode, "node", node);
    xmpp_stanza_add_child(pubSub, deleteNode);
    xmpp_stanza_release(deleteNode);

    xmpp_stanza_add_child(iq, pubSub);

    // We can release the stanza since it belongs to iq now.
    xmpp_stanza_release(pubSub);
    return iq;
}

bool XmppClient::setup_push_node(const std::string& node) {
    assert(conn);
    if (!xmpp_conn_is_connected(conn)) {
        return false;
    }

    // Create:
    xmpp_stanza_t* iq = xmpp_pep_publish_empty_new(node.c_str(), "somePepCreateId");
    xmpp_send(conn, iq);
    xmpp_stanza_release(iq);

    // Subscribe:
    iq = xmpp_pep_subscribe_new(node.c_str(), "someSubscribeId");
    xmpp_send(conn, iq);
    xmpp_stanza_release(iq);
    return true;
}

void XmppClient::delete_push_node(const std::string& node) {
    assert(conn);
    if (!xmpp_conn_is_connected(conn)) {
        return;
    }

    // Unsubscribe:
    xmpp_stanza_t* iq = xmpp_pep_unsubscribe_new(node.c_str(), "someUnsubscribeId");
    xmpp_send(conn, iq);

    // Delete:
    iq = xmpp_pep_delete_new(node.c_str(), "someDeleteId");
    xmpp_send(conn, iq);
    xmpp_stanza_release(iq);
}

xmpp_stanza_t* XmppClient::xmpp_pep_subscribe_new(const char* node, const char* id) {
    assert(ctx);

    xmpp_stanza_t* iq = xmpp_iq_new(ctx, "set", id);
    xmpp_stanza_set_from(iq, xmpp_conn_get_bound_jid(conn));

    xmpp_stanza_t* pubSub = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(pubSub, "pubsub");
    xmpp_stanza_set_ns(pubSub, "http://jabber.org/protocol/pubsub");

    xmpp_stanza_t* subscribe = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(subscribe, "subscribe");
    xmpp_stanza_set_attribute(subscribe, "node", node);
    xmpp_stanza_set_attribute(subscribe, "jid", jid.c_str());
    xmpp_stanza_add_child(pubSub, subscribe);
    xmpp_stanza_release(subscribe);

    xmpp_stanza_add_child(iq, pubSub);

    // We can release the stanza since it belongs to iq now.
    xmpp_stanza_release(pubSub);
    return iq;
}

xmpp_stanza_t* XmppClient::xmpp_pep_publish_empty_new(const char* node, const char* id) {
    assert(ctx);

    xmpp_stanza_t* iq = xmpp_iq_new(ctx, "set", id);
    xmpp_stanza_set_from(iq, xmpp_conn_get_bound_jid(conn));

    xmpp_stanza_t* pubSub = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(pubSub, "pubsub");
    xmpp_stanza_set_ns(pubSub, "http://jabber.org/protocol/pubsub");

    xmpp_stanza_t* publish = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(publish, "publish");
    xmpp_stanza_set_attribute(publish, "node", node);
    xmpp_stanza_t* item = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(item, "item");
    xmpp_stanza_t* notification = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(notification, "notification");
    xmpp_stanza_set_ns(notification, "urn:xmpp:push:0");
    xmpp_stanza_add_child(item, notification);
    xmpp_stanza_add_child(publish, item);
    xmpp_stanza_add_child(pubSub, publish);
    xmpp_stanza_release(publish);

    xmpp_stanza_t* options = xmpp_pep_publish_options_new();
    xmpp_stanza_add_child(pubSub, options);
    xmpp_stanza_release(options);

    xmpp_stanza_add_child(iq, pubSub);

    xmpp_stanza_release(pubSub);
    return iq;
}

xmpp_stanza_t* XmppClient::xmpp_pep_publish_options_new() {
    xmpp_stanza_t* options = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(options, "publish-options");

    xmpp_stanza_t* x = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(x, "x");
    xmpp_stanza_set_ns(x, "jabber:x:data");
    xmpp_stanza_set_attribute(x, "type", "submit");

    xmpp_stanza_t* field = xmpp_field_new("FORM_TYPE", "hidden", "http://jabber.org/protocol/pubsub#publish-options");
    xmpp_stanza_add_child(x, field);
    xmpp_stanza_release(field);

    field = xmpp_field_new("pubsub#access_model", "list-single", "open");  // Specify the subscriber model
    xmpp_stanza_add_child(x, field);
    xmpp_stanza_release(field);
    field = xmpp_field_new("pubsub#publish_model", "list-single", "open");  // Specify the publisher model
    xmpp_stanza_add_child(x, field);
    xmpp_stanza_release(field);
    field = xmpp_field_new("pubsub#deliver_notifications", "boolean", "1");  // Whether to deliver event notifications
    xmpp_stanza_add_child(x, field);
    xmpp_stanza_release(field);
    field = xmpp_field_new("pubsub#deliver_payloads", "boolean", "1");  // Whether to deliver payloads with event notifications
    xmpp_stanza_add_child(x, field);
    xmpp_stanza_release(field);
    field = xmpp_field_new("pubsub#persist_items", "boolean", "1");  // Persist items to storage
    xmpp_stanza_add_child(x, field);
    xmpp_stanza_release(field);
    field = xmpp_field_new("pubsub#subscribe", "boolean", "1");  // Whether to allow subscriptions
    xmpp_stanza_add_child(x, field);
    xmpp_stanza_release(field);
    field = xmpp_field_new("pubsub#presence_based_delivery", "boolean", "0");  // Deliver event notifications only to available users
    xmpp_stanza_add_child(x, field);
    xmpp_stanza_release(field);
    field = xmpp_field_new("pubsub#notification_type", "list-single", "normal");  // Specify the delivery style for event notifications
    xmpp_stanza_add_child(x, field);
    xmpp_stanza_release(field);

    xmpp_stanza_add_child(options, x);
    xmpp_stanza_release(x);

    return options;
}

xmpp_stanza_t* XmppClient::xmpp_pep_unsubscribe_new(const char* node, const char* id) {
    assert(ctx);

    xmpp_stanza_t* iq = xmpp_iq_new(ctx, "set", id);
    xmpp_stanza_set_from(iq, xmpp_conn_get_bound_jid(conn));

    xmpp_stanza_t* pubSub = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(pubSub, "pubsub");
    xmpp_stanza_set_ns(pubSub, "http://jabber.org/protocol/pubsub");

    xmpp_stanza_t* unsubscribe = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(unsubscribe, "unsubscribe");
    xmpp_stanza_set_attribute(unsubscribe, "node", node);
    xmpp_stanza_set_attribute(unsubscribe, "jid", jid.c_str());
    xmpp_stanza_add_child(pubSub, unsubscribe);
    xmpp_stanza_release(unsubscribe);

    xmpp_stanza_add_child(iq, pubSub);

    // We can release the stanza since it belongs to iq now.
    xmpp_stanza_release(pubSub);
    return iq;
}

}  // namespace xmpp