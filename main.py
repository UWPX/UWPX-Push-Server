from lib.log import log
from lib.server.app.appServer import appServer


def main():
    import sys
    print(sys.version)
    logger = log("log", True)
    Server = appServer("", logger)
    Server.loop()




if __name__ == '__main__':
    main()
