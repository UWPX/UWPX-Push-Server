import ssl
import socket


class xmpp_socket(object):
    def __init__(self, hostname, port, logger):
        self.hostname = hostname
        self.port = port
        self.sock = socket.socket(socket.AF_INET)
        #self.context = ssl.create_default_context()
        self.log = logger
        self.connect()

    def connect(self):
        try:
            self.sock.connect((self.hostname, self.port))
        except OSError:
            self.log.printError("xmpp_socket couldn connect!")
            exit(-1)

    def close(self):
        print("")
        self.sendMessage("</stream:stream>")
        print(self.recvMessage())
        self.sock.close()

    def sendMessage(self, message):
        self.sock.send(message.encode())

    def recvMessage(self):
        temp = self.sock.recv(4096).decode()
        return temp

    def upgradetoTLS(self):
        self.context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_1)
        self.context.verify_mode = ssl.CERT_REQUIRED
        self.context.check_hostname = True
        self.context.load_cert_chain(certfile='cert/tls.cert', keyfile='cert/tls.key')
        self.context.load_verify_locations("/etc/pki/tls/certs/ca-bundle.crt")
        self.sock = self.context.wrap_socket(self.sock, server_hostname=self.hostname)
        return self.sock


if __name__ == '__main__':
    pass