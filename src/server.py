#!/usr/bin/python3

from enum import Enum
from wns.wnslib import WNSClient
from tcp.tcpServer import TcpServer
from typing import Dict, Any, List, Tuple, Optional
from socket import socket
from tcp.messages.AbstractMessage import AbstractMessage
from tcp.messages.SetChannelUriMessage import SetChannelUriMessage
from tcp.messages.ErrorResponseMessage import ErrorResponseMessage
from tcp.messages.SetPushAccountsMessage import SetPushAccountsMessage
from tcp.messages.SuccessSetPushAccountsMessage import SuccessSetPushAccountsMessage
from tcp.messages.RequestTestPushMessage import RequestTestPushMessage
from db.dbManager import ChannelUri, PushAccount, WNSTokenModel, initDb
from peewee import DoesNotExist
from datetime import datetime, timezone, timedelta
from xmpp.xmppClient import XmppClient
from ssl import SSLSocket


class ServerState(Enum):
    NOT_RUNNING = 0
    STARTING = 1
    RUNNING = 2
    STOPPING = 3
    ERROR = 4


class Server:
    __state: ServerState
    config: Any
    wnsClient: WNSClient
    tcpServer: TcpServer
    xmppClient: XmppClient

    def __init__(self, config: Any):
        self.config = config
        self.__state = ServerState.NOT_RUNNING
        self.wnsClient = WNSClient(
            config["wns"]["packetId"], config["wns"]["clientSecret"])
        self.tcpServer = TcpServer(config["tcp"]["port"], config["tcp"]["tls"]
                                   ["serverCertPath"], config["tcp"]["tls"]["serverKeyPath"])
        self.xmppClient: XmppClient = XmppClient(
            config["xmpp"]["bareJid"], config["xmpp"]["password"], config["xmpp"]["pub_sub_jid"])

    def isRunning(self):
        return self.__state != ServerState.ERROR and self.__state != ServerState.NOT_RUNNING

    def start(self):
        if self.__state != ServerState.NOT_RUNNING and self.state != ServerState.ERROR:
            print(
                "Unable to start the server - already running. State: {} ".format(self.state))
            return
        self.__state = ServerState.STARTING
        print("Starting the server...")
        # DB
        initDb()

        # WNS:
        self.wnsClient.load_token_from_db()
        if self.wnsClient.isTokenExpired():
            print("Requesting a new WNS token...")
            if self.wnsClient.requestToken():
                WNSTokenModel.truncate_table()
                self.wnsClient.token.toWNSTokenModel().save()
                print("WNS token requested successfully.")
            else:
                print("WNS token requested failed.")
        else:
            print("No need to request a new WNS token.")

        self.__state = ServerState.RUNNING
        print("Server started.")

        # TCP:
        self.tcpServer.registerValidMessageReceivedCallback(
            self.__onValidMessageReceived)
        self.tcpServer.start()

        # XMPP client:
        self.xmppClient.start()

    def stop(self):
        if self.__state != ServerState.RUNNING:
            print("No need to stop the server - not running.")
            return

        self.__state = ServerState.STARTING
        print("Stopping the server...")
        self.tcpServer.requestStop()
        self.tcpServer.removeValidMessageReceivedCallback(
            self.__onValidMessageReceived)
        self.tcpServer.join()
        self.xmppClient.stop()
        print("Server stopped.")
        self.__state = ServerState.NOT_RUNNING

    def __updateChannelUri(self, deviceId: str, channelUri: str, sock: SSLSocket):
        ChannelUri.replace(deviceId=deviceId, channelUri=channelUri).execute()
        self.tcpServer.respondClientWithSuccessMessage(sock)
        print("Channel URI set for 'device' {} to: {}".format(deviceId, channelUri))

    def __updatePushDevices(self, deviceId: str, accounts: List[str], sock: socket):
        try:
            channelUri = ChannelUri.get(ChannelUri.deviceId == deviceId)
        except DoesNotExist:
            print("Update push for unknown device id.")
            self.tcpServer.respondClientWithErrorMessage(
                "Device id unknown.", sock)
            return
        linesAffected: int = PushAccount.delete().where(
            PushAccount.channelUri == channelUri).execute()
        print("Removed {} old push accounts for device '{}'.".format(
            linesAffected, deviceId))

        accountsResult: List[PushAccount] = list()
        accountsResponse: List[Tuple[str, str, str]] = list()
        for account in accounts:
            pAcc: PushAccount = PushAccount.createFrom(channelUri, account)
            accountsResult.append(pAcc)
            accountsResponse.append((account, pAcc.node, pAcc.secret))

        # Create and subscribe to XMPP nodes:
        try:
            if not self.xmppClient.createAndSubscribeToNode(pAcc.node):
                self.tcpServer.respondClientWithErrorMessage(
                    "Failed to create PubSub node.", sock)
                return
        except Exception as e:
            self.tcpServer.respondClientWithErrorMessage(
                "Failed to create PubSub node with: {}".format(e), sock)
            return

        # Send the success response:
        self.tcpServer.sendToClient(str(SuccessSetPushAccountsMessage(
            self.config["xmpp"]["bareJid"], accountsResponse)), sock)
        print("Set {} push accounts(s) for device '{}'.".format(
            len(accountsResult), deviceId))

    def __sendTestPush(self, deviceId: str, sock: SSLSocket):
        try:
            channelUri = ChannelUri.get(ChannelUri.deviceId == deviceId)
        except DoesNotExist:
            print("Test push for unknown device id.")
            self.tcpServer.respondClientWithErrorMessage(
                "Device id unknown.", sock)
            return
        self.wnsClient.sendRawNotification(
            channelUri.channelUri, "Test push notification from your push server.")
        self.tcpServer.respondClientWithSuccessMessage(sock)
        print("Test push notification send to: {}".format(channelUri.channelUri))

    # Handle all incoming messages:
    def __onValidMessageReceived(self, msg: AbstractMessage, sock: SSLSocket):
        if isinstance(msg, SetChannelUriMessage):
            self.__updateChannelUri(msg.deviceId, msg.channelUri, sock)
        elif isinstance(msg, SetPushAccountsMessage):
            self.__updatePushDevices(msg.deviceId, msg.accounts, sock)
        elif isinstance(msg, RequestTestPushMessage):
            self.__sendTestPush(msg.deviceId, sock)
        else:
            self.tcpServer.respondClientWithErrorMessage(
                "Unsupported message type.", sock)

    def sendTestPush(self, deviceId: str):
        try:
            channelUri = ChannelUri.get(ChannelUri.deviceId == deviceId)
        except DoesNotExist:
            print("Test push for unknown device id: \"{}\"".format(deviceId))
            return
        self.wnsClient.sendRawNotification(
            channelUri.channelUri, "Test push notification from your push server.")
        print("Test push send to device id: \"{}\"".format(deviceId))
