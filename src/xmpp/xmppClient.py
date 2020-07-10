from slixmpp import ClientXMPP
from slixmpp.exceptions import XMPPError
from typing import Optional, Any
from asgiref.sync import async_to_sync
from threading import Thread


"""
Based on: https://github.com/poezio/slixmpp/blob/master/examples/pubsub_client.py
"""


class XmppThread(Thread):
    __client: ClientXMPP

    def __init__(self, client: ClientXMPP):
        Thread.__init__(self, name=("XMPPClientThread"))
        self.__client: ClientXMPP = client

    def run(self):
        print("XMPP thread started.")
        self.__client.process(forever=False)
        print("XMPP thread stopped.")


class XmppClient(ClientXMPP):
    __bareJid: str
    __password: str
    __pubSubJid: str
    __xmppThread: XmppThread
    __resourcePart: str = "UWPX_Push_Server"

    def __init__(self, bareJid: str, password: str, pubSubJid: str):
        ClientXMPP.__init__(self, bareJid + '/' +
                            self.__resourcePart, password)
        self.__bareJid: str = bareJid
        self.__password: str = password
        self.__pubSubJid: str = pubSubJid
        self.__xmppThread = XmppThread(self)

        # Register plugins:
        self.register_plugin('xep_0030')  # Disco
        self.register_plugin('xep_0059')  # Result Set Management
        self.register_plugin('xep_0060')  # PubSub
        self.register_plugin('xep_0199')  # XMPP Ping

        self.add_event_handler('session_start', self.__onSessionStart)
        self.add_event_handler("message", self.__onMessage)

    def __onSessionStart(self, event: Any):
        self.get_roster()
        self.send_presence()

    def __onMessage(self, msg: Any):
        print("Message received: {}".format(msg))

    def start(self):
        print("Starting the XMPP client...")
        # Connect and start processing stanzas:
        self.connect()
        # Process messages in a different thread:
        self.__xmppThread.start()
        print("Started the XMPP client.")

    def stop(self):
        print("Stopping the XMPP client...")
        self.disconnect()
        self.__xmppThread.join()
        print("Stopped the XMPP client.")

    def createNode(self, node: str):
        try:
            async_to_sync(self['xep_0060'].create_node)(self.__pubSubJid, node)
            return True
        except XMPPError as error:
            print("Could not create node '{}': {}".format(node, error.format()))
        return False

    def subscribeToNode(self, node: str):
        try:
            iq: Any = async_to_sync(self['xep_0060'].subscribe)(
                self.pubsub_server, self.node)
            subscription = iq['pubsub']['subscription']
            print("Subscribed {} to node '{}'".format(
                subscription['jid'], subscription['node']))
            return True
        except XMPPError as error:
            print("Could not subscribe to node '{}': {}".format(
                node, error.format()))
        return False

    def configureNode(self, node: str):
        return True

    def createAndSubscribeToNode(self, node: str):
        return self.createNode(node) and self.configureNode() and self.subscribeToNode(node)
