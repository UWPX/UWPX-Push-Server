#!/usr/bin/python3

from enum import Enum
from socket import socket, AF_INET, SOCK_STREAM
from typing import Optional


class TcpServerState(Enum):
    NOT_RUNNING = 0
    STARTING = 1
    RUNNING = 2
    STOPPING = 3
    ERROR = 4

class TcpServer:
    port: int
    sock: Optional[socket]

    __state: TcpServerState

    def __init__(self, port: int):
        self.port = port
        self.__state = TcpServerState.NOT_RUNNING    
        self.sock = None    

    def start(self):
        self.__state = TcpServerState.STARTING
        self.sock = socket(AF_INET, SOCK_STREAM)
        self.sock.bind(("127.0.0.1", self.port))
        self.__state = TcpServerState.RUNNING

    def stop(self):
        self.__state = TcpServerState.STOPPING
        self.__state = TcpServerState.NOT_RUNNING