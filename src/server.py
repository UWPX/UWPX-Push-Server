#!/usr/bin/python3

from enum import Enum
from wns.wnslib import WNSClient
from wns.credentials import CLIENT_SECRET, PACKET_ID
from tcp.tcpServer import TcpServer
from typing import Dict, Any, List, Tuple
from socket import socket
from tcp.messages.AbstractMessage import AbstractMessage
from tcp.messages.SetChannelUriMessage import SetChannelUriMessage
from tcp.messages.ErrorResponseMessage import ErrorResponseMessage
from tcp.messages.SuccessResponseMessage import SuccessResponseMessage
from tcp.messages.SetPushAccountsMessage import SetPushAccountsMessage
from tcp.messages.SuccessSetPushAccountsMessage import SuccessSetPushAccountsMessage
from db.dbManager import DbManager
from datetime import datetime
from PushAccount import PushAccount

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
    db: DbManager

    def __init__(self):
        self.__state = ServerState.NOT_RUNNING
        self.wnsClient = WNSClient(PACKET_ID, CLIENT_SECRET)
        self.tcpServer = TcpServer()
        self.db = DbManager()

    def start(self):
        if self.__state != ServerState.NOT_RUNNING and self.state != ServerState.ERROR:
            print("Unable to start the server - already running. State: {} ".format(self.state))
            return
        print("Starting the server...")
        self.db.start()
        self.tcpServer.registerValidMessageReceivedCallback(self.__onValidMessageReceived)
        self.tcpServer.start()
        
        # self.wnsClient.requestToken()
        # print("Token requested.")
        # channelUrl: str = "https://am3p.notify.windows.com/?token=AwYAAACZOIJlnEQfx5r8GKGyA6SAGFoK4RAKMGxo%2bab9tZg4F7g1cCOr4tpODbBwaIiMuWlFtHVDqX1Zk%2f0gF2VlAAOeVacebP5wKsY4Er%2bq5%2fEpOjkYJiKc4%2fdjkX40F%2fvYNCHAD28MwO7Zv3auzmQ1%2f8UC"
        # self.wnsClient.sendRawNotification(channelUrl, "Some test content!")

        print("Server started.")

    def stop(self):
        print("Stopping the server...")
        self.tcpServer.requestStop()
        self.tcpServer.removeValidMessageReceivedCallback(self.__onValidMessageReceived)
        self.tcpServer.join()
        self.db.requestStop()
        self.db.join()
        print("Server stopped.")
    
    def __updateChannelUri(self, deviceId: str, channelUri: str):
        self.db.updateChannelUri(deviceId, channelUri, datetime.now())
        print("Channel URI set for 'device' {} to: {}".format(deviceId, channelUri))
        pass

    def __updatePushDevices(self, deviceId: str, accounts: List[str], sock:socket):
        accountsResult: List[PushAccount] = list()
        accountsResponse: List[Tuple[str, str, str]] = list()
        for account in accounts:
            pAcc: PushAccount = PushAccount(deviceId)
            pAcc.generate(account)
            accountsResult.append(pAcc)
            accountsResponse.append(account, pAcc.node, pAcc.secret)

        # Update the DB:
        

        # Send the success response:
        self.tcpServer.sendToClient(str(SuccessSetPushAccountsMessage(accountsResponse)), sock)

    # Handle all incoming messages:
    def __onValidMessageReceived(self, msg: AbstractMessage, sock: socket):
        if isinstance(msg, SetChannelUriMessage):
            self.__updateChannelUri(msg.deviceId, msg.channelUri)
            self.tcpServer.respondClientWithSuccessMessage(sock)
        if isinstance(msg, SetPushAccountsMessage):
            self.__updatePushDevices(msg.deviceId, msg.accounts, sock)
        else:
            self.tcpServer.respondClientWithErrorMessage("Unsupported message typ.", sock)
        