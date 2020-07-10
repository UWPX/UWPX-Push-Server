from aioxmpp import PresenceManagedClient, make_security_layer, JID
from aioxmpp import PubSubClient
from typing import Optional, Any
from asgiref.sync import async_to_sync
from threading import Thread
from com.queue import Queue
from com.task import Task
import asyncio


class XmppClient(Thread):
    __resourcePart: str = "UWPX_Push_Server"

    __bareJid: JID
    __password: str
    __pubSubJid: JID

    __client: Optional[PresenceManagedClient]
    __pubSub: Optional[PubSubClient]

    __queue: Queue

    shouldRun: bool

    def __init__(self, bareJid: str, password: str, pubSubJid: str):
        Thread.__init__(self, name=("XMPPClientThread"))
        self.__bareJid: JID = JID.fromstr(bareJid)
        self.__password: str = password
        self.__pubSubJid: JID = JID.fromstr(pubSubJid)
        self.__client: Optional[PresenceManagedClient] = None
        self.__pubSub: Optional[PubSubClient] = None

        self.__queue: Queue = Queue()

        self.shouldRun: bool = False

    def run(self):
        loop: asyncio.AbstractEventLoop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)
        loop.run_until_complete(self.__runAsync())
        loop.close()

    async def __runAsync(self):
        print("Starting the XMPP client...")
        self.shouldRun: bool = True

        self.__client = PresenceManagedClient(
            self.__bareJid, make_security_layer(self.__password))

        async with self.__client.connected():
            self.__setupPush()
            while self.shouldRun:
                await self.executeQueueAsync()
                await asyncio.sleep(0.25)

    async def executeQueueAsync(self):
        task: Optional[Task] = await self.__queue.popAsync()
        if task:
            print("Executing task")
            await task.executeAsync()
            print("Task executed")

    def __setupPush(self):
        self.__pubSub: PubSubClient = self.__client.summon(PubSubClient)
        self.__pubSub.on_subscription_update.connect(
            self.__pubSubOnSubscriptionUpdate)
        self.__pubSub.on_item_published.connect(self.__pubSubOnItemPublished)
        self.__pubSub.on_node_deleted.connect(self.__pubSubOnNodeDeleted)

    def __pubSubOnSubscriptionUpdate(self):
        pass

    def __pubSubOnItemPublished(self):
        pass

    def __pubSubOnNodeDeleted(self):
        pass

    async def subscribeToNodeAsync(self, node: str):
        return await self.__pubSub.subscribe(self.__pubSubJid, node)

    async def unsubscribeFromNodeAsync(self, node: str, subId: Optional[str] = None):
        return await self.__pubSub.unsubscribe(self.__pubSubJid, node, subid=subId)

    def requestStop(self):
        print("Stopping the XMPP client...")
        self.shouldRun: bool = False

    def addTask(self, task: Task):
        self.__queue.put(task)
