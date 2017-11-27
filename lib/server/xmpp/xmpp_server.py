from lib.server.raw_server import raw_Server
from lib.log import log
from lib.server.xmpp import xml_processor


class xmpp_server(raw_Server):
    def __init__(self, hostname, log):
        super(xmpp_server, self).__init__(hostname, log, 5269)
