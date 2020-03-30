import sqlite3
from threading import Thread
from typing import Optional
from queue import Queue
from time import sleep

class DbManager(Thread):
    DEFAULT_DB_PATH: str = "pushServer.db"

    con: Optional[sqlite3.Connection]
    dbPath: str
    q: Queue
    __shouldRun: bool

    def __init__(self, dbPath: str = DEFAULT_DB_PATH):
        Thread.__init__(self, name=("Sqlite3Thread"))
        self.dbPath: str = dbPath
        self.con: Optional[sqlite3.Connection] = None
        self.q: Queue = Queue()
        self.__shouldRun: bool = False
        
    def __del__(self):
        self.close()

    def run(self):
        print("DB thread started.")
        self.__shouldRun = True
        self.con = sqlite3.Connection = sqlite3.connect(self.dbPath)
        self.createTablesSave()
        c: sqlite3.Cursor = self.con.cursor()
        while self.__shouldRun:
            if self.q.empty():
                sleep(0.2)
                continue
            item = self.q.get(True, 0)
            if item[1]:
                c.execute(item[0], item[1])
            else:
                c.execute(item[0])
        self.con.close()
        print("DB thread stopped.")

    def requestStop(self):
        self.__shouldRun = False

    def createTablesSave(self):
        self.__createChannelUriTableSave()
        self.__createAccountTableSave()

    def __createChannelUriTableSave(self):
        self.q.put(('''CREATE TABLE IF NOT EXISTS channelUris (deviceId TEXT PRIMARY KEY, channelUri TEXT NOT NULL)''', None))

    def __createAccountTableSave(self):
        self.q.put(('''CREATE TABLE IF NOT EXISTS accounts (deviceId TEXT NOT NULL, bareJid TEXT NOT NULL, domain TEXT NOT NULL, node TEXT NOT NULL, nodeSecret TEXT NOT NULL)''', None))

    def updateChannelUri(self, channelUri: str, deviceId: str):
        self.q.put(('''INSERT OR REPLACE INTO channelUris (?, ?)''', (deviceId, channelUri)))