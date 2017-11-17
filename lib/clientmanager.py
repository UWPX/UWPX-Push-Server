import sqlite3
from lib.log import log


class Clientmanager(object):
    """docstring for Clientmanager"""
    log=None

    def __init__(self, logger):
        self.log=logger

    def addClient(self, hash, ip, server):
        pass
        

if __name__ == '__main__':
    pass
