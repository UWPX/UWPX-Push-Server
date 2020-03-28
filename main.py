#!/usr/bin/python3

from server import Server


def main():
    server: Server = Server()
    try:
        server.start()
    except KeyboardInterrupt:
        server.stop()
        print("Server stopped by keyboard interrupt.")


if __name__ == '__main__':
    main()
