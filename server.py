#!/usr/bin/python3

from enum import Enum
from wns import WNSClient
from typing import Dict, Any
from credentials import CLIENT_SECRET, PACKET_ID

class ServerState(Enum):
    NOT_RUNNING = 0
    STARTING = 1
    RUNNING = 2
    STOPPING = 3
    ERROR = 4

class Server:
    state: ServerState
    wnsClient: WNSClient

    def __init__(self):
        self.state = ServerState.NOT_RUNNING
        self.wnsClient = WNSClient(PACKET_ID, CLIENT_SECRET)
        self.wnsClient.requestToken()
        print("Token requested.")
        channelUrl: str = "https://am3p.notify.windows.com/?token=AwYAAACZOIJlnEQfx5r8GKGyA6SAGFoK4RAKMGxo%2bab9tZg4F7g1cCOr4tpODbBwaIiMuWlFtHVDqX1Zk%2f0gF2VlAAOeVacebP5wKsY4Er%2bq5%2fEpOjkYJiKc4%2fdjkX40F%2fvYNCHAD28MwO7Zv3auzmQ1%2f8UC"
        self.wnsClient.sendRawNotification(channelUrl, "Some test content!")

    def start(self):
        if self.state != ServerState.NOT_RUNNING and self.state != ServerState.ERROR:
            print("Unable to start the server - already running. State: {} ".format(self.state))
            return
        print("Starting the server...")



        print("Server started.")

    def stop(self):
        print("Stopping the server...")
        print("Server stopped.")
        