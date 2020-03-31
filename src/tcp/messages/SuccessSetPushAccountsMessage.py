from tcp.messages.SuccessResponseMessage import SuccessResponseMessage
from typing import Any, List, Dict, Tuple

class SuccessSetPushAccountsMessage(SuccessResponseMessage):
    deviceId: str
    # List of accounts where each account has a 'bare_jid', 'node' and 'secret'
    accounts: List[Tuple[str, str, str]]

    def __init__(self, accounts: List[Tuple[str, str, str]]):
        super(SuccessSetPushAccountsMessage, self).__init__()
        self.accounts = accounts

    def _fromJson(self, jsonObj: Any):
        super()._fromJson(jsonObj)
        self.deviceId = jsonObj["device_id"]

        self.accounts = list()
        jsonAccountsObj: Any = jsonObj["accounts"]
        for account in jsonAccountsObj:
            self.accounts.append((account["bare_jid"], account["node"], account["secret"]))


    def _toJson(self):
        jsonObj: Any = super()._toJson()
        jsonObj["device_id"] = self.deviceId

        jsonAccountsObj: List[Dict[str, Any]] = list()
        for account in self.accounts:
            jsonAccountsObj.append({"bare_jid": account[0], "node": account[1], "secret": account[2] })
        jsonObj["accounts"] = jsonAccountsObj

        return jsonObj