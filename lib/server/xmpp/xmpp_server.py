from lib.server.raw_server import raw_Server
from lib.clientmanager import Clientmanager
from lib.server.wns.wns_connector import wns_connector
from lib.log import log
from lib.server.xmpp import xml_processor


class xmpp_server(raw_Server):
    def __init__(self, hostname, logger, clientmanager):
        super(xmpp_server, self).__init__(hostname, logger, 5269)
        self.__client = clientmanager
        self.__wnsCon = wns_connector(logger)


def __processClient(self, conn):
        pass


def __sendPush(self, message, wns_id):
    self.__wnsCon.sendNotification(wns_id, message, self.clientmanager.getWnsSecret(wns_id))


if __name__ == '__main__':
    print("this file is not meant to run as standalone")
    exit(-1)
