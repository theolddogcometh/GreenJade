#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Persist helper for make-hwtest-img: product OpenSSH 10.5p1 DUT sshd_config
# + jay authorized_keys, and the lab-host enable snippet.
#
# Live DUT VFS is kernel vfs_ram (frozen; packed rootfs not mounted). This
# stages GJ-PERSIST/ssh/ and optionally overlays a packed rootfs so init can
# drop jay keys + DUT config. Packed != Dual DoD B close.
#
# Glue (user/openssh/include/config.h): WITH_OPENSSL=1, SANDBOX_NULL=1,
# USE_PAM unset. sshd has no PassEnv keyword (would be BadOption); AcceptEnv
# is the server side of client SendEnv.
#
# Usage:
#   ./scripts/hwtest-sshd-enable.sh <persist_dir> [rootfs_dir]
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"

if [ "${1:-}" = "" ]; then
	echo "usage: $0 <persist_dir> [rootfs_dir]" >&2
	exit 1
fi
persist_dir="$1"
rootfs_dir="${2:-}"

# Same lab pubkey vfs_ram seeds for /home/jay and /root (T0 PTY_OK).
CANON_PUB='ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAICyCCjetU1+33s+YtEo5td8zFDgjetsOhUYr3+z1efK9 greenjade-hwtest@lab'

mkdir -p build/hwtest-keys
if [ ! -f build/hwtest-keys/id_ed25519 ]; then
	if command -v ssh-keygen >/dev/null 2>&1; then
		ssh-keygen -t ed25519 -N "" -C "greenjade-hwtest@lab" \
			-f build/hwtest-keys/id_ed25519 >/dev/null
		echo "hwtest-sshd-enable: generated build/hwtest-keys/id_ed25519"
	else
		echo "hwtest-sshd-enable: warn: ssh-keygen missing; using canon jay pubkey" >&2
	fi
fi

mkdir -p "$persist_dir/ssh"
chmod 755 "$persist_dir/ssh"

# DUT sshd_config: T0 vfs_ram knobs + named hole (UsePAM no, privsep, PermitTTY).
# UsePAM is unsupported without USE_PAM (INFO, ignored; default off).
# UsePrivilegeSeparation is deprecated in 10.5p1 (always on; INFO, ignored).
cat >"$persist_dir/ssh/sshd_config" <<'EOF'
# OpenSSH-portable 10.5p1 DUT (WITH_OPENSSL, SANDBOX_NULL). Packed != Dual DoD B.
Port 22
AddressFamily inet
HostKey /etc/ssh/ssh_host_ed25519_key
PidFile /var/run/sshd.pid
AuthorizedKeysFile .ssh/authorized_keys
PermitRootLogin yes
PubkeyAuthentication yes
PasswordAuthentication no
KbdInteractiveAuthentication no
UsePAM no
UsePrivilegeSeparation yes
PermitTTY yes
PrintMotd no
PrintLastLog no
UseDNS no
AcceptEnv LANG LC_*
EOF
chmod 644 "$persist_dir/ssh/sshd_config"

# Lab-host snippet (not DUT). enable-lab-ssh.sh copies this into sshd_config.d.
cat >"$persist_dir/ssh/sshd_config.snippet" <<'EOF'
# Append or include on lab host (/etc/ssh/sshd_config.d/99-greenjade-hwtest.conf)
Port 22
PermitRootLogin prohibit-password
PasswordAuthentication no
PubkeyAuthentication yes
EOF

auth="$persist_dir/ssh/authorized_keys"
: >"$auth"
printf '%s\n' "$CANON_PUB" >>"$auth"
if [ -f build/hwtest-keys/id_ed25519.pub ]; then
	cp -f build/hwtest-keys/id_ed25519.pub "$persist_dir/ssh/id_ed25519.pub"
	while IFS= read -r line; do
		[ -z "$line" ] && continue
		grep -qxF "$line" "$auth" 2>/dev/null || printf '%s\n' "$line" >>"$auth"
	done <build/hwtest-keys/id_ed25519.pub
else
	printf '%s\n' "$CANON_PUB" >"$persist_dir/ssh/id_ed25519.pub"
fi
chmod 600 "$auth"

# Overlay fragment for init: jay must exist when packed /etc/passwd is applied.
cat >"$persist_dir/ssh/passwd" <<'EOF'
root:x:0:0:root:/root:/bin/sh
sshd:x:74:74:sshd:/var/empty:/usr/sbin/nologin
jay:x:1000:1000:jay:/home/jay:/bin/sh
EOF
chmod 644 "$persist_dir/ssh/passwd"
cat >"$persist_dir/ssh/group" <<'EOF'
root:x:0:
sshd:x:74:
jay:x:1000:
EOF
chmod 644 "$persist_dir/ssh/group"

