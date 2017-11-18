import socket
from lib.log import log
from threading import *


class appServer(object):
    def __init__(self, hostname, logg):
        self.sock = socket.socket(socket.AF_INET)
        self.hostname = hostname
        self.log = logg
        self.active = False
        self.connect()

    def connect(self):
        try:
            self.sock.bind(address=(self.hostname, 6000))
        except socket.error as e:
            self.log.printError("appserver bind failed. Error Code: "+str(e[0]) + " message: " + str(e[1]))
        self.log.printMessage("appserver socket binded")
        self.sock.listen(10)
        self.active = True

    def close(self):
        self.active = False
        self.log.printMessage("closeing appserver, requested by other class")

    def processClient(self, conn):
        input = conn.recv(1024)
        print(input.decode())
        conn.sendall(b'success')
        conn.close

    def loop(self):
        while self.active:
            conn, addr = self.sock.accept()
            start_new_thread(self.processClient, (conn,))
        self.sock.close()
