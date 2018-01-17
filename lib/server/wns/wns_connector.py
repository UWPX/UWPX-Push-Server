from wnslib import WNSClient
from lib.log import log
from lib.clientmanager import Clientmanager


class wns_connector(object):
    token = None

    def __init__(self, logger, clients):
        self.log = logger
        self.client = clients
        try:
            with open("key.key", "r") as keyFile:
                self.token = keyFile.readline()
        except FileNotFoundError:
            self.log.printError("no key file found (key.key), terminating!")
            exit(-1)

    def sendNotification(self, wns_id, message):
        push = {"type": 'toast', "text": [message]}
        wnssession = WNSClient(wnsclientid=wns_id, wnsclientsecret=self.client.getWnsSecret(wns_id))
        wnssession.process(token=self.token, message=push)
