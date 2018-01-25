#from wnslib import WNSClient
from lib.log import log
from lib.clientmanager import Clientmanager


class wns_connector(object):
    token = None

    def __init__(self, logger):
        self.log = logger
        try:
            with open("key.key", "r") as keyFile:
                self.token = keyFile.readline()
        except FileNotFoundError:
            self.log.printError("no wns key file found (key.key), terminating!")
            exit(-1)

    """def sendNotification(self, wns_id, message, wnssecret):
        push = {"type": 'toast', "text": [message]}
        wnssession = WNSClient(wnsclientid=wns_id, wnsclientsecret=wnssecret)
        wnssession.process(token=self.token, message=push)
"""