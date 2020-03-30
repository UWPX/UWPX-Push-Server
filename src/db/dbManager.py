import sqlite3
from typing import Optional

class DbManager:
    DEFAULT_DB_PATH: str = "pushServer.db"

    con: Optional[sqlite3.Connection]
    dbPath: str

    def __init__(self, dbPath: str = DEFAULT_DB_PATH):
        self.dbPath: str = dbPath
        self.con: Optional[sqlite3.Connection] = None
        
    def __del__(self):
        self.close()

    def start(self):
        self.con = sqlite3.Connection = sqlite3.connect(self.dbPath)

    def close(self):
        self.con.close()

    def createTablesSave(self):
        self.__createChannelUriTableSave()
        self.__createAccountTableSave()

    def __createChannelUriTableSave(self):
        c: sqlite3.Cursor = self.con.cursor()
        c.execute('''CREATE TABLE IF NOT EXISTS channelUris (deviceId TEXT PRIMARY KEY, channelUri TEXT NOT NULL)''')

    def __createAccountTableSave(self):
        c: sqlite3.Cursor = self.con.cursor()
        c.execute('''CREATE TABLE IF NOT EXISTS accounts (deviceId TEXT NOT NULL, bareJid TEXT NOT NULL, domain TEXT NOT NULL, node TEXT NOT NULL, nodeSecret TEXT NOT NULL)''')

    def updateChannelUri(self, channelUri: str, deviceId: str):
        c: sqlite3.Cursor = self.con.cursor()
        c.execute('''INSERT OR REPLACE INTO channelUris (?, ?)''', deviceId, channelUri)