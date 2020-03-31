import requests
from typing import Dict, Any, Optional

class WNSTokenParseException(Exception):
    field: str

    def __init__(self, field: str):
        self.field = field

class WNSToken:
    tokenType: str
    token: str
    expiresIn: int

    def __init__(self, tokenType: str, token: str, expiresIn: int):
        self.tokenType = tokenType
        self.token = token
        self.expiresIn = expiresIn

    def toAuthorizationString(self):
        return "{} {}".format(self.tokenType, self.token)

    @staticmethod
    def fromResponse(response: requests.Response):
        data: Any = response.json()
        if not "token_type" in data:
            raise WNSTokenParseException("token_type")

        tokenType: str = data["token_type"]

        if not "access_token" in data:
            raise WNSTokenParseException("access_token")

        token: str = data["access_token"]

        if not "expires_in" in data:
            raise WNSTokenParseException("expires_in")

        try:
            expiresIn: int = int(data["expires_in"])
        except ValueError:
            raise WNSTokenParseException("expires_in")

        return WNSToken(tokenType, token, expiresIn)

class WNSClient:
    REQUEST_TOKEN_URL: str = "https://login.live.com/accesstoken.srf"

    packetSID: str
    clientSecret: str
    token: Optional[WNSToken]

    def __init__(self, packetSID: str, clientSecret: str):
        self.packetSID = packetSID
        self.clientSecret = clientSecret
        self.token = None
    
    # https://docs.microsoft.com/en-us/previous-versions/windows/apps/hh465435(v=win.10)
    def requestToken(self):
        data: Dict[str, str] = {"grant_type": "client_credentials",
                                "client_id": self.packetSID,
                                "client_secret": self.clientSecret,
                                "scope": "notify.windows.com"}
        response: requests.Response = requests.post(url = self.REQUEST_TOKEN_URL, data = data)

        if response.status_code != 200:
            print("Requesting a new token failed. Status code: {}".format(response.status_code))
            print("Response: {}".format(response.json()))
            return False

        try:
            self.token = WNSToken.fromResponse(response)
        except Exception as e:
            print("Failed to parse WNS token response: {} Exception: {}".format(response.content, e))
            return False
        print("Successfully requested a new WNS token.")
        return True

    def sendRawNotification(self, channelUri, content: str):
        print("Sending raw notification...")
        # https://docs.microsoft.com/en-us/previous-versions/windows/apps/hh465435(v=win.10)#request-parameters
        headers: Dict[str, str] = {"X-WNS-Type": "wns/raw",
                                   "X-WNS-RequestForStatus": "true",
                                   "Content-Type": "application/octet-stream",
                                   "Authorization": self.token.toAuthorizationString()}
        response: requests.Response = requests.post(url = channelUri, headers = headers, data = content)

        if response.status_code != 200:
            print("Sending a raw notification failed. Status code: {}".format(response.status_code))
            print("Response: {}".format(response.content))
            return False
        print("Successfully send raw notification.")
        return True

