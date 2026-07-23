#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Stage a minimal freestanding userspace rootfs tree for product packaging.
# Not a full distro — ELFs + text stubs for install path bring-up.
#
# Usage:
#   ./scripts/stage-rootfs.sh [out_dir]
# Default: build/rootfs
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
out="${1:-build/rootfs}"

make -j"$(nproc)" userland libcgj >/dev/null

rm -rf "$out"
mkdir -p "$out/bin" "$out/sbin" "$out/etc" "$out/tmp" "$out/var/tmp" \
         "$out/usr/bin" "$out/usr/sbin" "$out/usr/lib" "$out/lib" "$out/proc" \
         "$out/dev" "$out/dev/shm" "$out/mnt" "$out/var/log" "$out/etc/ssh"

cp -f build/user/init.elf "$out/sbin/init"
cp -f build/user/shell.elf "$out/bin/sh"
cp -f build/user/shell.elf "$out/bin/greenjade-shell"
# Product shell also as usr/bin for FHS-shaped lookup
cp -f build/user/shell.elf "$out/usr/bin/sh" 2>/dev/null || true
cp -f build/user/sessiond.elf "$out/usr/bin/sessiond" 2>/dev/null || true
cp -f build/user/netstackd.elf "$out/usr/bin/netstackd" 2>/dev/null || true
cp -f build/user/storaged.elf "$out/usr/bin/storaged" 2>/dev/null || true
cp -f build/user/vfsd.elf "$out/usr/bin/vfsd" 2>/dev/null || true
# Product sshd — freestanding ELF, enabled by default on live/hwtest boot
if [ -f build/user/sshd.elf ]; then
	cp -f build/user/sshd.elf "$out/usr/sbin/sshd"
	cp -f build/user/sshd.elf "$out/sbin/sshd"
	mkdir -p "$out/etc/ssh"
	cat >"$out/etc/ssh/sshd_config" <<'SSHEOF'
# GreenJade product sshd (freestanding). Port 22; pubkey preferred.
Port 22
Protocol 2
PermitRootLogin prohibit-password
PasswordAuthentication no
PubkeyAuthentication yes
AuthorizedKeysFile /etc/ssh/authorized_keys
X11Forwarding no
SSHEOF
	if [ -f build/hwtest-keys/id_ed25519.pub ]; then
		cp -f build/hwtest-keys/id_ed25519.pub "$out/etc/ssh/authorized_keys"
	else
		: >"$out/etc/ssh/authorized_keys"
	fi
fi

# Dynlinker scaffold (PT_INTERP target for product Steam/glibc path)
if [ -f build/user/ld-gj.so.1 ]; then
    cp -f build/user/ld-gj.so.1 "$out/lib/ld-gj.so.1"
    cp -f build/user/ld-gj.so.1 "$out/usr/lib/ld-gj.so.1"
    # Classic Linux path alias used by many ET_DYN INTERP strings
    cp -f build/user/ld-gj.so.1 "$out/lib/ld-linux-x86-64.so.2" 2>/dev/null || true
fi
# Clean-room glibc-shaped libc (libcgj) — real ELF soname libc.so.6 (not GNU glibc)
if [ -f build/user/libc.so.6 ]; then
    cp -f build/user/libc.so.6 "$out/lib/libc.so.6"
    cp -f build/user/libc.so.6 "$out/usr/lib/libc.so.6"
else
    cat >"$out/lib/libc.so.6" <<'EOF'
# GreenJade libc placeholder — build make libcgj for real ELF
EOF
    cp -f "$out/lib/libc.so.6" "$out/usr/lib/libc.so.6"
fi
# Product DT_NEEDED SOs (real ELF — SysV hash + GNU hash)
if [ -f build/user/libgj-so.so.1 ]; then
    cp -f build/user/libgj-so.so.1 "$out/lib/libgj-so.so.1"
    cp -f build/user/libgj-so.so.1 "$out/usr/lib/libgj-so.so.1"
else
    echo "FAIL: missing build/user/libgj-so.so.1 (make userland)" >&2
    exit 1
fi
if [ -f build/user/libgj-gnu.so.1 ]; then
    cp -f build/user/libgj-gnu.so.1 "$out/lib/libgj-gnu.so.1"
    cp -f build/user/libgj-gnu.so.1 "$out/usr/lib/libgj-gnu.so.1"
else
    echo "FAIL: missing build/user/libgj-gnu.so.1 (make userland)" >&2
    exit 1
fi

cat >"$out/etc/os-release" <<'EOF'
NAME="GreenJade"
ID=greenjade
VERSION_ID="0.1"
PRETTY_NAME="GreenJade 0.1"
HOME_URL="https://greenjade.local"
EOF

cat >"$out/etc/hostname" <<'EOF'
greenjade
EOF

cat >"$out/etc/passwd" <<'EOF'
root:x:0:0:root:/root:/bin/sh
EOF

cat >"$out/etc/issue" <<'EOF'
GreenJade \n \l
EOF

: >"$out/tmp/.keep"
: >"$out/var/tmp/.keep"

# Steam tree (option 2): prebuilt payload under /opt/steam when lab host fetched it
chmod +x scripts/stage-steam-tree.sh
GJ_STEAM_AS_ROOTFS=1 ./scripts/stage-steam-tree.sh "$out" >/dev/null || true

cat >"$out/README.txt" <<'EOF'
GreenJade minimal rootfs (freestanding ELFs)

sbin/init              — first process (embedded in kernel for Multiboot smoke)
bin/sh                 — shell scaffold
lib/ld-gj.so.1         — dynamic linker (PT_INTERP; also ld-linux-x86-64.so.2)
lib/libgj-so.so.1      — SysV-hash product SO (gj_so_export)
lib/libgj-gnu.so.1     — GNU-hash product SO (gj_gnu_export)
lib/libc.so.6          — clean-room libcgj (glibc-shaped; see docs/GLIBC_COMPAT.md)
usr/bin/sessiond       — compositor server ELF
usr/bin/netstackd      — net server ELF
usr/sbin/sshd          — product SSH daemon (port 22, on by default)
usr/bin/storaged       — storage server ELF
usr/bin/vfsd           — block-backed VFS server (store door + named cache)
etc/ssh/sshd_config    — freestanding sshd config
etc/ssh/authorized_keys — hwtest ed25519 pubkey when present
opt/steam/             — staged Steam bootstrap (option 2; make steam-fetch)
usr/bin/steam          — thin launcher → /opt/steam or GJ-PERSIST/steam

Product: Steam via prebuilt tree on media (docs/STEAM_HWTEST.md); in-tree libc is libcgj.
EOF

n=$(find "$out" -type f | wc -l | tr -d ' ')
sz=$(du -sk "$out" | awk '{print $1}')
echo "stage-rootfs: PASS files=$n size_kb=$sz path=$out"
