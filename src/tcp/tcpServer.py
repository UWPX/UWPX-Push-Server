from enum import Enum
from socket import socket, AF_INET, SOCK_STREAM, gethostname
from typing import Optional, Any, Tuple, Set, Callable
from threading import Thread
from tcp.messages.AbstractMessage import AbstractMessage
from tcp.messages.ErrorResponseMessage import ErrorResponseMessage
from tcp.messages.SuccessResponseMessage import SuccessResponseMessage
from tcp.messages.parser import parse
from ssl import create_default_context, CERT_REQUIRED, Purpose, SSLContext, PROTOCOL_TLSv1_2, SSLSocket


class TcpServerState(Enum):
    NOT_RUNNING = 0
    STARTING = 1
    RUNNING = 2
    STOPPING = 3
    ERROR = 4

class TcpServer(Thread):
    BUFFER_SIZE: int = 1024

    __port: int
    __state: TcpServerState
    __serverCertPath: str
    __serverKeyPath: str

    # Callbacks:
    __validMessageReceivedCallbacks: Set[Callable[[AbstractMessage, socket], None]]

    def __init__(self, port: int, serverCertPath: str, serverKeyPath: str):
        Thread.__init__(self, name=("TCPServerThread"))
        self.__port = port
        self.__state = TcpServerState.NOT_RUNNING
        self.__serverCertPath = serverCertPath
        self.__serverKeyPath = serverKeyPath

        # Callbacks:
        self.__validMessageReceivedCallbacks: Set[Callable[[AbstractMessage, socket], None]] = set()

    def __prepSslCtx(self):
        sslCtx: SSLContext = create_default_context(purpose=Purpose.CLIENT_AUTH)
        # sslCtx.options |= PROTOCOL_TLSv1_2 # TLS1.2 since C# does not yet fully support TLS1.3
        sslCtx.load_cert_chain(certfile=self.__serverCertPath, keyfile=self.__serverKeyPath)
        return sslCtx

    def run(self):
        self.__state = TcpServerState.STARTING
        with socket(AF_INET, SOCK_STREAM) as sock:
            sock = socket(AF_INET, SOCK_STREAM)
            sock.bind((gethostname(), self.__port))
            sock.listen(100)
            sock.settimeout(1)
        
            # Upgrade to TLS:
            sslCtx: SSLContext = self.__prepSslCtx()
            with sslCtx.wrap_socket(sock, True) as ssock:
                self.__state = TcpServerState.RUNNING

                while self.__state == TcpServerState.RUNNING:
                    # Accept:
                    try:
                        conn: Tuple[socket, Any] = ssock.accept()
                    except Exception as e:
                        continue
                    print("New TCP connection from {} accepted.".format(conn[1]))

                    # Read:
                    try:
                        msg: str = self.readFromClient(conn[0], conn[1])
                    except Exception as e:
                        print("Reading from client failed - {}. Disconnecting...".format(e))
                        conn[0].close()    
                        continue

                    # Process:
                    if msg:
                        try:
                            self.processMessage(msg, conn[0])
                        except Exception as e:
                            print("Processing message failed - {}. Disconnecting...".format(e))
                    conn[0].close()
                    print("TCP connection with {} closed.".format(conn[1]))

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