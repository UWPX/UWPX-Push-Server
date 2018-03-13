import platform
from time import gmtime, strftime


class log(object):
    def __init__(self, name: str, isOn: bool) -> None:
        self.__doLog = isOn
        if self.__doLog:
            if platform.system() == "Windows":
                self.__fileName = "log\\" + str(name)+".log"
            else:
                self.__fileName = "log/" + str(name)+".log"
            try:
                test = open(self.__fileName, "a")
                test.close()
            except:
                from os import makedirs, path
                if not path.isdir("log/"):
                    makedirs("log/")

    def __writetoFile(self, message: str) -> None:
        newMassage = message + "\n"
        logFile = open(self.__fileName, "a")
        logFile.write(newMassage)
        logFile.close()

    def __getTime(self) -> str:
        return str(strftime("%Y-%m-%d %H:%M:%S", gmtime()))

    def printWarning(self, warning: str) -> None:
        message = self.__getTime() + "\t[WARNING]\t" + warning
        print(message)
        if self.__doLog:
            self.__writetoFile(message)

    def printError(self, error: str) -> None:
        message = self.__getTime() + "\t[ ERROR ]\t" + error
        print(message)
        if self.__doLog:
            self.__writetoFile(message)

    def printMessagke(self, theMessage: str) -> None:
        message = self.__getTime() + "\t[MESSAGE]\t" + theMessage
        print(message)
        if self.__doLog:
            self.__writetoFile(message)
