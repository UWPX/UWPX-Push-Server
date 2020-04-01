from peewee import *
from datetime import datetime, timezone
from secrets import token_hex, token_urlsafe
from hashlib import sha256

'''
Documentation for peewee:
https://docs.peewee-orm.com/en/latest/index.html
'''

DB_PATH: str = "pushServer.db"
db: SqliteDatabase = SqliteDatabase(DB_PATH)

class BaseModel(Model):
    class Meta:
        database = db


class ChannelUri(BaseModel):
    deviceId: CharField = CharField(unique=True)
    channelUri: CharField = CharField(null=False)
    timeStamp: DateTimeField(default=datetime.now(timezone.utc), null=False)


class PushAccount(BaseModel):
    channelUri: ForeignKeyField(ChannelUri, backref="pushAccounts")
    bareJidHash: CharField = CharField(null=False)
    domainPart: CharField = CharField(null=False)
    node: CharField = CharField(null=False)
    secret: CharField = CharField(null=False)


class WNSToken(BaseModel):
    token: CharField = CharField(unique=True)
    expires: DateTimeField(default=datetime.now(timezone.utc))

    @classmethod
    def createFrom(cls, channelUri: ChannelUri, bareJid: str):
        bareJidHash: str = cls.__genBareJidHash(channelUri.deviceId, bareJid)
        domainPart: str = cls.__getDomainPart(bareJid)
        # Genrate a secure random node and secret:
        node = token_urlsafe(128)
        secret = token_hex(128)

        return cls.create(channelUri=channelUri, bareJidHash=bareJidHash, domainPart=domainPart, node=node, secret=secret)
    
    @classmethod
    def __genBareJidHash(cls, deviceId: str, bareJid: str):
        # Based on: https://gist.github.com/markito/30a9bc2afbbfd684b31986c2de305d20
        return sha256(deviceId + bareJid).hexdigest()
    
    @classmethod
    def __getDomainPart(cls, bareJid: str):
        return bareJid


def initDb():
    print("Initalizing the DB...")
    db.connect()
    db.create_tables([ChannelUri, PushAccount, WNSToken])
    print("DB initialized.")
