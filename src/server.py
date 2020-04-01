#!/usr/bin/python3

from enum import Enum
from wns.wnslib import WNSClient
from wns.credentials import CLIENT_SECRET, PACKET_ID
from tcp.tcpServer import TcpServer
from typing import Dict, Any, List, Tuple, Optional
from socket import socket
from tcp.messages.AbstractMessage import AbstractMessage
from tcp.messages.SetChannelUriMessage import SetChannelUriMessage
from tcp.messages.ErrorResponseMessage import ErrorResponseMessage
from tcp.messages.SuccessResponseMessage import SuccessResponseMessage
from tcp.messages.SetPushAccountsMessage import SetPushAccountsMessage
from tcp.messages.SuccessSetPushAccountsMessage import SuccessSetPushAccountsMessage
from tcp.messages.RequestTestPushMessage import RequestTestPushMessage
from db.dbManager import ChannelUri, PushAccount, WNSToken, initDb
from peewee import DoesNotExist
from datetime import datetime, timezone, timedelta

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
        # DB
        initDb()

        # TCP:
        self.tcpServer.registerValidMessageReceivedCallback(self.__onValidMessageReceived)
        self.tcpServer.start()

        # WNS:
        if self.__shouldRequestWnsToken():
            print("Requesting a new WNS token...")
            if self.wnsClient.requestToken():
                WNSToken.truncate_table()
                WNSToken.create(token=self.wnsClient.token.token, expires=self.wnsClient.token.getExpireDateUtc())
                print("WNS token requested successfully.")
            else:
                print("WNS token requested failed.")
        else:
            print("No need to request a new WNS token.")

        print("Server started.")

    def __shouldRequestWnsToken(self):
        token: Optional[WNSToken] = None
        try:
            token: WNSToken = WNSToken.get_by_id(1)
        except DoesNotExist:
            print("No WNS token found.")
            return True
        return self.__isWnsTokenExpired(token)
    
    def __isWnsTokenExpired(self, token: WNSToken):
        date = datetime.now(timezone.utc)
        date += timedelta(hours=2) # Two hours backup before a token expires
        return token.expires <= date

    def stop(self):
        print("Stopping the server...")
        self.tcpServer.requestStop()
        self.tcpServer.removeValidMessageReceivedCallback(self.__onValidMessageReceived)
        self.tcpServer.join()
        print("Server stopped.")

    def __updateChannelUri(self, deviceId: str, channelUri: str, sock: socket):
        ChannelUri.replace(deviceId=deviceId, channelUri=channelUri)
        self.tcpServer.respondClientWithSuccessMessage(sock)
        print("Channel URI set for 'device' {} to: {}".format(deviceId, channelUri))

    def __updatePushDevices(self, deviceId: str, accounts: List[str], sock:socket):
        channelUri = ChannelUri.get(ChannelUri.deviceId == deviceId)
        if not channelUri:
            print("Update push for unknown device id.")
            self.tcpServer.respondClientWithErrorMessage("Device id unknown.", sock)
            return
        linesAffected: int = PushAccount.delete().where(PushAccount.channelUri == channelUri)
        print("Removed {} old push accounts for device '{}'.".format(linesAffected, deviceId))

        accountsResult: List[PushAccount] = list()
        accountsResponse: List[Tuple[str, str, str]] = list()
        for account in accounts:
            pAcc: PushAccount = PushAccount.createFrom(channelUri, account)
            pAcc.generate(account)
            accountsResult.append(pAcc)
            accountsResponse.append(account, pAcc.node, pAcc.secret)

        # Send the success response:
        self.tcpServer.sendToClient(str(SuccessSetPushAccountsMessage(accountsResponse)), sock)
        print("Set {} push device(s) for device '{}'.".format(len(accountsResult), deviceId))

    def __sendTestPush(self, deviceId: str, sock: socket):
        channelUri = ChannelUri.get(ChannelUri.deviceId == deviceId)
        if not channelUri:
            print("Test push for unknown device id.")
            self.tcpServer.respondClientWithErrorMessage("Device id unknown.", sock)
            return
        self.wnsClient.sendRawNotification(channelUri.channelUri, "Test push notification from your push server.")

    # Handle all incoming messages:
    def __onValidMessageReceived(self, msg: AbstractMessage, sock: socket):
        if isinstance(msg, SetChannelUriMessage):
            self.__updateChannelUri(msg.deviceId, msg.channelUri, sock)
        elif isinstance(msg, SetPushAccountsMessage):
            self.__updatePushDevices(msg.deviceId, msg.accounts, sock)
        elif isinstance(msg, RequestTestPushMessage):
            self.__sendTestPush(msg.deviceId, sock)
        else:
            self.tcpServer.respondClientWithErrorMessage("Unsupported message type.", sock)
