import json
from tcp.messages.SetChannelUriMessage import SetChannelUriMessage
from typing import Any

def parseJson(jsonObj: Any):
    action: str = jsonObj["action"]

    if action == SetChannelUriMessage.ACTION:
        return SetChannelUriMessage(jsonObj)

    print("Unknown message received: {}".format(jsonObj))
    return None

def parseJsonSave(jsonObj: Any):
    try:
        return parseJson(jsonObj)
    except Exception as e:
        print("Unknown message received: {}".format(jsonObj))
        return None

def parse(msg: str):
    jsonObj: Any = None
    try:
        jsonObj = json.loads(msg)
    except ValueError as e:
        print("Failed to parse received message '{}' as JSON: {}".format(msg, e))
        return None
    return parseJsonSave(jsonObj)