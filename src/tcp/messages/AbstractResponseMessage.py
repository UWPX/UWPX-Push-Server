from tcp.messages.AbstractMessage import AbstractMessage
from typing import Any

class AbstractResponseMessage(AbstractMessage):
    ACTION: str = "response"
    status: int

    def __init__(self, status: int):
        super(AbstractResponseMessage, self).__init__(action = self.ACTION)
        self.status = status

    def _fromJson(self, jsonObj: Any):
        super()._fromJson(jsonObj)
        self.status = jsonObj["status"]

    def _toJson(self):
        jsonObj: Any = super()._toJson()
        jsonObj["status"] = self.status
        return jsonObj