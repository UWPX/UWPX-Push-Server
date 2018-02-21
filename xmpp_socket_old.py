#!/usr/bin/python
import ssl
import socket


class xmpp_socket(object):
    def __init__(self, hostname, port, logger):
        self.hostname = hostname
        self.port = port
        self.sock = socket.socket(socket.AF_INET)
        self.context = ssl.create_default_context()
        self.log = logger
        self.connect()

    def connect(self):
        try:
            self.sock.connect((self.hostname, self.port))
        except OSError:
            self.log.printError("xmpp_socket couldn connect!")
            exit(-1)

    def close(self):
        self.sock.close()

    def sendMessage(self, message):
        self.sock.send(message.encode())

    def recvMessage(self):
        temp = self.sock.recv(4096).decode()
        while len(temp) == 0:
            temp = self.sock.recv(4096).decode()

        import xml.dom.minidom
        from pprint import pprint
        try:
            xml = xml.dom.minidom.parseString(temp)
            print(xml.toprettyxml())
        except Exception as e:
            print(e)
            pprint(temp)
        return temp

    def upgradetoTLS(self):
        self.context = ssl.SSLContext(ssl.PROTOCOL_SSLv23)
        self.context.verify_mode = ssl.CERT_REQUIRED
        self.context.check_hostname = True
        self.context.load_verify_locations("/etc/pki/tls/certs/ca-bundle.crt")
        self.sock = self.context.wrap_socket(self.sock, server_hostname=self.hostname)
        return self.sock


if __name__ == '__main__':
    from lib.log import log
    xp = xmpp_socket("push.xmpp.yandex.ru.", 5222, log("test", False))
    try:
        xp.sendMessage('<<?xml version="1.0"?><stream:stream xmlns:stream="http://etherx.jabber.org/streams" version="1.0" xmlns="jabber:client" to="ya.ru" xml:lang="en" xmlns:xml="http://www.w3.org/XML/1998/namespace">')
        xp.recvMessage()
        while True:
            var = str(input("Enter XML: "))
            xp.sendMessage(var)
            print("------------")
            xp.recvMessage()
    except Exception as e:
        print(e)
        xp.close()
        exit(0)
    exit(-1)
