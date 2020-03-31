from secrets import token_hex, token_urlsafe
from hashlib import sha256

class PushAccount:
    deviceId: str
    bareJidHash: str
    domainPart: str
    node: str
    secret: str

    def __init__(self, deviceId: str):
        super().__init__()
        self.deviceId = deviceId
        self.bareJidHash: str = ""
        self.domainPart: str = ""
        self.node: str = ""
        self.secret: str = ""

    def generate(self, bareJid: str):
        # Genrate a secure random node and secret:
        self.node = token_urlsafe(128)
        self.secret = token_hex(128)

        self.bareJidHash = self.__genBareJidHash(bareJid)
        self.domainPart = self.__getDomainPart(bareJid)
    
    def __genBareJidHash(self, bareJid: str):
        # Based on: https://gist.github.com/markito/30a9bc2afbbfd684b31986c2de305d20
        return sha256(self.deviceId + bareJid).hexdigest()
    
    def __getDomainPart(self, bareJid: str):
        return bareJid