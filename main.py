from lib.log import log
from lib.server.app.app_server import app_Server


def main():
    import sys
    print(sys.version)
    logger = log("log", True)
    Server = app_Server("", logger)
    Server.loop()




if __name__ == '__main__':
    main()
