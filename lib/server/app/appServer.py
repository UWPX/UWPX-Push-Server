from lib.server.raw_server import raw_Server
from lib.server.xmpp.xml_parser import XML_Parser

class appServer(raw_Server):

    def __init__(self, hostname, log):
        super(appServer, self).__init__(hostname, log, 6000)

    def processClient(self, conn):
        input = conn.recv(1024)
        print(input.decode())
        pros =XML_Parser(input.decode())
        d = pros.pushNotification()
        for x, y in d.items():
            print(x + ": " + y)
        conn.sendall(b'success')
        conn.close()
