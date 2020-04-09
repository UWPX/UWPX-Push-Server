from threading import Thread
from time import sleep
from typing import List, Tuple, Any
from ssl import SSLSocket
from tcp.messages.AbstractMessage import AbstractMessage

class SocketThread(Thread):
    msg: AbstractMessage
    conn: Tuple[SSLSocket, Any]

    def __init__(self, msg: AbstractMessage, conn: Tuple[SSLSocket, Any]):
        Thread.__init__(self, name=("SocketThread"))
        self.msg = msg
        self.conn = conn
    
    def run(self):
        sleep(5)
        try:
            self.conn[1].close()
        except Exception as e:
            print("Failed to close the TCP socket for '{}' wih: {}".format(self.conn[1], e))

class SocketHandler(Thread):
    threads: List[SocketThread]
    
    def __init__(self):
        Thread.__init__(self, name=("SocketHandlerThread"))
        self.threads: List[SocketThread] = list()

    def run(self):
        pass

    def addSocket(self, )