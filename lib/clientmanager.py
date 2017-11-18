import sqlite3
from log import log


class Clientmanager(object):
    """docstring for Clientmanager"""
    log = None

    connection = None
    cursor = None

    def __init__(self, logger, datebaseName):
        self.log = logger
        try:
            self.connection = sqlite3.connect(datebaseName)
            self.cursor = self.connection.cursor()
        except:
            self.log.printError("Connection to database couldn't be established!")
            exit(-1)

    def closeConncection(self):
        try:
            self.connection.close()
        except:
            self.log.printError("Connection to database couldn't be closed!")

    def commitChanges(self):
        try:
            self.connection.commit()
        except:
            self.log.printError("Couldn't commit (save) changes in database!")

    def addClient(self, wns_id, server, token, wns_secret):
        try:
            self.cursor.execute("INSERT OR REPLACE INTO Client VALUES (?, ?, ?, ?)",
                (wns_id, server, token, wns_secret))
        except :
            self.log.printError("Couldn't write new entry to database")

    def readClientData(self, criteria):
        pass
        self.cursor.execute("SELECT * FROM Client WHERE Xyz=?", criteria)
        # TODO

    def deleteClient(self, wns_id):
        try:
            self.cursor.execute("DELETE FROM Client WHERE wns_id = (?)", (wns_id, ))
            # if not working check without brakket sourrnding of questionmark
        except IOError:
            self.log.printError("Database entry couldn't be deleted!")


if __name__ == '__main__':
    pass

    """
    manager = Clientmanager(logger = log("test", False), datebaseName="C:\\Users\\Tim\\Documents\\GitKraken\\XMPP-Push\\db\\client.db")
    manager.addClient(wns_id=1234, server="fuck.you.com", token="token", wns_secret = "secret")
    manager.addClient(wns_id=2345, server="fuck.you.com", token="token1", wns_secret="secret1")
    manager.deleteClient(wns_id=2345)
    manager.commitChanges()
    manager.closeConncection()
    """