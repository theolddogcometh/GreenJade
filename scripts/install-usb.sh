#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Write GreenJade GPT ESP image to a block device (USB / disk).
# DESTROYS the entire target device. Requires root for dd.
#
# Prerequisites:
#   make install-img   → build/greenjade-install.img
#     (ESP: BOOTX64.EFI + KERNEL.ELF + user/ + lib/ + INSTALL.txt)
#
# Usage:
#   sudo ./scripts/install-usb.sh /dev/sdX
#   sudo GJ_INSTALL_IMG=build/greenjade-install.img ./scripts/install-usb.sh /dev/nvme0n1
#   GJ_FORCE_DISK=1  — override refusal of primary-looking disks
#
# Post-write (operator):
#   1. Boot firmware into EFI Boot → GreenJade BOOTX64.EFI
#   2. Expect serial: GJ-EFI, KERNEL.ELF loaded, M0 OK
#   3. Soft-scan serial capture:
#        ./scripts/gj-product-summary.sh logs/serial-….txt   # soft exit 0
#        ./scripts/gj-quick-keys.sh logs/serial-….txt        # hard keys; exit 1 on miss
#   4. After bootable: install Steam userspace when ready; fill
#      matrix/deck-top50-*.md (real DUT only — never claim Top-50 from media alone)
#
# For hardware-test media with GJ-PERSIST (logs + ssh + steam), prefer:
#   make hwtest-img && sudo ./scripts/install-hwtest-usb.sh /dev/sdX
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
img="${GJ_INSTALL_IMG:-build/greenjade-install.img}"
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
    echo "missing $img — run: make install-img" >&2
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

# Soft: refuse if target looks smaller than image (best-effort)
img_sz=$(wc -c <"$img" | tr -d ' ')
if [ -b "$dev" ] && [ -r "$dev" ]; then
    # /sys/block/<name>/size is in 512-byte sectors
    base=$(basename "$dev")
    # nvme0n1p1 → strip partition; for whole-disk names keep as-is
    sys_name=$base
    case "$base" in
    nvme*n*p*|mmcblk*p*)
        sys_name=$(echo "$base" | sed 's/p[0-9][0-9]*$//')
        ;;
    *[0-9])
        # sdX1 style — operator should pass whole disk; still soft-check device size
        :
        ;;
    esac
    if [ -f "/sys/block/$sys_name/size" ]; then
        sectors=$(cat "/sys/block/$sys_name/size")
        dev_sz=$((sectors * 512))
        if [ "$dev_sz" -gt 0 ] && [ "$dev_sz" -lt "$img_sz" ]; then
            echo "install-usb: FAIL device ${dev_sz}B < image ${img_sz}B" >&2
            exit 1
        fi
    fi
fi

echo "install-usb: image=$img (${img_sz}B)"
ls -la "$img"
echo "install-usb: TARGET=$dev (ALL DATA WILL BE ERASED)"
echo "install-usb: 5 second abort window (Ctrl-C)..."
sleep 5

dd if="$img" of="$dev" bs=4M status=progress conv=fsync
sync
if command -v partprobe >/dev/null 2>&1; then
    partprobe "$dev" 2>/dev/null || true
fi
echo "install-usb: PASS wrote $img → $dev"
echo "  Next: reboot to EFI, select GreenJade BOOTX64.EFI"
echo "  Serial should show GJ-EFI / KERNEL.ELF / M0 OK"
echo "  Soft summary: ./scripts/gj-product-summary.sh <serial-log>"
echo "  Hard keys:    ./scripts/gj-quick-keys.sh <serial-log>"
echo "  Prefer logs:  make hwtest-img + install-hwtest-usb (GJ-PERSIST)"
