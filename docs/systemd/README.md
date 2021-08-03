# systemd Configuration

This directory contains the `redis` and `push` [systemd](https://de.wikipedia.org/wiki/Systemd) service configuration files.

## Requirements

* A user called `redis`.
* A user called `push`.

## Usage

```bash
cp redis.service /etc/systemd/system/redis.service
cp push.service /etc/systemd/system/push.service
systemctl enable redis
systemctl enable push
reboot -h now
```