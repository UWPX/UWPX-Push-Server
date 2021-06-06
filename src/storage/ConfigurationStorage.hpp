#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>

#include "Serializer.hpp"

namespace storage {
struct WnsConfiguration {
    /**
     * The UWP package ID starting with: 'ms-app://...'.
     **/
    std::string packetId{"The UWP package ID starting with: 'ms-app://...'"};
    /**
     * The secret obtained from the Devcenter.
     **/
    std::string clientSecret{"The secret obtained from the Devcenter"};
};

struct XmppConfiguration {
    /**
     * The bare JID of the push XMPP client (e.g. 'pushServer@xmpp.example.com').
     **/
    std::string bareJid{"The bare JID of the push XMPP client (e.g. 'pushServer@xmpp.example.com')"};
    /**
     * The password for the push XMPP client.
     **/
    std::string password{"The password for the push XMPP client"};
};

struct TlsConfiguration {
    /**
     * Path to your 'domain.key' file.
     **/
    std::string serverKeyPath{"Path to your 'domain.key' file"};
    /**
     * Path to your 'domain.cert' file.
     **/
    std::string serverCertPath{"Path to your 'domain.cert' file"};
};

struct TcpConfiguration {
    /**
     * The port where the server should listen to requests from clients.
     * Default: 1997
     **/
    uint16_t port{1997};
    /**
     * All TLS related configuration options.
     **/
    TlsConfiguration tls{};
};

struct DbConfiguration {};

struct Configuration {
    /**
     * All WNS related configuration options.
     **/
    WnsConfiguration wns{};
    /**
     * All XMPP related configuration options.
     **/
    XmppConfiguration xmpp{};
    /**
     * All TCP related configuration options.
     **/
    TcpConfiguration tcp{};
    /**
     * All DB related configuration options.
     **/
    DbConfiguration db{};
};

struct ConfigurationStorage {
    explicit ConfigurationStorage(const std::filesystem::path& configFilePath);

 private:
    storage::Serializer fileHandle;
    Configuration config{};

    void write_configuration();
};

ConfigurationStorage& get_configuration_storage_instance();
}  // namespace storage