from lib.server.raw_server import raw_Server
from lib.server.wns.wns_connector import wns_connector
from lib.server.xmpp import xml_processor
import socket
import ssl


class xmpp_server(raw_Server):
    def __init__(self, hostname, logger, clientmanager):
        super(xmpp_server, self).__init__(hostname, logger, 5269)
        self.__client = clientmanager
        # self.__wnsCon = wns_connector(logger)

    def close(self):
        print(2)
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
        self.connect(host, 5269)

    def connect(self, host, port):
        try:
            self.__sock.connect((host, port))
        except OSError:
            self.logger.printWarning("couldn't connect to: "+host+":"+port)
            raise s2sException
        self.sendMessage(self.__handshake)
        print(self.recvMessage())

    def upgradetoTLS(self):
        self.sendMessage('<starttls xmlns="urn:ietf:params:xml:ns:xmpp-tls"/>')
        if self.recvMessage() == "<proceed xmlns='urn:ietf:params:xml:ns:xmpp-tls'/>":
            self.__context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
            self.__context.verify_mode = ssl.CERT_REQUIRED
            self.__context.check_hostname = True
            self.__context.load_verify_locations("/etc/pki/tls/certs/ca-bundle.crt")
            self.__context.load_cert_chain(certfile='cert/tls.cert', keyfile='cert/tls.key')
            self.__context.check_hostname = True
            try:
                self.__sock = self.__context.wrap_socket(self.__sock, server_hostname=self.__hostname)
            except ssl.SSLError:
                self.__log.printWarning("error with certificat from: "+self.__hostname)
                raise s2sException("bad certificat")
            self.sendMessage(self.__handshake)
            print(self.recvMessage())
        else:
            self.log.printWarning("Server seems to have problems with TLS")

    def close(self):
        self.sendMessage("</stream:stream>")
        print(self.recvMessage())
        self.__sock.close()

    def sendMessage(self, message):
        self.__sock.send(message.encode())

    def recvMessage(self):
        return self.__sock.recv(4096).decode()


class s2sException(Exception):
    pass


if __name__ == '__main__':
    print("this file is not meant to run as standalone")
    exit(-1)
