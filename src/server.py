#!/usr/bin/python3

from enum import Enum
from wns.wnslib import WNSClient
from wns.credentials import CLIENT_SECRET, PACKET_ID
from tcp.tcpServer import TcpServer
from typing import Dict, Any

class ServerState(Enum):
    NOT_RUNNING = 0
    STARTING = 1
    RUNNING = 2
    STOPPING = 3
    ERROR = 4

class Server:
    __state: ServerState
    wnsClient: WNSClient
    tcpServer: TcpServer

    def __init__(self):
        self.__state = ServerState.NOT_RUNNING
        self.wnsClient = WNSClient(PACKET_ID, CLIENT_SECRET)
        self.tcpServer = TcpServer()

    def start(self):
        if self.__state != ServerState.NOT_RUNNING and self.state != ServerState.ERROR:
            print("Unable to start the server - already running. State: {} ".format(self.state))
            return
        print("Starting the server...")
        self.tcpServer.start()
        
        self.wnsClient.requestToken()
        print("Token requested.")
        channelUrl: str = "https://am3p.notify.windows.com/?token=AwYAAACZOIJlnEQfx5r8GKGyA6SAGFoK4RAKMGxo%2bab9tZg4F7g1cCOr4tpODbBwaIiMuWlFtHVDqX1Zk%2f0gF2VlAAOeVacebP5wKsY4Er%2bq5%2fEpOjkYJiKc4%2fdjkX40F%2fvYNCHAD28MwO7Zv3auzmQ1%2f8UC"
        self.wnsClient.sendRawNotification(channelUrl, "Some test content!")

        print("Server started.")

    def stop(self):
        print("Stopping the server...")
        self.tcpServer.requestStop()
        self.tcpServer.join()
        print("Server stopped.")
        