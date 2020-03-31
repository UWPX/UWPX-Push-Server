from tcp.messages.AbstractMessage import AbstractMessage
from typing import Any, List, Dict

class SetPushAccountsMessage(AbstractMessage):
    ACTION: str = "set_push_accounts"

    deviceId: str
    accounts: List[str]

    def __init__(self, jsonObj: Any):
        super(SetPushAccountsMessage, self).__init__(jsonObj = jsonObj)

    def _fromJson(self, jsonObj: Any):
        super()._fromJson(jsonObj)
        self.deviceId = jsonObj["device_id"]

        self.accounts = list()
        jsonAccountsObj: Any = jsonObj["accounts"]
        for account in jsonAccountsObj:
            self.accounts.append(account["bare_jid"])


    def _toJson(self):
        jsonObj: Any = super()._toJson()
        jsonObj["device_id"] = self.deviceId

        jsonAccountsObj: List[Dict[str, Any]] = list()
        for account in self.accounts:
            jsonAccountsObj.append({"bare_jid": account})
        jsonObj["accounts"] = jsonAccountsObj

        return jsonObj