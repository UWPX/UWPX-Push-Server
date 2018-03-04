from lib.server.raw_server import raw_Server
from lib.server.xmpp.xml.xml_processor import xml_processor
import socket
import ssl


class xmpp_server(raw_Server):
    def __init__(self, hostname, logger, clientmanager):
        super(xmpp_server, self).__init__(hostname, logger, 5269)
        self.__client = clientmanager
        # self.__wnsCon = wns_connector(logger)

    def close(self):
        super.close()

    def __processClient(self, conn):
            pass

    def __sendPush(self, message, wns_id):
        self.__wnsCon.sendNotification(wns_id, message, self.clientmanager.getWnsSecret(wns_id))


class xmpp_s2s():
    def __init__(self, ownDomain, logger, host):
        self.__ownDomain = ownDomain
        self.__log = logger
        self.__sock = socket.socket(socket.AF_INET)
        self.__hostname = host
        self.__handshake = '<stream:stream xmlns:stream="http://etherx.jabber.org/streams" version="1.0" xmlns="jabber:server" to="'+host+'" from="'+self.__ownDomain+'" xml:lang="en">'
        print(xml_processor.beautifyXMLString(self.__handshake))
        self.connect(host, 5269)

    def connect(self, host, port):
        try:
            self.__sock.connect((host, port))
        except OSError:
            self.logger.printWarning("couldn't connect to: "+host+":"+port)
            raise s2sException
        self.sendMessage(self.__handshake)
        print(xml_processor.beautifyXMLString(self.recvMessage()))

    def upgradetoTLS(self):
        self.sendMessage('<starttls xmlns="urn:ietf:params:xml:ns:xmpp-tls"/>')
        if self.recvMessage() == "<proceed xmlns='urn:ietf:params:xml:ns:xmpp-tls'/>":
            self.__context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
            self.__context.verify_mode = ssl.CERT_REQUIRED
            self.__context.check_hostname = True
            self.__context.load_verify_locations("/etc/pki/tls/certs/ca-bundle.crt")
            try:
                self.__context.load_cert_chain(certfile='cert/tls.cert', keyfile='cert/tls.key')
            except (IOError, ssl.SSLError) as e:
                self.log.printError("cert or keyfile faulty: " + e)
                raise s2sException(e)
            self.__context.check_hostname = True
            try:
                self.__sock = self.__context.wrap_socket(self.__sock, server_hostname=self.__hostname)
            except ssl.SSLError:
                self.__log.printWarning("error with certificat from: "+self.__hostname)
                raise s2sException("bad certificat")
            self.sendMessage(self.__handshake)
            print(xml_processor.beautifyXMLString(self.recvMessage()))
        else:
            self.__log.printWarning("Server seems to have problems with TLS")

    def close(self):
        self.sendMessage("</stream:stream>")
        print(self.recvMessage())
        self.__sock.close()

    def sendMessage(self, message):
        self.__sock.send(message.encode())

    def recvMessage(self):
        var = self.__sock.recv(4096).decode()
        if var == "</stream:stream>":
            self.__sock.close()
            self.__log.printWarning("session closed by server: "+self.__hostname)
            raise s2sException("session closed")
        else:
            return self.__sock.recv(4096).decode()


class s2sException(Exception):
    pass


if __name__ == '__main__':
    print("this file is not meant to run as standalone")
    exit(-1)
