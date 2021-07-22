#include "XmppClient.hpp"
#include "logger/Logger.hpp"
#include "storage/Serializer.hpp"
#include <cassert>
#include <chrono>
#include <cstdint>
#include <strophe.h>

namespace xmpp {
XmppClient::XmppClient(const storage::XmppConfiguration& config) : jid(config.bareJid), password(config.password), pubSubServerJid(config.pubSubServerJid), port(config.port), host(config.host) {}

XmppClient::~XmppClient() { assert(state == XmppClientState::NOT_RUNNING); }

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

void conn_handler(xmpp_conn_t* const conn, const xmpp_conn_event_t status, const int /*error*/, xmpp_stream_error_t* const /*stream_error*/, void* const userdata) {
    XmppClient* client = static_cast<XmppClient*>(userdata);

    if (status == XMPP_CONN_CONNECT) {
        if (xmpp_conn_is_secured(conn)) {
            LOG_DEBUG << "XMPP connection is secured.";
        } else {
            LOG_DEBUG << "XMPP connection is NOT secured!";
        }
        // xmpp_disconnect(conn);
    } else {
        xmpp_stop(client->get_ctx());
    }
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
        xmpp_run(ctx);
        // Ensure we do not register the same handler multiple times:
        xmpp_handler_delete(conn, &iq_handler);
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

xmpp_stanza_t* XmppClient::xmpp_pub_sub_create_config_new() {
    xmpp_stanza_t* configure = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(configure, "configure");

    xmpp_stanza_t* x = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(x, "x");
    xmpp_stanza_set_ns(x, "jabber:x:data");
    xmpp_stanza_set_attribute(x, "type", "submit");

    xmpp_stanza_t* field = xmpp_field_new("FORM_TYPE", "hidden", "http://jabber.org/protocol/pubsub#node_config");
    xmpp_stanza_add_child(x, field);
    xmpp_stanza_release(field);

    field = xmpp_field_new("pubsub#access_model", "list-single", "whitelist");  // Specify the subscriber model
    xmpp_stanza_add_child(x, field);
    xmpp_stanza_release(field);
    field = xmpp_field_new("pubsub#publish_model", "open", "whitelist");  // Specify the publisher model
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

    xmpp_stanza_add_child(configure, x);
    xmpp_stanza_release(x);

    return configure;
}

xmpp_stanza_t* XmppClient::xmpp_pub_sub_create_new(const char* node, const char* id) {
    assert(ctx);

    xmpp_stanza_t* iq = xmpp_iq_new(ctx, "set", id);
    xmpp_stanza_set_to(iq, pubSubServerJid.c_str());
    xmpp_stanza_set_from(iq, xmpp_conn_get_bound_jid(conn));

    xmpp_stanza_t* pubSub = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(pubSub, "pubsub");
    xmpp_stanza_set_ns(pubSub, "http://jabber.org/protocol/pubsub");

    xmpp_stanza_t* create = xmpp_stanza_new(ctx);
    xmpp_stanza_set_name(create, "create");
    xmpp_stanza_set_attribute(create, "node", node);
    xmpp_stanza_add_child(pubSub, create);
    xmpp_stanza_release(create);

    xmpp_stanza_t* config = xmpp_pub_sub_create_config_new();
    xmpp_stanza_add_child(pubSub, config);
    xmpp_stanza_release(config);

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
    xmpp_stanza_t* iq = xmpp_pub_sub_create_new(node.c_str(), "someId");
    xmpp_send(conn, iq);
    xmpp_stanza_release(iq);
    return true;
}
}  // namespace xmpp