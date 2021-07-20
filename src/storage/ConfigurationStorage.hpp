#pragma once

#include "Serializer.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>
#include <bits/stdint-uintn.h>

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
    /**
     * The port of the XMPP server.
     **/
    uint16_t port = 5222;
    /**
     * The hostname of the XMPP server.
     **/
    std::string host{"The hostname of the XMPP server."};
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

struct DbConfiguration {
    /**
     * Uri to connect to the Redis server.
     * e.g. 'tcp://127.0.0.1', 'tcp://127.0.0.1:6379', or 'unix://path/to/socket'.
     * Full URI scheme: 'tcp://[[username:]password@]host[:port][/db]' or
     * unix://[[username:]password@]path-to-unix-domain-socket[/db]
     **/
    std::string url;
};

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

 public:
    Configuration config{};

 private:
    storage::Serializer fileHandle;

    void write_configuration();
};

ConfigurationStorage& get_configuration_storage_instance(const std::string& path);
}  // namespace storage