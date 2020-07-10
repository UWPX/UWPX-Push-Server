from threading import Semaphore
from enum import Enum
from typing import Any, Callable, Awaitable, Optional


class TaskState(Enum):
    ENQUEUED = 0,
    PROCESSING = 1,
    SUCCESS = 2,
    ERROR = 3


class Task:
    __state: TaskState
    __waitSema: Semaphore
    __func: Callable[..., Awaitable[Any]]
    __args: Any
    __timeoutSec: float
    __result: Any
    __exc: Optional[Exception]

    def __init__(self, func: Callable[..., Awaitable[Any]], *args: Any):
        self.__state: TaskState = TaskState.ENQUEUED
        self.__waitSema: Semaphore = Semaphore(0)  # Create a blocked semaphore
        self.__func: Callable[..., Awaitable[Any]] = func
        self.__args: Any = args
        self.__timeoutSec: float = 5.0  # Default to a 5 sec timeout for execution
        self.__result: Any = None
        self.__exc: Optional[Exception] = None

    def setTimeout(self, timeoutSec: float = 5.0):
        self.__timeoutSec = timeoutSec

    def waitForCompletion(self):
        return self.__waitSema.acquire(True, self.__timeoutSec)

    def __setResult(self, result: Any):
        self.__result = result
        self.__state = TaskState.SUCCESS
        self.__waitSema.release()

    def __setError(self, exc: Exception):
        self.__exc = exc
        self.__state = TaskState.ERROR
        self.__waitSema.release()

    async def executeAsync(self):
        try:
            result: Any = await self.__func(*self.__args)
            self.__set_result(result)
        except Exception as exc:
            print("Executing task failed with: {}".format(exc))
            self.__set_error(exc)
