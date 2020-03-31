import sqlite3
from threading import Thread
from typing import Optional, List
from queue import Queue
from time import sleep
from datetime import datetime
from PushAccount import PushAccount

class DbManager(Thread):
    DEFAULT_DB_PATH: str = "pushServer.db"

    __con: Optional[sqlite3.Connection]
    __dbPath: str
    __q: Queue
    __shouldRun: bool

    def __init__(self, __dbPath: str = DEFAULT_DB_PATH):
        Thread.__init__(self, name=("Sqlite3Thread"))
        self.__dbPath: str = __dbPath
        self.__con: Optional[sqlite3.Connection] = None
        self.__q: Queue = Queue()
        self.__shouldRun: bool = False

    def __close(self):
        if self.__con:
            self.__con.close()
            self.__con = None

    def run(self):
        print("DB thread started.")
        self.__shouldRun = True
        self.__con = sqlite3.Connection = sqlite3.connect(self.__dbPath)
        self.__createTablesSave()
        c: sqlite3.Cursor = self.__con.cursor()
        while self.__shouldRun:
            if self.__q.empty():
                sleep(0.2)
                continue
            item = self.__q.get(True, 0)
            try:
                if item[1]:
                    c.execute(item[0], item[1])
                else:
                    c.execute(item[0])
            except Exception as e:
                print("Failed to execute DB statement. Exception: {}".format(e))
        self.__close()
        print("DB thread stopped.")

    def requestStop(self):
        self.__shouldRun = False

    def __createTablesSave(self):
        self.__createChannelUriTableSave()
        self.__createAccountTableSave()

    def __createChannelUriTableSave(self):
        self.__q.put(('''CREATE TABLE IF NOT EXISTS channelUris (deviceId TEXT PRIMARY KEY, channelUri TEXT NOT NULL, timeStamp TIMESTAMP);''', None))

    def __createAccountTableSave(self):
        self.__q.put(('''CREATE TABLE IF NOT EXISTS accounts (deviceId TEXT NOT NULL, bareJidHash TEXT NOT NULL, domainPart TEXT NOT NULL, node TEXT NOT NULL, secret TEXT NOT NULL);''', None))

    def updateChannelUri(self, deviceId: str, channelUri: str, timeStamp: datetime):
        self.__q.put(('''INSERT OR REPLACE INTO channelUris (deviceId, channelUri, timeStamp) VALUES (?, ?, ?);''', (deviceId, channelUri, timeStamp)))
    
    def updatePushAccounts(self, deviceId: str, accounts: List[PushAccount]):
        # Remove all old entries:
        self.__q.put('''DELETE FROM accounts WHERE deviceId = ?''', deviceId)
        # Insert all new entries:
        for account in accounts:
            self.__q.put('''INSERT OR REPLACE INTO accounts (deviceId, bareJidHash, domainPart, node, secret) VALUES (?, ?, ?, ?, ?)''', account.deviceId, account.bareJidHash, account.domainPart, account.node, account.secret)