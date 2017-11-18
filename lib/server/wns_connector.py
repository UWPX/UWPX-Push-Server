from lib.server.wnslib import WNSClient
from lib.log import log


class wns_connector(object):
    token = None

    def __init__(self, log):
        self.log = log
        try:
            with open("key.key", "r") as keyFile:
                self.token = keyFile.readline()
        except FileNotFoundError:
            self.log.printError("no key file found (key.key), terminating!")
            exit(-1)

    def sendNotification(wns_id, message):
        pass
