#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Write GreenJade hardware-test GPT image (ESP + GJ-PERSIST) to a block device.
# DESTROYS the entire target device. Requires root for dd.
#
# Layout written (from make hwtest-img):
#   p1  EF00 ESP (FAT32, GREENJADE)  — BOOTX64.EFI + KERNEL.ELF + rootfs/user
#                                     (FAT required by UEFI firmware)
#   p2  8300 Linux (ext4, GJ-PERSIST) — durable logs/ + ssh/ + steam/
#                                     (ext4 default for Linux/Unix lab)
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
#   2. Logs:  sudo mount -t ext4 -L GJ-PERSIST /mnt/gj-persist  →  logs/
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
*/sda|*/nvme0n1|*/vda|*/mmcblk0)
	if [ "${GJ_FORCE_DISK:-0}" != "1" ]; then
		echo "refusing $dev (looks like primary disk). Set GJ_FORCE_DISK=1 to override." >&2
		exit 1
	fi
	;;
esac

img_sz=$(wc -c <"$img" | tr -d ' ')
# Soft: refuse if target looks smaller than image (best-effort)
base=$(basename "$dev")
sys_name=$base
case "$base" in
nvme*n*p*|mmcblk*p*)
	sys_name=$(echo "$base" | sed 's/p[0-9][0-9]*$//')
	;;
esac
if [ -f "/sys/block/$sys_name/size" ]; then
	sectors=$(cat "/sys/block/$sys_name/size")
	dev_sz=$((sectors * 512))
	if [ "$dev_sz" -gt 0 ] && [ "$dev_sz" -lt "$img_sz" ]; then
		echo "install-hwtest-usb: FAIL device ${dev_sz}B < image ${img_sz}B" >&2
		exit 1
	fi
fi

echo "install-hwtest-usb: image=$img (${img_sz}B)"
ls -la "$img"
echo "install-hwtest-usb: TARGET=$dev (ALL DATA WILL BE ERASED)"

# Must unmount before dd: writing while automount holds old vfat leaves a live
# wrong FS in the kernel (I/O errors, missing steam/STATUS, lsblk still "vfat").
unmount_target() {
	_d="$1"
	if command -v findmnt >/dev/null 2>&1; then
		# Collect targets first (umount changes findmnt output while iterating)
		_tgts=$(findmnt -rn -o SOURCE,TARGET 2>/dev/null | while read -r src tgt; do
			case "$src" in
			"$_d"|"$_d"p[0-9]*|"$_d"[0-9]*)
				printf '%s\n' "$tgt"
				;;
			esac
		done)
		for tgt in $_tgts; do
			[ -n "$tgt" ] || continue
			echo "install-hwtest-usb: umount $tgt"
			umount "$tgt" 2>/dev/null || umount -l "$tgt" 2>/dev/null || true
		done
	fi
	for p in "$_d"?* "$_d"[0-9]* "$_d"p[0-9]*; do
		[ -b "$p" ] || continue
		if findmnt -n "$p" >/dev/null 2>&1; then
			echo "install-hwtest-usb: umount $p"
			umount "$p" 2>/dev/null || umount -l "$p" 2>/dev/null || true
		fi
	done
	if command -v blockdev >/dev/null 2>&1; then
		blockdev --flushbufs "$_d" 2>/dev/null || true
	fi
}
unmount_target "$dev"

if findmnt -rn -o SOURCE 2>/dev/null | grep -E "^${dev}(p?[0-9]+)?\$" >/dev/null 2>&1; then
	echo "install-hwtest-usb: FAIL $dev still has mounts — umount and retry:" >&2
	findmnt -rn -o SOURCE,TARGET 2>/dev/null | grep -E "^${dev}" >&2 || true
	exit 1
fi

echo "install-hwtest-usb: 5 second abort window (Ctrl-C)..."
sleep 5

dd if="$img" of="$dev" bs=4M status=progress conv=fsync,notrunc
sync

if command -v partprobe >/dev/null 2>&1; then
	partprobe "$dev" 2>/dev/null || true
fi
if command -v udevadm >/dev/null 2>&1; then
	udevadm settle --timeout=10 2>/dev/null || true
fi
# Invalidate stale blkid cache (old TYPE=vfat for p2)
if command -v blkid >/dev/null 2>&1; then
	blkid -g 2>/dev/null || true
	for p in "$dev"?* "$dev"[0-9]* "$dev"p[0-9]*; do
		[ -b "$p" ] || continue
		blkid -p -o udev "$p" >/dev/null 2>&1 || true
	done
fi

echo "install-hwtest-usb: partition probe:"
if command -v lsblk >/dev/null 2>&1; then
	lsblk -o NAME,SIZE,FSTYPE,LABEL,PARTTYPENAME "$dev" 2>/dev/null || true
fi
if command -v blkid >/dev/null 2>&1; then
	blkid "${dev}1" "${dev}2" "${dev}p1" "${dev}p2" 2>/dev/null || \
		blkid | grep -E "$(basename "$dev")" || true
fi

p2=""
for cand in "${dev}2" "${dev}p2"; do
	[ -b "$cand" ] && p2=$cand && break
done
if [ -n "$p2" ] && command -v blkid >/dev/null 2>&1; then
	p2_type=$(blkid -o value -s TYPE "$p2" 2>/dev/null || true)
	p2_label=$(blkid -o value -s LABEL "$p2" 2>/dev/null || true)
	echo "install-hwtest-usb: p2 $p2 TYPE=${p2_type:-?} LABEL=${p2_label:-?}"
	case "$p2_type" in
	ext4|ext3|ext2)
		echo "install-hwtest-usb: p2 filesystem OK (ext*, Linux lab default)"
		;;
	vfat|msdos)
		echo "install-hwtest-usb: warn: p2 is FAT (expected ext4 unless GJ_HWTEST_PERSIST_FS=fat32)" >&2
		;;
	*)
		echo "install-hwtest-usb: warn: unexpected p2 TYPE=${p2_type:-unknown}" >&2
		;;
	esac
fi

echo "install-hwtest-usb: PASS wrote $img -> $dev"
echo "  Boot:  UEFI -> GreenJade BOOTX64.EFI (serial: GJ-EFI / M0 OK)"
echo "  Logs:  umount any automount first, then:"
echo "         sudo umount /run/media/\$USER/GJ-PERSIST 2>/dev/null; true"
echo "         sudo mount -t ext4 -L GJ-PERSIST /mnt/gj-persist"
echo "         cat /mnt/gj-persist/steam/STATUS   # expect READY"
echo "  SSH:   sudo bash /mnt/gj-persist/ssh/enable-lab-ssh.sh"
echo "  Soft:  ./scripts/gj-product-summary.sh <serial-log>   # exit 0 always"
echo "  Keys:  ./scripts/gj-quick-keys.sh <serial-log>        # hard miss exit 1"
echo "  Steam: if STATUS=SKELETON: sudo make steam-to-persist"
echo "  Note:  READY/media != Steam client; Top-50 stays NOT-TRIED until DUT run"
