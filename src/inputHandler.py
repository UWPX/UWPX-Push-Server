from server import Server
from threading import Thread
from typing import Optional, List, Any, Tuple
from sys import stdin
from select import select
from com.task import Task


class InputHandler(Thread):

    __shouldRun: bool
    __server: Server

    def __init__(self, server: Server):
        Thread.__init__(self, name=("InputHandler"))
        self.__shouldRun: bool = False
        self.__server = server

    def readInput(self):
        # Cancel waiting for input after 0.5 seconds:
        result: Tuple[List[Any], List[Any], List[Any]
                      ] = select([stdin], [], [], 0.5)
        if result[0]:
            return stdin.readline().strip()
        return None

    def handleInput(self, line: str):
        if line == "help":
            self.printHelp()
        elif line == "q" or line == "exit":
            self.__server.stop()
        elif line.startswith("test push"):
            self.testPush(line)
        elif line.startswith("test xmpp publish"):
            self.testXmppPublishNode(line)
        else:
            print("Unknown command received.\nTry typing 'help' for a list of commands.")

    def testXmppPublishNode(self, line: str):
        node: str = line.replace("test xmpp publish", "").strip()
        task: Task = Task(self.__server.xmppClient.subscribeToNodeAsync, node)
        task.waitForCompletion()

    def testPush(self, line: str):
        deviceId: str = line.replace("test push", "").strip()
        self.__server.sendTestPush(deviceId)

    def printHelp(self):
        print("-------------------HELP-------------------")
        print("'help' - This help.")
        print("'test push <deviceId>' - Sends a test push to the given device ID.")
        print("'test xmpp publish <nodeName>' - Publishes and subscribes to the given PubSub node.")
        print("'q' or 'exit' - Stop the server.")
        print("------------------------------------------")

    def run(self):
        self.__shouldRun = True
        print("InputHandler thread started.")
        while self.__shouldRun:
            line: Optional[str] = self.readInput()
            if line:
                self.handleInput(line)
        print("InputHandler thread stopped.")

    def requestStop(self):
        self.__shouldRun = False
        print("Stopping the InputHandler thread...")
