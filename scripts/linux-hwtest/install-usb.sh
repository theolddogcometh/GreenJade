#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Write GreenJade Linux HWTEST image to a block device (DESTROYS device).
#
#   sudo ./scripts/linux-hwtest/install-usb.sh /dev/sdX
#
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/../.." && pwd)"
cd "$root"
img="${GJ_LINUX_HWTEST_IMG:-build/linux-hwtest.img}"
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
	echo "missing $img — run: sudo ./scripts/linux-hwtest/build-img.sh" >&2
	exit 1
fi
if [ "$(id -u)" -ne 0 ]; then
	echo "need root: sudo $0 $dev" >&2
	exit 1
fi

case "$dev" in
*/sda|*/nvme0n1|*/vda|*/mmcblk0)
	if [ "${GJ_FORCE_DISK:-0}" != "1" ]; then
		echo "refusing $dev (looks like primary disk). Set GJ_FORCE_DISK=1 to override." >&2
		exit 1
	fi
	;;
esac

img_sz=$(wc -c <"$img" | tr -d ' ')
echo "install-linux-hwtest: image=$img (${img_sz}B)"
echo "install-linux-hwtest: TARGET=$dev (ALL DATA WILL BE ERASED)"

# Refuse empty / no-medium devices (common intermittent USB: SIZE=0, ENOMEDIUM)
base=$(basename "$dev")
sys_name=$base
case "$base" in
nvme*n*|mmcblk*)
	sys_name=$(echo "$base" | sed 's/p[0-9][0-9]*$//')
	;;
sd*[0-9]|vd*[0-9])
	sys_name=$(echo "$base" | sed 's/[0-9][0-9]*$//')
	;;
esac
if [ -f "/sys/block/$sys_name/size" ]; then
	sectors=$(cat "/sys/block/$sys_name/size")
	dev_sz=$((sectors * 512))
	if [ "$dev_sz" -eq 0 ]; then
		echo "install-linux-hwtest: FAIL $dev reports size 0 (No medium / dead link)" >&2
		echo "  lsblk shows SIZE 0B — unplug/replug stick, try another USB3 port," >&2
		echo "  wait 2s, then: udevadm settle; lsblk -o NAME,SIZE,MODEL,TRAN" >&2
		exit 1
	fi
	if [ "$dev_sz" -lt "$img_sz" ]; then
		echo "install-linux-hwtest: FAIL device ${dev_sz}B < image ${img_sz}B" >&2
		exit 1
	fi
	echo "install-linux-hwtest: device size ${dev_sz}B"
fi

# Unmount any partitions on target
if command -v findmnt >/dev/null 2>&1; then
	findmnt -rn -o SOURCE,TARGET 2>/dev/null | while read -r src tgt; do
		case "$src" in
		"$dev"|"$dev"p[0-9]*|"$dev"[0-9]*)
			echo "umount $tgt"
			umount "$tgt" 2>/dev/null || umount -l "$tgt" 2>/dev/null || true
			;;
		esac
	done
fi

echo "5 second abort window (Ctrl-C)..."
sleep 5

dd if="$img" of="$dev" bs=4M status=progress conv=fsync,notrunc
sync
partprobe "$dev" 2>/dev/null || true
udevadm settle --timeout=10 2>/dev/null || true

echo "install-linux-hwtest: partition probe:"
lsblk -o NAME,SIZE,FSTYPE,LABEL,PARTTYPENAME "$dev" 2>/dev/null || true
blkid "${dev}1" "${dev}2" "${dev}p1" "${dev}p2" 2>/dev/null || true

echo "install-linux-hwtest: PASS $img -> $dev"
echo "  Boot DUT UEFI (Secure Boot OFF) → GreenJade Linux HWTEST"
echo "  Wait for auto probes + poweroff"
echo "  Lab host:"
echo "    sudo mount -L GJ-LNX-ESP /mnt/esp"
echo "    cat /mnt/esp/GJ-HWTEST/reports/LATEST.txt"
echo "    cat /mnt/esp/GJ-HWTEST/reports/KEY-FACTS.txt"
echo "    cat /mnt/esp/GJ-HWTEST-RESULT.txt"
echo "    sudo umount /mnt/esp"
