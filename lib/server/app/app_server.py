from lib.server.raw_server import raw_Server
from lib.server.xmpp.xml_parser import XML_Parser
from xml.parsers.expat import ExpatError


class app_Server(raw_Server):

    def __init__(self, hostname, log):
        super(app_Server, self).__init__(hostname, log, 6000)
        self.connect()
        self.upgradetoTLS(True)

    def __processClient(self, conn):
        try:
            input = conn.recv(1024).decode()
        except ConnectionResetError:
            self.log.printWarning("app_Server Connection reset by peer!")
            return
        try:
            print(input)
            pros = XML_Parser(input)
            d = pros.pushToTupel()
            print(d)
            # C = Clientmanager(self.log, "db/client.db")
            # C.addClient(d)
            conn.sendall(b'<success/>')
        except ExpatError:
            conn.sendall(b'Error empy request')
            self.log.printWarning("<error message='empty request'/>")
        conn.close()
