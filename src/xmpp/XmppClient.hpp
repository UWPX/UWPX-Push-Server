#pragma once

#include "storage/ConfigurationStorage.hpp"
#include <string>
#include <QXmppClient.h>
#include <QXmppMessage.h>

namespace xmpp {
class XmppClient : private QXmppClient {
 private:
    const QString jid;
    const QString password;

 public:
    explicit XmppClient(const storage::XmppConfiguration& config);
    XmppClient(XmppClient&&) = delete;
    XmppClient(const XmppClient&) = delete;
    XmppClient& operator=(XmppClient&&) = delete;
    XmppClient& operator=(const XmppClient&) = delete;
    ~XmppClient() override = default;

    void init();

 private:
 public slots:
    void messageReceived(const QXmppMessage&);
};
}  // namespace xmpp