from threading import Thread
from aioxmpp import PresenceManagedClient, make_security_layer, JID
import asyncio

class XmppClient(Thread):
    bareJid: str
    password: str

    shouldRun: bool

    def __init__(self, bareJid: str, password: str):
        Thread.__init__(self, name=("XMPPClientThread"))
        self.bareJid = bareJid
        self.password = password
        self.shouldRun: bool = False

    def run(self):
        loop: asyncio.AbstractEventLoop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        loop.run_until_complete(self.runAsync())
        loop.close()

    async def runAsync(self):
        print("Starting the XMPP client...")
        self.shouldRun: bool = True

        client: PresenceManagedClient = PresenceManagedClient(JID.fromstr(self.bareJid), make_security_layer(self.password))
        
        async with client.connected():
            while self.shouldRun:
                await asyncio.sleep(1)

    
    def requestStop(self):
        print("Stopping the XMPP client...")
        self.shouldRun: bool = False