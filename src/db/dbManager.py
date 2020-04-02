from peewee import *
from datetime import datetime, timezone
from secrets import token_hex, token_urlsafe
from hashlib import sha256
from aioxmpp import JID

'''
Documentation for peewee:
https://docs.peewee-orm.com/en/latest/index.html
'''

DB_PATH: str = "pushServer.db"
db: SqliteDatabase = SqliteDatabase(DB_PATH)

# Required since SQLite stores datetime objects as string in a to peewee unknown format
# in case they have an UTC offset.
class DateTimeTZField(DateTimeField):
    def python_value(self, value):
        if value is None:
            return
        return datetime.strptime(value, '%Y-%m-%d %H:%M:%S.%f%z')

    def db_value(self, value):
        return value.strftime('%Y-%m-%d %H:%M:%S.%f%z') if value else None

class BaseModel(Model):
    class Meta:
        database = db


class ChannelUri(BaseModel):
    deviceId: TextField = TextField(unique=True)
    channelUri: TextField = TextField(null=False)
    timeStamp: DateTimeTZField = DateTimeTZField(default=datetime.now(timezone.utc), null=False)


class PushAccount(BaseModel):
    channelUri: ForeignKeyField(ChannelUri, backref="pushAccounts")
    bareJidHash: TextField = TextField(null=False)
    domainPart: TextField = TextField(null=False)
    node: TextField = TextField(null=False)
    secret: TextField = TextField(null=False)


class WNSTokenModel(BaseModel):
    token: TextField = TextField(unique=True)
    tokenType: TextField = TextField(unique=True)
    expires: DateTimeTZField = DateTimeTZField(default=datetime.now(timezone.utc), null=False)

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
        return JID.fromstr(bareJid).domain


def initDb():
    print("Initalizing the DB...")
    db.connect()
    db.create_tables([ChannelUri, PushAccount, WNSTokenModel])
    print("DB initialized.")
