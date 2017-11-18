from lib.log import log
from lib.server.app import appServer

def main():
    import sys
    print(sys.version)
    logger = log("log", True)
    Server = appServer("w215-5a-v4.eduroam.dynamic.rbg.tum.de", log)
    appServer.loop()




if __name__ == '__main__':
    main()
