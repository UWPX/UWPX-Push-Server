import sqlite3
from typing import Tuple


class Clientmanager(object):

    connection = None
    cursor = None

    def __init__(self, logger, datebaseName: str) -> None:
        self.log = logger
        try:
            self.connection = sqlite3.connect(datebaseName)
            self.cursor = self.connection.cursor()
        except:
            self.log.printError("Connection to database couldn't be established!")
            exit(-1)

    def closeConncection(self) -> None:
        try:
            self.connection.close()
        except:
            self.log.printError("Connection to database couldn't be closed!")

    def commitChanges(self) -> None:
        try:
            self.connection.commit()
        except:
            self.log.printError("Couldn't commit (save) changes in database!")

    def addClient(self, inputTupel: Tuple[str, str, str, str, str]) -> None:
        try:
            self.cursor.execute("INSERT OR REPLACE INTO Client VALUES (?, ?, ?, ?, ?)", inputTupel)
            # tupel body: (wns_id, server, token, wns_secret, jabber_id)
        except:
            self.log.printError("Couldn't write new entry to database")

    def readClientData(self, wns_id: str)-> Tuple:
        try:
            return self.cursor.execute("SELECT * FROM Client WHERE wns_id= ?", (wns_id, )).fetchone()
        except:
            self.log.printError("Couldn't get data from Database!")
            raise Exception

    def deleteClient(self, wns_id: str) -> None:
        try:
            self.cursor.execute("DELETE FROM Client WHERE wns_id = (?)", (wns_id, ))
            # if not working check without brakket sourrnding of questionmark
        except:
            self.log.printError("Database entry couldn't be deleted!")

    def getWnsSecret(self, wns_id: str) -> str:
        try:
            return self.cursor.execute("SELECT wns_secret FROM Client WHERE wns_id= ?", (wns_id, )).fetchone()[0]
        except:
            self.log.printError("Couldn't get wns_secret from Database!")
            raise Exception


if __name__ == '__main__':
    print("this file is not intended for standalone use")
    """
    manager = Clientmanager(logger = log("test", False), datebaseName="C:\\Users\\Tim\\Documents\\GitKraken\\XMPP-Push\\db\\client.db")
    manager.addClient(wns_id=1234, server="fuck.you.com", token="token", wns_secret = "secret", jabber_id="jabber")
    manager.addClient(wns_id=2345, server="fuck.you.com", token="token1", wns_secret="secret1", jabber_id="jabber1")
    #print(manager.getWnsSecret(wns_id=2345))
    #print(manager.readClientData(wns_id=2345))
    manager.deleteClient(wns_id=1234)
    manager.deleteClient(wns_id=2345)
    manager.commitChanges()
    manager.closeConncection()
    """
