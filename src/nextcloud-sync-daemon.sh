#!/bin/sh
GCONF_KEY_NEXTCLOUD_PATH="/system/maemo/sync/nextcloud/sync-path"
GCONF_KEY_NEXTCLOUD_HOST="/system/maemo/sync/nextcloud/host"
GCONF_KEY_NEXTCLOUD_USER="/system/maemo/sync/nextcloud/username"
GCONF_KEY_NEXTCLOUD_PASS="/system/maemo/sync/nextcloud/password"

path="$(gconftool-2 -g $GCONF_KEY_NEXTCLOUD_PATH)"
host="$(gconftool-2 -g $GCONF_KEY_NEXTCLOUD_HOST)"
user="$(gconftool-2 -g $GCONF_KEY_NEXTCLOUD_USER)"
pass="$(gconftool-2 -g $GCONF_KEY_NEXTCLOUD_PASS)"

if [ -z "$path" ]; then
	echo "Error: Sync path is empty." >&2
	exit 1
fi

if [ -z "$path" ]; then
	echo "Error: Host is empty." >&2
	exit 1
fi

if [ -z "$path" ]; then
	echo "Error: Username is empty." >&2
	exit 1
fi

if [ -z "$path" ]; then
	echo "Error: Password is empty." >&2
	exit 1
fi

cd "$path" || {
	echo "Error: Could not chdir." >&2
	exit 1
}

# 394 = MODIFY|CLOSE_WRITE|MOVED_TO|CREATE
wendy -rd -m 394 . \
	nextcloudcmd --non-interactive -u "$user" -p "$pass" . "$host" &
