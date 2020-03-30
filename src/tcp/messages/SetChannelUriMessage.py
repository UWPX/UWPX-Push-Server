from tcp.messages.AbstractMessage import AbstractMessage
from typing import Any

class SetChannelUriMessage(AbstractMessage):
    ACTION: str = "set_channel_uri"

    channelUri: str
    deviceId: str

    def __init__(self, jsonObj: Any):
        super(SetChannelUriMessage, self).__init__(jsonObj = jsonObj)

    def _fromJson(self, jsonObj: Any):
        super()._fromJson(jsonObj)
        self.channelUri = jsonObj["channel_uri"]
        self.deviceId = jsonObj["device_id"]

    def _toJson(self):
        jsonObj: Any = super()._toJson()
        jsonObj["channel_uri"] = self.channelUri
        jsonObj["device_id"] = self.deviceId
        return jsonObj