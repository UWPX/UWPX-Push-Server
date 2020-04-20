import requests
from typing import Dict, Any, Optional
from datetime import datetime, timedelta, timezone
from db.dbManager import WNSTokenModel
from peewee import DoesNotExist
from traceback import print_exc
from sys import stdout
from enum import Enum

class WNSTokenParseException(Exception):
    field: str

    def __init__(self, field: str):
        self.field = field

class WNSToken:
    tokenType: str
    token: str
    expires: datetime

    def __init__(self, tokenType: str, token: str, expires: datetime):
        self.tokenType = tokenType
        self.token = token
        self.expires = expires

    def toAuthorizationString(self):
        return "{} {}".format(self.tokenType, self.token)

    @classmethod
    def getExpireDateUtc(cls, expiresInSeconds: int):
        return datetime.now(timezone.utc) + timedelta(seconds=expiresInSeconds)

    def toWNSTokenModel(self):
        return WNSTokenModel.create(token=self.token, tokenType=self.tokenType, expires=self.expires)

    @classmethod
    def fromWNSTokenModel(cls, tokenModel: WNSTokenModel):
        return WNSToken(tokenModel.tokenType, tokenModel.token, tokenModel.expires)

    @classmethod
    def fromResponse(cls, response: requests.Response):
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
            expiresInSeconds: int = int(data["expires_in"])
        except ValueError:
            raise WNSTokenParseException("expires_in")

        return WNSToken(tokenType, token, WNSToken.getExpireDateUtc(expiresInSeconds))

class ResponseCodeAction(Enum):
    SUCCESS = 0,
    RETRY = 1,
    ERROR = 2

