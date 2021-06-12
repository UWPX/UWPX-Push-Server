#include "Serializer.hpp"
#include "ConfigurationStorage.hpp"

#include <chrono>
#include <cstddef>
#include <functional>

namespace storage {

void to_json(nlohmann::json& j, const WnsConfiguration& c) {
    j = nlohmann::json{{"packetId", c.packetId},
                       {"clientSecret", c.clientSecret}};
}

void from_json(const nlohmann::json& j, WnsConfiguration& c) {
    j.at("packetId").get_to(c.packetId);
    j.at("clientSecret").get_to(c.clientSecret);
}

void to_json(nlohmann::json& j, const XmppConfiguration& c) {
    j = nlohmann::json{{"bareJid", c.bareJid},
                       {"password", c.password}};
}

void from_json(const nlohmann::json& j, XmppConfiguration& c) {
    j.at("bareJid").get_to(c.bareJid);
    j.at("password").get_to(c.password);
}

void to_json(nlohmann::json& j, const TlsConfiguration& c) {
    j = nlohmann::json{{"serverCertPath", c.serverCertPath},
                       {"serverKeyPath", c.serverKeyPath}};
}

void from_json(const nlohmann::json& j, TlsConfiguration& c) {
    j.at("serverCertPath").get_to(c.serverCertPath);
    j.at("serverKeyPath").get_to(c.serverKeyPath);
}

void to_json(nlohmann::json& j, const TcpConfiguration& c) {
    j = nlohmann::json{{"port", c.port},
                       {"tls", c.tls}};
}

void from_json(const nlohmann::json& j, TcpConfiguration& c) {
    j.at("port").get_to(c.port);
    j.at("tls").get_to(c.tls);
}

void to_json(nlohmann::json& j, const DbConfiguration& c) {
    j = nlohmann::json{{"url", c.url}};
}

void from_json(const nlohmann::json& j, DbConfiguration& c) {
    j.at("url").get_to(c.url);
}

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
