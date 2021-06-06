#include "Serializer.hpp"
#include "ConfigurationStorage.hpp"

#include <chrono>
#include <cstddef>
#include <functional>

namespace storage {

void to_json(nlohmann::json& j, const WnsConfiguration& c) {
    j = nlohmann::json{{"packet_id", c.packet_id},
                       {"client_secret", c.client_secret}};
}

void from_json(const nlohmann::json& j, WnsConfiguration& c) {
    j.at("packet_id").get_to(c.packet_id);
    j.at("client_secret").get_to(c.client_secret);
}

void to_json(nlohmann::json& j, const XmppConfiguration& c) {
    j = nlohmann::json{{"bare_jid", c.bare_jid},
                       {"password", c.password}};
}

void from_json(const nlohmann::json& j, XmppConfiguration& c) {
    j.at("bare_jid").get_to(c.bare_jid);
    j.at("password").get_to(c.password);
}

void to_json(nlohmann::json& j, const TlsConfiguration& c) {
    j = nlohmann::json{{"server_cert_path", c.server_cert_path},
                       {"server_key_path", c.server_key_path}};
}

void from_json(const nlohmann::json& j, TlsConfiguration& c) {
    j.at("server_cert_path").get_to(c.server_cert_path);
    j.at("server_key_path").get_to(c.server_key_path);
}

void to_json(nlohmann::json& j, const TcpConfiguration& c) {
    j = nlohmann::json{{"port", c.port},
                       {"tls", c.tls}};
}

void from_json(const nlohmann::json& j, TcpConfiguration& c) {
    j.at("port").get_to(c.port);
    j.at("tls").get_to(c.tls);
}

void to_json(nlohmann::json& j, const DbConfiguration& /*c*/) {
    j = nlohmann::json{};
}

void from_json(const nlohmann::json& /*j*/, DbConfiguration& /*c*/) {}

void to_json(nlohmann::json& j, const Configuration& c) {
    j = nlohmann::json{{"wns", c.wns},
                       {"xmpp", c.xmpp},
                       {"tcp", c.tcp},
                       {"db", c.db}};
}

void from_json(const nlohmann::json& j, Configuration& c) {
    j.at("wns").get_to(c.wns);
    j.at("xmpp").get_to(c.xmpp);
    j.at("tcp").get_to(c.tcp);
    j.at("db").get_to(c.db);
}
}  // namespace storage

namespace nlohmann {
void adl_serializer<std::chrono::system_clock::time_point>::to_json(json& j, const std::chrono::system_clock::time_point& tp) {
    j = std::chrono::system_clock::to_time_t(tp);
}

void adl_serializer<std::chrono::system_clock::time_point>::from_json(const json& j, std::chrono::system_clock::time_point& tp) {
    std::time_t t = 0;
    j.get_to(t);
    tp = std::chrono::system_clock::from_time_t(t);
}
}  // namespace nlohmann