cat >"$persist_dir/ssh/enable-lab-ssh.sh" <<'EOF'
#!/bin/sh
# Run on the *lab host* Linux (not inside freestanding GreenJade).
# Enables sshd and installs GJ-PERSIST authorized_keys for root.
set -eu
HERE="$(CDPATH= cd -- "$(dirname "$0")" && pwd)"
AUTH="$HERE/authorized_keys"
if [ ! -f "$AUTH" ]; then
	echo "missing $AUTH" >&2
	exit 1
fi
if [ "$(id -u)" -ne 0 ]; then
	echo "run as root: sudo $0" >&2
	exit 1
fi
mkdir -p /root/.ssh
chmod 700 /root/.ssh
# Merge key if not already present
if [ -f /root/.ssh/authorized_keys ]; then
	while IFS= read -r line; do
		[ -z "$line" ] && continue
		grep -qxF "$line" /root/.ssh/authorized_keys 2>/dev/null || \
			echo "$line" >>/root/.ssh/authorized_keys
	done <"$AUTH"
else
	cp -f "$AUTH" /root/.ssh/authorized_keys
fi
chmod 600 /root/.ssh/authorized_keys
if [ -d /etc/ssh/sshd_config.d ]; then
	cp -f "$HERE/sshd_config.snippet" \
		/etc/ssh/sshd_config.d/99-greenjade-hwtest.conf
fi
if command -v systemctl >/dev/null 2>&1; then
	systemctl enable --now sshd 2>/dev/null || systemctl enable --now ssh 2>/dev/null || true
	systemctl reload sshd 2>/dev/null || systemctl reload ssh 2>/dev/null || true
elif command -v service >/dev/null 2>&1; then
	service sshd start 2>/dev/null || service ssh start 2>/dev/null || true
fi
# Ensure sshd is listening
if command -v ss >/dev/null 2>&1; then
	ss -ltn | grep -E ':22\s' || echo "warn: nothing listening on :22 yet" >&2
fi
HOST="$(hostname -f 2>/dev/null || hostname || echo lab-host)"
IP="$(hostname -I 2>/dev/null | awk '{print $1}')"
echo "enable-lab-ssh: PASS"
echo "  ssh -i build/hwtest-keys/id_ed25519 root@${IP:-$HOST}"
echo "  Then attach DUT serial (picocom /dev/ttyUSB0) for GreenJade console"
EOF
chmod +x "$persist_dir/ssh/enable-lab-ssh.sh"

stage_home_keys() {
	base="$1"
	mkdir -p "$base/home/jay/.ssh" "$base/root/.ssh"
	chmod 755 "$base/home" "$base/home/jay" 2>/dev/null || true
	chmod 700 "$base/home/jay/.ssh" "$base/root/.ssh"
	cp -f "$auth" "$base/home/jay/.ssh/authorized_keys"
	cp -f "$auth" "$base/root/.ssh/authorized_keys"
	chmod 600 "$base/home/jay/.ssh/authorized_keys" "$base/root/.ssh/authorized_keys"
}

if [ -n "$rootfs_dir" ] && [ -d "$rootfs_dir" ]; then
	mkdir -p "$rootfs_dir/etc/ssh"
	cp -f "$persist_dir/ssh/sshd_config" "$rootfs_dir/etc/ssh/sshd_config"
	chmod 644 "$rootfs_dir/etc/ssh/sshd_config"
	if [ -f "$rootfs_dir/etc/passwd" ]; then
		grep -q '^jay:' "$rootfs_dir/etc/passwd" 2>/dev/null || \
			printf '%s\n' 'jay:x:1000:1000:jay:/home/jay:/bin/sh' \
				>>"$rootfs_dir/etc/passwd"
	else
		cp -f "$persist_dir/ssh/passwd" "$rootfs_dir/etc/passwd"
		chmod 644 "$rootfs_dir/etc/passwd"
	fi
	if [ -f "$rootfs_dir/etc/group" ]; then
		grep -q '^jay:' "$rootfs_dir/etc/group" 2>/dev/null || \
			printf '%s\n' 'jay:x:1000:' >>"$rootfs_dir/etc/group"
	else
		cp -f "$persist_dir/ssh/group" "$rootfs_dir/etc/group"
		chmod 644 "$rootfs_dir/etc/group"
	fi
	stage_home_keys "$rootfs_dir"
fi

echo "hwtest-sshd-enable: staged persist/ssh DUT sshd_config + jay authorized_keys (packed != Dual DoD B close)"
