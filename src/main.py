#!/usr/bin/python3

from server import Server
from time import sleep


def main():
    server: Server = Server()
    try:
        server.start()

        # Wait for a keyboard interupt:
        while True:
            sleep(1)
    except KeyboardInterrupt:
        server.stop()
        print("Server stopped by keyboard interrupt.")


if __name__ == '__main__':
    main()