class WNSClient:
    REQUEST_TOKEN_URL: str = "https://login.live.com/accesstoken.srf"

    packetSID: str
    clientSecret: str
    token: Optional[WNSToken]

    # ---------------------------
    # Response codes for WNS requests
    # Source: https://docs.microsoft.com/en-us/previous-versions/windows/apps/hh465435(v=win.10)#response-codes
    # ---------------------------

    # The notification was accepted by WNS.
    STATUS_CODE_OK: int = 200
    # One or more headers were specified incorrectly or conflict with another header.
    STATUS_CODE_BAD_REQUEST: int = 400
    # The cloud service did not present a valid authentication ticket. The OAuth ticket may be invalid.
    STATUS_CODE_UNAUTHORIZED: int = 401
    # The cloud service is not authorized to send a notification to this URI even though they are authenticated.
    STATUS_CODE_FORBIDDEN: int = 403
    # The channel URI is not valid or is not recognized by WNS.
    STATUS_CODE_NOT_FOUND: int = 404
    # Invalid method (GET, CREATE); only POST (Windows or Windows Phone) or DELETE (Windows Phone only) is allowed.
    STATUS_CODE_METHOD_NOT_ALLOWED: int = 405
    # The cloud service exceeded its throttle limit.
    STATUS_CODE_NOT_ACCEPTABLE: int = 406
    # The channel expired.
    STATUS_CODE_GONE: int = 410
    # The notification payload exceeds the 5000 byte size limit.
    STATUS_CODE_REQUEST_ENTRY_TOO_LARGE: int = 413
    # An internal failure caused notification delivery to fail.
    STATUS_CODE_INTERNAL_SERVER_ERROR: int = 500
    # The server is currently unavailable.
    STATUS_CODE_SERVICE_UNAVAILABLE: int = 503

    def __init__(self, packetSID: str, clientSecret: str):
        self.packetSID = packetSID
        self.clientSecret = clientSecret
        self.token = None
    
    def loadTokenFromDb(self):
        try:
            self.token = WNSToken.fromWNSTokenModel(WNSTokenModel.get_by_id(1))
            print("WNS token successfully loaded.")
        except DoesNotExist:
            self.token = None
            print("No WNS token found.")

    def isTokenExpired(self):
        # A two hours backup window before the token expires:
        return self.token is None or self.token.expires <= (datetime.now(timezone.utc) + timedelta(hours=2))

    # https://docs.microsoft.com/en-us/previous-versions/windows/apps/hh465435(v=win.10)
    def requestToken(self):
        data: Dict[str, str] = {"grant_type": "client_credentials",
                                "client_id": self.packetSID,
                                "client_secret": self.clientSecret,
                                "scope": "notify.windows.com"}
        response: requests.Response = requests.post(url = self.REQUEST_TOKEN_URL, data = data)

        if response.status_code != self.STATUS_CODE_OK:
            print("Requesting a new token failed. Status code: {}".format(response.status_code))
            print("Response: {}".format(response.json()))
            return False

        try:
            self.token = WNSToken.fromResponse(response)
        except Exception as e:
            print("Failed to parse WNS token response: {} Exception: {}".format(response.content, e))
            print_exc(file=stdout)
            return False
        print("Successfully requested a new WNS token.")
        return True

    def sendRawNotification(self, channelUri, content: str):
        print("Sending raw notification...")
        if self.isTokenExpired():
            print("WNS token expired. requesting a new one...")
            if not self.requestToken():
                print("Failed to send raw notification. Requesting a new WNS token failed.")
                return False

        retryCount: int = 0

        while retryCount < 3:
            # https://docs.microsoft.com/en-us/previous-versions/windows/apps/hh465435(v=win.10)#request-parameters
            headers: Dict[str, str] = {"X-WNS-Type": "wns/raw",
                                       "X-WNS-RequestForStatus": "true",
                                       "Content-Type": "application/octet-stream",
                                       "Authorization": self.token.toAuthorizationString()}
            response: requests.Response = requests.post(url = channelUri, headers = headers, data = content)

            result: ResponseCodeAction = self.handleSendRawNotificationResponseCodes(response)
            if result == ResponseCodeAction.SUCCESS:
                return True
            elif result == ResponseCodeAction.RETRY:
                retryCount += 1
                continue
            else:
                return False
        return False

    def handleSendRawNotificationResponseCodes(self, response: requests.Response):
        code: int = response.status_code
        if code == self.STATUS_CODE_OK:
            print("Successfully send raw notification.")
            return ResponseCodeAction.SUCCESS
        elif code == self.STATUS_CODE_BAD_REQUEST:
            print("Sending a raw notification failed. Malformed headers.")
            return ResponseCodeAction.ERROR
        elif code == self.STATUS_CODE_UNAUTHORIZED:
            print("Sending a raw notification failed. WNS token invalid.")
            print("Requesting a new WNS token...")
            if self.requestToken():
                return ResponseCodeAction.RETRY
            return ResponseCodeAction.ERROR
        elif code == self.STATUS_CODE_FORBIDDEN:
            print("Sending a raw notification failed. WNS token does not match the app credentials.")
            return ResponseCodeAction.ERROR
        elif code == self.STATUS_CODE_NOT_FOUND:
            print("Sending a raw notification failed. Channel URI is not valid.")
            return ResponseCodeAction.ERROR
        elif code == self.STATUS_CODE_METHOD_NOT_ALLOWED:
            print("Sending a raw notification failed. HTTP POST should be used.")
            return ResponseCodeAction.ERROR
        elif code == self.STATUS_CODE_NOT_ACCEPTABLE:
            print("Sending a raw notification failed. Too many notifications in to short of a time.")
            return ResponseCodeAction.ERROR
        elif code == self.STATUS_CODE_GONE:
            print("Sending a raw notification failed. Channel URI expired.")
            return ResponseCodeAction.ERROR
        elif code == self.STATUS_CODE_REQUEST_ENTRY_TOO_LARGE:
            print("Sending a raw notification failed. Payload > 5000 byte.")
            return ResponseCodeAction.ERROR
        elif code == self.STATUS_CODE_INTERNAL_SERVER_ERROR:
            print("Sending a raw notification failed. Internal WNS server error.")
            return ResponseCodeAction.RETRY
        elif code == self.STATUS_CODE_SERVICE_UNAVAILABLE:
            print("Sending a raw notification failed. WNS server currently unavailable.")
            return ResponseCodeAction.RETRY
        else:
            print("Sending a raw notification failed. Status code: {}".format(code))
            print("Response: {}".format(response.content))
            return ResponseCodeAction.ERROR

