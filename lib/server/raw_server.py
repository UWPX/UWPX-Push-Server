import socket
import ssl
from lib.log import log
from threading import Thread


class raw_Server(object):
    def __init__(self, hostname, log, port):
        self.sock = socket.socket(socket.AF_INET)
        self.hostname = hostname
        self.log = log
        self.port = port
        self.active = False
        self.connect()

    def connect(self):
        try:
            self.sock.bind(('', self.port))
        except socket.error.bind as e:
            self.log.printError("server bind on port: "+str(self.port)+" failed.")
            exit(-1)
        self.log.printMessage("server sccesfuly binded port: "+str(self.port))
        self.sock.listen(10)
        self.active = True

    def close(self):
        self.active = False
        self.log.printMessage("closeing appserver, requested by other class")

    def processClient(self, conn):
        raise NotImplementedError("raw_Server isn't ment to be used as standalone")

    def upgradetoTLS(self, server):
        if(server):
            try:
                self.sock = ssl.wrap_socket(self.sock, server_side=True, certfile='/home/kilian/Projekte/GitHub/XMPP-Push/cert/rbg.cert', keyfile='/home/kilian/Projekte/GitHub/XMPP-Push/cert/rgb.key')
            except FileNotFoundError: 
                self.log.printError("cert or keyfile not found terminating!")
                exit(-1)
        else:
            self.context = ssl.create_default_context()
            self.context = ssl.SSLContext(ssl.PROTOCOL_SSLv23)
            self.context.verify_mode = ssl.CERT_REQUIRED
            self.context.check_hostname = True
            self.sock = self.context.wrap_socket(self.sock, server_hostname=self.hostname)
        return self.sock

    def loop(self):
        while self.active:
            try:
                conn, addr = self.sock.accept()
                Thread(target=self.processClient, args=(conn,)).start()

            except ssl.SSLError:
                self.log.printWarning("unecrypted connection, blocked!")
        self.sock.close()
