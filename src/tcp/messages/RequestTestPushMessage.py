from tcp.messages.AbstractMessage import AbstractMessage
from typing import Any

class RequestTestPushMessage(AbstractMessage):
    ACTION: str = "request_test_push"

    deviceId: str

    def __init__(self, jsonObj: Any):
        super(RequestTestPushMessage, self).__init__(jsonObj = jsonObj)

    def _fromJson(self, jsonObj: Any):
        super()._fromJson(jsonObj)
        self.deviceId = jsonObj["device_id"]

    def _toJson(self):
        jsonObj: Any = super()._toJson()
        jsonObj["device_id"] = self.deviceId
        return jsonObj