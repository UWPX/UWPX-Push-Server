import socket
from lib.log import log
from threading import Thread
from lib.server.xmpp.xml_parser import XML_Parser

class appServer(object):

    def __init__(self, hostname, log):
        self.sock = socket.socket(socket.AF_INET)
        self.hostname = hostname
        self.log = log
        self.active = False
        self.connect()

    def connect(self):
        try:
            self.sock.bind(('', 6000))
        except socket.error as e:
            self.log.printError("appserver bind failed.")
            exit(-1)
        self.log.printMessage("appserver successfuly added webserver")
        self.sock.listen(10)
        self.active = True

    def close(self):
        self.active = False
        self.log.printMessage("closeing appserver, requested by other class")

    def processClient(self, conn):
        input = conn.recv(1024)
        print(input.decode())
        pros =XML_Parser(input.decode())
        d = pros.pushNotification()
        for x, y in d.items():
            print(x + ": " + y)
        conn.sendall(b'success')
        conn.close()

    def loop(self):
        while self.active:
            conn, addr = self.sock.accept()
            Thread(target=self.processClient, args=(conn,)).start()
        self.sock.close()
