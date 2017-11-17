import platform
from time import gmtime, strftime


class log(object):
    """Server logger"""

    def __init__(self, name, isOn):
        self.__doLog = isOn
        if self.__doLog:
            if platform.system() == "Windows":
                self.__fileName = "log\\" + str(name)
            else:
                self.__fileName = "log/" + str(name)
            try:
                test = open(self.__fileName, "a")
                test.close()
            except:
                from os import makedirs, path
                if not path.isdir("log/"):
                    makedirs("log/")

    def writetoFile(self, message):
        newMassage = message + "\n"
        logFile = open(self.__fileName, "a")
        logFile.write(newMassage)
        logFile.close()

    def getTime(self):
        return str(strftime("%Y-%m-%d %H:%M:%S", gmtime()))

    def printWarning(self, warning):
        message = self.getTime() + "\t[WARNING]\t" + warning
        print(message)
        if self.__doLog:
            self.writetoFile(message)

    def printError(self, fehler):
        message = self.getTime() + "\t[ ERROR ]\t" + fehler
        print(message)
        if self.__doLog:
            self.writetoFile(message)

    def printMessage(self, Message):
        message = self.getTime() + "\t[MESSAGE]\t" + Message
        print(message)
        if self.__doLog:
            self.writetoFile(message)
