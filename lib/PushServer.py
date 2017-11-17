from lib.log import log
from lib.clientmanager import Clientmanager
from concurrent import futures


class PushServer(object):
    """docstring for PushServer"""
    log = None
    dataHandler = None

    def __init__(self, log):
        self.log = log
        self.printMessage("server started")

    def __getCpuCount():
        from os import cpu_count
        return cpu_count()

    def mainLoop(self):
        with futures.ProcessPoolExecutor(max_workers=self.__getCpucount()) as worker:
            pass
        while True:
            pass
        self.dataHandler.closeConnection()
