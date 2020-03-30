from enum import Enum
from socket import socket, AF_INET, SOCK_STREAM, gethostname
from typing import Optional, Any, Tuple, Set, Callable
from threading import Thread
from tcp.messages.AbstractMessage import AbstractMessage
from tcp.messages.ErrorResponseMessage import ErrorResponseMessage
from tcp.messages.SuccessResponseMessage import SuccessResponseMessage
from tcp.messages.parser import parse


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

    # Callbacks:
    __validMessageReceivedCallbacks: Set[Callable[[AbstractMessage, socket], None]]

    def __init__(self, port: int = DEFAULT_PORT):
        Thread.__init__(self, name=("TCPServerThread"))
        self.port = port
        self.__state = TcpServerState.NOT_RUNNING
        self.sock = None

        # Callbacks:
        self.__validMessageReceivedCallbacks: Set[Callable[[AbstractMessage, socket], None]] = set()

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
            print("TCP connection with {} closed.".format(conn[1]))

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
                    return msg.rstrip('\x00')
            else:
                return msg

    def sendToClient(self, msg: str, sock: socket):
        data: bytes = msg.encode("utf-8")
        # Append a NULL-Byte to signal the end of the message:
        data += b'\x00'
        sock.send(data)

    def processMessage(self, msg: str, sock: socket):
        result: Optional[AbstractMessage] = parse(msg)
        if result:
            self.__fireValidMessageReceived(result, sock)
        else:
            self.respondClientWithErrorMessage("Malformed message!", sock)

    def respondClientWithErrorMessage(self, msg: str, sock: socket):
        self.sendToClient(str(ErrorResponseMessage(msg)), sock)
    
    def respondClientWithSuccessMessage(self, sock: socket):
        self.sendToClient(str(SuccessResponseMessage()), sock)

    def requestStop(self):
        print("Stopping the TCP server...")
        self.__state = TcpServerState.STOPPING

    def registerValidMessageReceivedCallback(self, callback: Callable[[AbstractMessage, socket], None]):
        """
        Registers a new callback that fires when a new valid message has been received
        """
        self.__validMessageReceivedCallbacks.add(callback)

    def removeValidMessageReceivedCallback(self, callback: Callable[[AbstractMessage, socket], None]):
        """
        Removes a callback that fires when a new valid message has been received
        """
        try:
            self.__validMessageReceivedCallbacks.remove(callback)
        except Exception:
            pass

    def __fireValidMessageReceived(self, msg: AbstractMessage, sock: socket):
        """
        Invokes all registered valid message received callbacks
        """
        for callback in self.__validMessageReceivedCallbacks:
            callback(msg, sock)