from lib.log import log
from lib.server.wns.wns_connector import wns_connector
from concurrent import futures


def main():
    import sys
    print(sys.version)
    logger = log("log", True)
    wns = wns_connector(logger)

    with futures.ProcessPoolExecutor(4) as worker:
        wns = wns_connector(logger)
        while True:
            pass




if __name__ == '__main__':
    main()
