from enum import Enum
from socket import socket, AF_INET, SOCK_STREAM, gethostname
from typing import Optional, Any, Tuple
from threading import Thread
import json


class TcpServerState(Enum):
    NOT_RUNNING = 0
    STARTING = 1
    RUNNING = 2
    STOPPING = 3
    ERROR = 4

class TcpServer(Thread):
    DEFAULT_PORT: int = 1997
    BUFFER_SIZE: int = 1024

    port: int
    sock: Optional[socket]

    __state: TcpServerState

    def __init__(self, port: int = DEFAULT_PORT):
        Thread.__init__(self, name=("TCPServerThread"))
        self.port = port
        self.__state = TcpServerState.NOT_RUNNING    
        self.sock = None    

    def run(self):
        self.__state = TcpServerState.STARTING
        self.sock = socket(AF_INET, SOCK_STREAM)
        self.sock.bind((gethostname(), self.port))
        self.sock.listen(100)
        self.sock.settimeout(1)
        self.__state = TcpServerState.RUNNING

        while self.__state == TcpServerState.RUNNING:
            try:
                conn: Tuple[socket, Any] = self.sock.accept()
            except Exception as e:
                continue
            print("New TCP connection from {} accepted.".format(conn[1]))
            msg: str = self.readFromClient(conn[0], conn[1])
            if msg:
                self.processMessage(msg, conn[0])
            conn[0].close()
        
        self.sock.close()
        self.__state = TcpServerState.NOT_RUNNING
        print("Stopped the TCP server.")

    def readFromClient(self, sock: socket, addr: Any):
        msg: str = ""
        while True:
            data: bytes = sock.recv(self.BUFFER_SIZE)
            if data:
                msg += data.decode("utf-8")
                if data.endswith(b'\x00'):
                    return msg
            else:
                return msg

    def processMessage(self, msg: str, sock:socket):
        jsonObj: Any = None
        try:
            jsonObj = json.loads(msg)
        except ValueError as e:
            print("Failed to parse received message '{}' as JSON: {}".format(msg, e))
            return

        print(jsonObj)
        # version: str = jsonObj["version"]


    def requestStop(self):
        print("Stopping the TCP server...")
        self.__state = TcpServerState.STOPPING