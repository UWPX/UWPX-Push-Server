from lib.log import log
from lib.server.wns_connector import wns_connector

def main():
    import sys
    print(sys.version)
    logger = log("log", True)
    wns = wns_connector(logger)




if __name__ == '__main__':
    main()
