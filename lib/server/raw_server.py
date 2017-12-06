import socket
import ssl
from lib.log import log
from threading import Thread


class raw_Server(object):
    def __init__(self, hostname, thelog, port):
        self.__sock = socket.socket(socket.AF_INET)
        self.__hostname = hostname
        self.log = thelog
        self.__port = port
        self.__active = False
        self.connect()

    def connect(self):
        try:
            self.__sock.bind(('', self.__port))
        except socket.error as e:
            self.log.printError("server bind on port: "+str(self.__port)+" failed.")
            exit(-1)
        self.log.printMessage("server sccesfuly binded port: "+str(self.__port))
        self.__sock.listen(10)
        self.__active = True

    def close(self):
        self.__active = False
        self.log.printMessage("closeing appserver, requested by other class")

    def __processClient(self, conn):
        raise NotImplementedError("raw_Server isn't ment to be used as standalone")

    def upgradetoTLS(self, server):
        if(server):
            try:
                self.__sock = ssl.wrap_socket(self.__sock, server_side=True, certfile='cert/rbg.cert', keyfile='cert/rgb.key', ssl_version=ssl.PROTOCOL_TLSv1_2)
            except FileNotFoundError:
                self.log.printError("cert or keyfile not found terminating!")
                exit(-1)
        else:
            self.__context = ssl.create_default_context()
            self.__context = ssl.SSLContext(PROTOCOL_TLSv1_2)
            self.__context.verify_mode = ssl.CERT_REQUIRED
            self.__context.check_hostname = True
            self.__sock = self.__context.wrap_socket(self.__sock, server_hostname=self.__hostname)
        return self.__sock

    def loop(self):
        while self.__active:
            try:
                conn, addr = self.__sock.accept()
                Thread(target=self.__processClient, args=(conn,)).start()
            except ssl.SSLError:
                self.log.printWarning("unecrypted connection, blocked!")
        self.__sock.close()
