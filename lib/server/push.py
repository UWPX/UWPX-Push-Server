from lib.server.wnslib import WNSClient
from lib.log import log


class pushConnenction(object):
    log = None
    token = None

    def __init__(self, log):
        self.log = log
        try:
           with open("key.key", "r") as keyFile:
                self.token = keyFile.readline()
                print(self.token)
        except:
            self.log.printError("no key file found (key.key), terminating")
            from os import close
            close(1)
