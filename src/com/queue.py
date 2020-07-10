from com.task import Task
from asyncio import Queue as AsQueue
from threading import Semaphore
from typing import Optional


class Queue:
    __queue: AsQueue
    __queueSema: Semaphore

    def __init__(self):
        self.__queue: AsQueue = AsQueue(maxsize=0)  # Create an infinite queue
        self.__queueSema: Semaphore = Semaphore()

    def put(self, item: Task):
        self.__queueSema.acquire()
        self.__queue.put_nowait(item)
        self.__queueSema.release()

    async def popAsync(self):
        self.__queueSema.acquire()
        item: Optional[Task] = None
        if len(self.__queue) > 0:
            item = await self.__queue.get()
        self.__queueSema.release()
        return item
