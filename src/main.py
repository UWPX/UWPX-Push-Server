#!/usr/bin/python3

from server import Server
from time import sleep
from config import loadConfig
from signal import signal, SIGTERM, SIGILL, SIGINT
from fileinput import input
from inputHandler import InputHandler

shouldRun: bool = True


def receiveSignal(signalNumber, frame):
    global shouldRun
    print("Shutting down...")
    shouldRun = False


def initSignalHandler():
    signal(SIGTERM, receiveSignal)
    signal(SIGILL, receiveSignal)
    signal(SIGINT, receiveSignal)


def commandHandler(server: Server):
    for line in input():
        line = line.strip()
        if line == "q" or line == "exit":
            shouldRun = False
            return


def main():
    initSignalHandler()

    server: Server = Server(loadConfig())
    inputHandler: InputHandler = InputHandler(server)
    try:
        server.start()
        inputHandler.start()

        # Wait for a keyboard interupt:
        while shouldRun and server.isRunning():
            sleep(0.5)
    except KeyboardInterrupt:
        print("Keyboard interupt received. Shutting down...")
    if server.isRunning():
        server.stop()
    inputHandler.requestStop()
    inputHandler.join()
    print("Good bye.")


if __name__ == '__main__':
    main()
