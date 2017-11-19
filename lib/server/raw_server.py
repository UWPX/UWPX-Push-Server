import ssl
import socket

class raw_Server(object):
    """docstring for raw_Server"""
    def __init__(self, hostname, log, port):
        super(raw_Server, self).__init__()
        self.sock = socket.socket(socket.AF_INET)
        self.hostname = hostname
        self.log = log
        self.port = port
        self.active = False
        self.connect()
        
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
            self.log.printError("appserver bind failed. Error Code: "+str(e[0]) + " message: " + str(e[1]))
        self.log.printMessage("appserver successfuly added webserver")
        self.sock.listen(10)
        self.active = True
