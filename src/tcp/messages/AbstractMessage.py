from abc import ABC, abstractmethod
import json
from typing import Optional, Any

class AbstractMessage(ABC):
    version: int
    action: str

    def __init__(self, jsonObj: Any = None, action: Optional[str] = None):
        super(AbstractMessage, self).__init__()
        if jsonObj:
            self._fromJson(jsonObj)
        else:
            self.version = 1
            self.action = action
    
    @abstractmethod
    def _fromJson(self, jsonObj: Any):
        version = jsonObj["version"]
        action = jsonObj["action"]

    @abstractmethod
    def _toJson(self):
        return {"version": self.version,
                "action": self.action}

    def __str__(self):
        return json.dumps(self._toJson())