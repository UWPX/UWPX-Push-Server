#include "XmppClient.hpp"
#include "storage/Serializer.hpp"

namespace xmpp {
XmppClient::XmppClient(const storage::XmppConfiguration& config) : jid(QString::fromStdString(config.bareJid)), password(QString::fromStdString(config.password)) {}

void XmppClient::init() {
    logger()->setLoggingType(QXmppLogger::LoggingType::StdoutLogging);
    connectToServer(jid, password);
}

void XmppClient::messageReceived(const QXmppMessage& message) {
    QString from = message.from();
    QString msg = message.body();

    sendPacket(QXmppMessage("", from, "Your message: " + msg));
}
}  // namespace xmpp