#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Write GreenJade GPT ESP image to a block device (USB / disk).
# DESTROYS the entire target device. Requires root for dd.
#
# Prerequisites:
#   make install-img   → build/greenjade-install.img  (ESP: BOOTX64.EFI + KERNEL.ELF)
#
# Usage:
#   sudo ./scripts/install-usb.sh /dev/sdX
#   sudo GJ_INSTALL_IMG=build/greenjade-install.img ./scripts/install-usb.sh /dev/nvme0n1
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

echo "install-usb: image=$img"
ls -la "$img"
echo "install-usb: TARGET=$dev (ALL DATA WILL BE ERASED)"
echo "install-usb: 5 second abort window (Ctrl-C)..."
sleep 5

dd if="$img" of="$dev" bs=4M status=progress conv=fsync
sync
echo "install-usb: PASS wrote $img → $dev"
echo "  Next: reboot to EFI, select GreenJade BOOTX64.EFI"
echo "  Serial should show GJ-EFI / KERNEL.ELF / M0 OK"
echo "  Soft summary: ./scripts/gj-product-summary.sh <serial-log>"
echo "  Hard keys:    ./scripts/gj-quick-keys.sh <serial-log>"
