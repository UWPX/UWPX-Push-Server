from lib.log import log
from lib.server.app.app_server import app_Server


def main():
    try:
        import sys
        print(sys.version)
        logger = log("log", True)
        Server = app_Server("", logger)
        Server.loop()
    except KeyboardInterrupt:
        Server.close()
        logger.printWarning("server stopped by keyboard interrupt")




if __name__ == '__main__':
    main()
