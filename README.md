# XMPP-Push Server
This is the push server for XMPP client [UWPX](https://github.com/UWPX/UWPX-Client).

## Protocol
The client connects to server and transmits his push url and all of its accounts to the server.

### Client -> Server
```JSON
{
	"version": 1,
	"url": "ms-app://s-1-15-2-3598129719-3378870262-4208132049-182512184-2493220926-1891298429-4035237700",
	"accounts": [{
			"jid": "someexample1@xmpp.exaple.org"
		},
		{
			"jid": "someexample2@xmpp.exaple.org"
		}
	]
}
```

### Server -> Client Response
#### Success
```JSON
{
	"version": 1,
	"status": 1
}
```

#### Error
```JSON
{
	"version": 1,
	"status": 0,
    "error": "Some error text"
}
```
