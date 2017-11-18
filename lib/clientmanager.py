import sqlite3
from log import log


class Clientmanager(object):
    """docstring for Clientmanager"""
    log = None

    connection = None
    cursor = None

    def __init__(self, logger):
        self.log = logger

    def connectToDatabase(self, datebaseName):
        try:
            self.connection = sqlite3.connect(datebaseName)
            self.cursor = self.connection.cursor()
        except:
            self.log.printError("Connection to database couldn't be established!")

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

    def addClient(self, hash, ip, server, token):
        try:
            self.cursor.execute("INSERT OR REPLACE INTO Client VALUES (?, ?, ?, ?)",
                (hash, ip, server, token))
        except :
            self.log.printError("Couldn't write new entry to database")

    def readClientData(self):
        pass
        # TODO

    def deleteClient(self, hash):
        try:
            self.cursor.execute("DELETE FROM Client WHERE Hash = (?)", (hash, ))
            # if not working check without brakket sourrnding of questionmark
        except IOError:
            self.log.printError("Database entry couldn't be deleted!")


if __name__ == '__main__':
    manager = Clientmanager(logger = log("test", False))
    manager.connectToDatabase("/home/kilian/Projekte/GitHub/XMPP-Push/db/client.db")
    manager.addClient(hash="superAwsomeHash", ip = "localhost", server = "fuck.you.com")
    manager.addClient(hash="superAwsomeHash1", ip = "localhost", server = "fuck.you.com")
    manager.deleteClient(hash="superAwsomeHash1")
    manager.commitChanges()
    manager.closeConncection()
