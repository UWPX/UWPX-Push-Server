import socket
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
        except socket.error as e:
            self.log.printError("server bind on Port: "+str(self.port)+" failed.")
            exit(-1)
        self.log.printMessage("server successfuly added webserver")
        self.sock.listen(10)
        self.active = True

    def close(self):
        self.active = False
        self.log.printMessage("closeing appserver, requested by other class")

    def processClient(self, conn):
        raise NotImplementedError("raw_Server isn't ment to be used as standalone")

    def loop(self):
        while self.active:
            conn, addr = self.sock.accept()
            Thread(target=self.processClient, args=(conn,)).start()
        self.sock.close()
