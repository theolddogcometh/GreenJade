#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Build a hybrid live ISO for hardware / QEMU testing:
#   - Multiboot2 GreenJade (GRUB) for QEMU -cdrom
#   - UEFI El Torito / EFI files for real-hw optical or dd-style hybrid boot
#   - Staged rootfs + HWTEST docs on the ISO filesystem
#
# Logs on real USB media use the companion GPT image (make hwtest-img):
#   partition 2 labeled GJ-PERSIST — see scripts/make-hwtest-img.sh
#
# Usage:
#   ./scripts/make-live-iso.sh [out.iso]
# Default: build/greenjade-live.iso
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
out="${1:-build/greenjade-live.iso}"
stage="${TMPDIR:-/tmp}/gj-live-iso.$$"

need() {
	command -v "$1" >/dev/null 2>&1 || {
		echo "missing tool: $1" >&2
		exit 1
	}
}
need dd
MKRESCUE="$(command -v grub2-mkrescue 2>/dev/null || command -v grub-mkrescue || true)"
if [ -z "$MKRESCUE" ]; then
	echo "missing grub2-mkrescue (package grub2-tools-extra)" >&2
	exit 1
fi

echo "make-live-iso: staging ESP + rootfs..."
chmod +x scripts/stage-esp.sh scripts/stage-rootfs.sh
./scripts/stage-esp.sh build/esp >/dev/null
./scripts/stage-rootfs.sh build/rootfs >/dev/null

rm -rf "$stage"
mkdir -p "$stage/boot/grub" \
	"$stage/EFI/BOOT" \
	"$stage/EFI/GREENJADE" \
	"$stage/live" \
	"$stage/live/rootfs" \
	"$stage/live/ssh" \
	"$stage/live/logs"

# Multiboot kernel
cp -f build/greenjade.elf "$stage/boot/greenjade.elf"
cp -f build/greenjade.elf "$stage/EFI/GREENJADE/KERNEL.ELF"
cp -f build/GreenJade.efi "$stage/EFI/BOOT/BOOTX64.EFI"
# Full product ESP tree (user ELFs, libcgj)
cp -a build/esp/EFI/GREENJADE/. "$stage/EFI/GREENJADE/" 2>/dev/null || true
# Rootfs snapshot on the ISO (read-only live content)
cp -a build/rootfs/. "$stage/live/rootfs/" 2>/dev/null || true

cat >"$stage/boot/grub/grub.cfg" <<'EOF'
set timeout=5
set default=0

menuentry "GreenJade live (Multiboot2 hardware test)" {
	multiboot2 /boot/greenjade.elf
	boot
}

menuentry "GreenJade live (serial console hint)" {
	echo "sshd on by default (port 22); serial: QEMU -serial stdio (see /live/README-HWTEST.txt)."
	multiboot2 /boot/greenjade.elf
	boot
}
EOF

# SSH materials for lab-host enable script (Grok remote debug)
mkdir -p build/hwtest-keys
if [ ! -f build/hwtest-keys/id_ed25519 ]; then
	if command -v ssh-keygen >/dev/null 2>&1; then
		ssh-keygen -t ed25519 -N "" -C "greenjade-hwtest@lab" \
			-f build/hwtest-keys/id_ed25519 >/dev/null
		echo "make-live-iso: generated build/hwtest-keys/id_ed25519"
	fi
fi
if [ -f build/hwtest-keys/id_ed25519.pub ]; then
	cp -f build/hwtest-keys/id_ed25519.pub "$stage/live/ssh/authorized_keys"
	cp -f build/hwtest-keys/id_ed25519.pub "$stage/live/ssh/id_ed25519.pub"
fi

cat >"$stage/live/ssh/sshd_config.template" <<'EOF'
# GreenJade product sshd (freestanding + lab-host template)
Port 22
Protocol 2
PermitRootLogin prohibit-password
PasswordAuthentication no
PubkeyAuthentication yes
AuthorizedKeysFile .ssh/authorized_keys /live/ssh/authorized_keys /etc/ssh/authorized_keys
X11Forwarding no
AllowTcpForwarding yes
ClientAliveInterval 30
EOF

cat >"$stage/live/README-HWTEST.txt" <<'EOF'
GreenJade — hardware-test live ISO
==================================

This ISO boots the GreenJade kernel (Multiboot2 via GRUB; UEFI BOOTX64.EFI).
Product sshd is **on by default** (embedded sshd.elf, port 22 / net door).

Persistent logs (USB)
---------------------
The ISO filesystem is read-only. For logs that survive reboot, use the
companion GPT image instead of (or in addition to) the ISO:

  make hwtest-img
  sudo ./scripts/install-hwtest-usb.sh /dev/sdX

That image has:
  p1  EFI System Partition  — GreenJade boot + product tree
  p2  FAT32 label GJ-PERSIST — logs/, journal/, ssh/

After a test run, remount GJ-PERSIST on a lab host and collect:
  logs/serial-*.txt
  logs/dmesg-*.txt
  logs/hwtest-*.log

SSH (remote debug for operators / Grok)
---------------------------------------
Product freestanding sshd starts at boot (listen + banner/KEX soft path on
the net door). Full virtio-net TCP + crypto KEX is staged next.

Until eth-TCP KEX is complete, bench remote debug also uses the **lab host**
(the machine with serial to the DUT):

  # On the lab Linux host (once):
  sudo ./scripts/hwtest-ssh-setup.sh

  # From your workstation:
  ssh -i build/hwtest-keys/id_ed25519 root@<lab-host>

Then attach serial to the DUT, e.g.:
  picocom -b 115200 /dev/ttyUSB0
  # or:  make ovmf / scripts/run-qemu.sh for virtual DUT

Host smoke (no QEMU):
  make sshd && ./build/sshd --once

QEMU quick test of this ISO
---------------------------
  qemu-system-x86_64 -cdrom build/greenjade-live.iso -m 2G -serial stdio \
    -device virtio-net-pci,netdev=n0 -netdev user,id=n0,hostfwd=tcp::2222-:22

Policy: pure C product, dual MIT OR Apache-2.0; BSD third-party OK; no GPL.
EOF

cat >"$stage/live/logs/README.txt" <<'EOF'
ISO /live/logs is not writable at runtime.
Write durable logs to the GJ-PERSIST partition (make hwtest-img).
EOF

cat >"$stage/EFI/GREENJADE/HWTEST.txt" <<'EOF'
GreenJade hardware-test media (ISO + USB)

1. Prefer: make hwtest-img && sudo ./scripts/install-hwtest-usb.sh /dev/sdX
2. Boot UEFI → BOOTX64.EFI → expect GJ-EFI / M0 OK on serial
3. Logs → second partition label GJ-PERSIST (/logs on host mount)
4. Remote: sudo ./scripts/hwtest-ssh-setup.sh on lab host; SSH with build/hwtest-keys/
EOF

echo "make-live-iso: packing $out ..."
"$MKRESCUE" -o "$out" "$stage" >/dev/null 2>&1 || \
	"$MKRESCUE" -o "$out" "$stage"

rm -rf "$stage"
sz=$(wc -c <"$out" | tr -d ' ')
echo "make-live-iso: PASS iso=$out size=${sz}B"
echo "  QEMU:  qemu-system-x86_64 -cdrom $out -m 2G -serial stdio"
echo "  USB:   prefer make hwtest-img (ESP + GJ-PERSIST for logs)"
echo "  SSH:   product sshd on by default; lab host: sudo ./scripts/hwtest-ssh-setup.sh"
