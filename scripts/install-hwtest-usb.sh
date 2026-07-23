#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Write GreenJade hardware-test GPT image (ESP + GJ-PERSIST) to a block device.
# DESTROYS the entire target device. Requires root for dd.
#
# Layout written (from make hwtest-img):
#   p1  EF00 ESP (FAT32, GREENJADE)  — BOOTX64.EFI + KERNEL.ELF + rootfs/user
#   p2  0700 data (FAT32, GJ-PERSIST) — durable logs/ + ssh/ + steam/
#
# Prerequisites:
#   make hwtest-img              → build/greenjade-hwtest.img
#   optional: make steam-fetch && make steam-stage   # STATUS=READY on p2
#
# Usage:
#   sudo ./scripts/install-hwtest-usb.sh /dev/sdX
#   sudo GJ_HWTEST_IMG=build/greenjade-hwtest.img ./scripts/install-hwtest-usb.sh /dev/nvme0n1
#   GJ_FORCE_DISK=1  — override refusal of primary-looking disks (sda/nvme0n1/vda)
#
# Post-write (lab host + DUT):
#   1. Boot DUT UEFI → GreenJade BOOTX64.EFI  (serial: GJ-EFI / M0 OK)
#   2. Logs:  sudo mount -L GJ-PERSIST /mnt/gj-persist  →  logs/
#   3. Lab SSH (serial bridge): sudo bash /mnt/gj-persist/ssh/enable-lab-ssh.sh
#      or: sudo ./scripts/hwtest-ssh-setup.sh
#   4. Soft-scan serial: ./scripts/gj-product-summary.sh /mnt/gj-persist/logs/….txt
#      Hard keys only:   ./scripts/gj-quick-keys.sh <serial-log>   # exit 1 on miss
#   5. Steam bar3 media: ./scripts/steam-bar3-check.sh   # soft exit 0
#   6. If steam/STATUS=SKELETON: sudo make steam-to-persist  (option 3 host prep)
#
# See: docs/STEAM_HWTEST.md
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
img="${GJ_HWTEST_IMG:-build/greenjade-hwtest.img}"
dev="${1:-}"

if [ -z "$dev" ]; then
	echo "usage: $0 /dev/sdX   # wipes entire device" >&2
	exit 1
fi
if [ ! -b "$dev" ]; then
	echo "not a block device: $dev" >&2
	exit 1
fi
if [ ! -f "$img" ]; then
	echo "missing $img — run: make hwtest-img" >&2
	exit 1
fi

# Refuse obvious system disks when possible
case "$dev" in
*/sda|*/nvme0n1|*/vda)
	if [ "${GJ_FORCE_DISK:-0}" != "1" ]; then
		echo "refusing $dev (looks like primary disk). Set GJ_FORCE_DISK=1 to override." >&2
		exit 1
	fi
	;;
esac

echo "install-hwtest-usb: image=$img"
ls -la "$img"
echo "install-hwtest-usb: TARGET=$dev (ALL DATA WILL BE ERASED)"
echo "install-hwtest-usb: 5 second abort window (Ctrl-C)..."
sleep 5

dd if="$img" of="$dev" bs=4M status=progress conv=fsync
sync
# Re-read partition table
if command -v partprobe >/dev/null 2>&1; then
	partprobe "$dev" 2>/dev/null || true
fi
echo "install-hwtest-usb: PASS wrote $img → $dev"
echo "  Boot:  UEFI → GreenJade BOOTX64.EFI (serial: GJ-EFI / M0 OK)"
echo "  Logs:  mount -L GJ-PERSIST  →  logs/"
echo "  SSH:   on lab host: sudo bash /mnt/gj-persist/ssh/enable-lab-ssh.sh"
echo "  Soft:  ./scripts/gj-product-summary.sh <serial-log>   # exit 0 always"
echo "  Keys:  ./scripts/gj-quick-keys.sh <serial-log>        # hard miss exit 1"
echo "  Steam: if STATUS=SKELETON, host prep (option 3):"
echo "         sudo make steam-to-persist   # or steam-host-prep --to-label"
echo "         docs/STEAM_HWTEST.md"
