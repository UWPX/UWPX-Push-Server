from tcp.messages.AbstractResponseMessage import AbstractResponseMessage
from typing import Any

class ErrorResponseMessage(AbstractResponseMessage):
    STATUS: int = 1

    error: str

    def __init__(self, error: str):
        super(ErrorResponseMessage, self).__init__(self.STATUS)
        self.error = error

    def _fromJson(self, jsonObj: Any):
        super()._fromJson(jsonObj)
        self.error = jsonObj["error"]

    def _toJson(self):
        jsonObj: Any = super()._toJson()
        jsonObj["error"] = self.error
        return jsonObj
