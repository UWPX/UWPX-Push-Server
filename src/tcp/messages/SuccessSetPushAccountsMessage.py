from tcp.messages.SuccessResponseMessage import SuccessResponseMessage
from typing import Any, List, Dict, Tuple

class SuccessSetPushAccountsMessage(SuccessResponseMessage):
    pushBareJid: str
    # List of accounts where each account has a 'bare_jid', 'node' and 'secret'
    accounts: List[Tuple[str, str, str]]

    def __init__(self, pushBareJid: str, accounts: List[Tuple[str, str, str]]):
        super(SuccessSetPushAccountsMessage, self).__init__()
        self.pushBareJid = pushBareJid
        self.accounts = accounts

    def _fromJson(self, jsonObj: Any):
        super()._fromJson(jsonObj)
        self.pushBareJid = jsonObj["push_bare_jid"]

        self.accounts = list()
        jsonAccountsObj: Any = jsonObj["accounts"]
        for account in jsonAccountsObj:
            self.accounts.append((account["bare_jid"], account["node"], account["secret"]))


    def _toJson(self):
        jsonObj: Any = super()._toJson()
        jsonObj["push_bare_jid"] = self.pushBareJid

        jsonAccountsObj: List[Dict[str, Any]] = list()
        for account in self.accounts:
            jsonAccountsObj.append({"bare_jid": account[0], "node": account[1], "secret": account[2] })
        jsonObj["accounts"] = jsonAccountsObj

        return jsonObj