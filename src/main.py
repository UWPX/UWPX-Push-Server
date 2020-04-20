#!/usr/bin/python3

from server import Server
from time import sleep
from config import loadConfig
from signal import signal, SIGTERM, SIGILL, SIGINT

shouldRun: bool = True

def receiveSignal(signalNumber, frame):
    print("Shutting down...")
    shouldRun = False

def initSignalHandler():
    signal(SIGTERM, receiveSignal)
    signal(SIGILL, receiveSignal)
    signal(SIGINT, receiveSignal)

def main():
    initSignalHandler()

    server: Server = Server(loadConfig())
    try:
        server.start()

        # Wait for a keyboard interupt:
        while shouldRun:
            sleep(0.5)
    except KeyboardInterrupt:
        print("Keyboard interupt received. Shutting down...")
    server.stop()
    print("Good bye.")


if __name__ == '__main__':
    main()
