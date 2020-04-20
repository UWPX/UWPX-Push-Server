from threading import Thread
from time import sleep
from typing import List, Tuple, Any
from ssl import SSLSocket
from tcp.messages.AbstractMessage import AbstractMessage
from tcp.tcpServer import TcpServer


class SocketThread(Thread):
    msg: AbstractMessage
    conn: Tuple[SSLSocket, Any]
    __tcpServer: TcpServer

    def __init__(self, msg: AbstractMessage, conn: Tuple[SSLSocket, Any], tcpServer: TcpServer):
        Thread.__init__(self, name=("SocketThread"))
        self.msg = msg
        self.conn = conn
        self.__tcpServer = tcpServer
    
    def run(self):
        try:
            self.__tcpServer.processMessageInThread(self.msg, self.conn)
        except Exception as e:
            print("Failed to close the TCP socket for '{}' wih: {}".format(self.conn[1], e))

class SocketHandler(Thread):
    threads: List[SocketThread]
    shouldRun: bool
    sockedAdded: bool
    
    def __init__(self):
        Thread.__init__(self, name=("SocketHandlerThread"))
        self.threads: List[SocketThread] = list()
        self.shouldRun: bool = False
        self.sockedAdded: bool = False

    def run(self):
        print("{} started.".format(self.name))
        self.shouldRun = True
        while self.shouldRun:
            while not self.sockedAdded:
                sleep(1)
            
            if self.shouldRun:
                self.sockedAdded = False
                self.cleanupSockThreads()
        print("{} stopped.".format(self.name))

    def cleanupSockThreads(self):
        print("Cleaning up socket threads...")
        newThreads: List[SocketThread] = list()
        for t in self.threads:
            if not t.isAlive():
                t.join()
            else:
                newThreads.append(t)
        print("Removed {} out of {} socket threads.".format(len(self.threads) - len(newThreads), len(self.threads)))
        self.threads = newThreads

    def requestStop(self):
        print("Stopping the {}...".format(self.name))
        self.shouldRun = False

    def addSocket(self, msg: AbstractMessage, conn: Tuple[SSLSocket, Any], tcpServer: TcpServer):
        sockThread: SocketThread = SocketThread(msg, conn, tcpServer)
        sockThread.start()
        self.threads.append(sockThread)
        self.sockedAdded = True