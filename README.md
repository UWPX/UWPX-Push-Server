# XMPP-Push Server
This is the push server for XMPP client [UWPX](https://github.com/UWPX/UWPX-Client).
I based my implementation roughly on the [Conversations Push Proxy](https://github.com/iNPUTmice/p2) by [@iNPUTmice](https://github.com/iNPUTmice).

## Terminology
 * `push server`: The [UWPX](https://github.com/UWPX/UWPX-Client) push server (this magic piece of software).
 * `user's XMPP server`: The XMPP server of the user.
 * `client`: The device running [UWPX](https://github.com/UWPX/UWPX-Client).

## Protocol
The following protocol is used to register and send [XEP-0357: Push Notifications](https://xmpp.org/extensions/xep-0357.html) to devices running [UWPX](https://github.com/UWPX/UWPX-Client).  
All of the following examples represent real world messages that have been send from and to test devices.

### Publishing the channel URI
During startup [UWPX](https://github.com/UWPX/UWPX-Client) will request a new [push notification channel](https://docs.microsoft.com/en-us/uwp/api/windows.networking.pushnotifications.pushnotificationchannelmanager.createpushnotificationchannelforapplicationasync#Windows_Networking_PushNotifications_PushNotificationChannelManager_CreatePushNotificationChannelForApplicationAsync) for the [Windows Push Notification Services (WNS)](https://docs.microsoft.com/en-us/windows/uwp/design/shell/tiles-and-notifications/windows-push-notification-services--wns--overview).  
On success [UWPX](https://github.com/UWPX/UWPX-Client) will receive a `channel_uri` which represents the Uniform Resource Identifier (URI) to which the push server sends push notification for the device.

This `channel_uri` will be send in combination with the `device_id` to this push server.

Hereby the `device_id` represents a unique hex string identifying the sender uniquely.
This is required to correlate all of the accounts for which a device wants to receive push notifications with the `channel_uri`.
Without this identifier, it wouldn't be possible to update/remove push channels from the server again.

#### Client -> Server
```JSON
{
	"version": 1,
	"action": "set_channel_uri",
	"device_id": "5486bd868050a620141f4e81c9f1d2c67ab0de27e5e26d218ca41c9394ee806b",
	"channel_uri": "ms-app://s-1-15-2-3598129719-3378870262-4208132049-182512184-2493220926-1891298429-4035237700"
}
```

#### Success: Server -> Client
```JSON
{
	"version": 1,
	"action": "response",
	"status": 1
}
```

#### Error: Server -> Client
```JSON
{
	"version": 1,
	"action": "response",
	"status": 0,
	"error": "Some error message e.g. Invalid JSON format."
}
```

### Update push accounts
The message has to include **all** accounts (Bare JIDs) for those, that should receive push notifications.
Once received by the server, it will remove all other accounts (Bare JIDs) that are not being included in the received `accounts` array.
This ensures, the client only receives notifications for those devices, that are still active.

#### Client -> Server
```JSON
{
	"version": 1,
	"action": "set_accounts",
	"device_id": "5486bd868050a620141f4e81c9f1d2c67ab0de27e5e26d218ca41c9394ee806b",
	"accounts": [
		{
			"bare_jid": "someClient@xmpp.uwpx.org"
		},
		{
			"bare_jid": "someOtherClient@xmpp.uwpx.org"
		}
	]
}
```

#### Success: Server -> Client
On success the server returns a success message, including a `node` and `secret` attribute for each account.
It also includes a `push_bare_jid` field, which represents the bare JID of the push server.
```JSON
{
	"version": 1,
	"action": "response",
	"status": 1,
	"push_bare_jid": "push@xmpp.uwpx.org",
	"accounts": [
		{
			"bare_jid": "someClient@xmpp.uwpx.org",
			"node": "773bds9nf932",
			"secret": "sdf/82h)=1",
		},
		{
			"bare_jid": "someOtherClient@xmpp.uwpx.org",
			"node": "8w3rn0MB3m38z2",
			"secret": "j$o909mN87!n/0m",
		}
	]
}
```

#### Error: Server -> Client
```JSON
{
	"version": 1,
	"action": "response",
	"status": 0,
	"error": "Some error message e.g. Invalid JSON format."
}
```

## What the push server stores
The push server stores the following information persistentend.

| `deviceId` | `channelUri` | `timeStamp` |
|:-:|:-:|:-:|
| 5486bd868050a620141f4e81c9f1d2c67ab0de27e5e26d218ca41c9394ee806b | ms-app://s-1-15-2-3598129719-3378870262-4208132049-182512184-2493220926-1891298429-4035237700 | 2020-03-31T02:51:53Z |

The `timeStamp` column represents a UTC timestamp when the entry last has been updated.
This allows the server to invalidate and remove outdated entries.

| `deviceId` | `bareJidHash` | `domainPart` | `node` | `secret` |
|:-:|:-:|:-:|:-:|:-:|
| 5486bd868050a620141f4e81c9f1d2c67ab0de27e5e26d218ca41c9394ee806b | e5397e96c6ff4d629ab9f203eec3ff17c777b2127cf1b41005e54877487ba982 | xmpp.uwpx.org | 773bds9nf932 | sdf/82h)=1 |

The `bareJidHash` column represents the bare JID (e.g. someClient@xmpp.uwpx.org ) hashed using SHA-256 with the `deviceId` (e.g. 5486bd868050a620141f4e81c9f1d2c67ab0de27e5e26d218ca41c9394ee806b) as salt.

## What the app server sends via the WNS to the client

## What the user's XMPP server sends to the app server

## Dependencies
* [Peewee](https://github.com/coleifer/peewee) is a simple and small ORM. It has few (but expressive) concepts, making it easy to learn and intuitive to use. It allows the push server to interact with the sqlite3 DB from multiple threads.

## Installation
```BASH
$ pip3 install --user -r requirements.txt
```

## Execution
### Start
```BASH
$ cd src/
$ python3 main.py
```

### Stop
`Ctrl + C`