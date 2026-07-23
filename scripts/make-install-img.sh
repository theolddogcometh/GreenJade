#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Build a GPT disk image with ESP suitable for:
#   - QEMU: qemu-system-x86_64 -drive file=build/greenjade-install.img,...
#   - Real hardware: dd if=build/greenjade-install.img of=/dev/sdX bs=4M status=progress
#
# Layout matches scripts/stage-esp.sh (BOOTX64.EFI + KERNEL.ELF + INSTALL.txt).
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
out="${1:-build/greenjade-install.img}"
esp_dir="${TMPDIR:-/tmp}/gj-esp-stage.$$"

need() {
    command -v "$1" >/dev/null 2>&1 || {
        echo "missing tool: $1" >&2
        exit 1
    }
}
need mformat
need mcopy
need mmd
need dd

./scripts/stage-esp.sh "$esp_dir" >/dev/null

# 128 MiB GPT + ESP
dd if=/dev/zero of="$out" bs=1M count=128 status=none 2>/dev/null || \
    dd if=/dev/zero of="$out" bs=1048576 count=128 2>/dev/null

if command -v sgdisk >/dev/null 2>&1; then
    sgdisk -o -n 1:2048:0 -t 1:ef00 -c 1:EFI "$out" >/dev/null
elif command -v parted >/dev/null 2>&1; then
    parted -s "$out" mklabel gpt mkpart ESP fat32 1MiB 100% set 1 esp on
else
    echo "need sgdisk or parted" >&2
    exit 1
fi

START=1048576
mformat -i "$out@@$START" -F -v GREENJADE ::
mmd -i "$out@@$START" ::/EFI
mmd -i "$out@@$START" ::/EFI/BOOT
mmd -i "$out@@$START" ::/EFI/GREENJADE
mcopy -o -i "$out@@$START" "$esp_dir/EFI/BOOT/BOOTX64.EFI" ::/EFI/BOOT/BOOTX64.EFI
mcopy -o -i "$out@@$START" "$esp_dir/EFI/GREENJADE/KERNEL.ELF" ::/EFI/GREENJADE/KERNEL.ELF
mcopy -o -i "$out@@$START" "$esp_dir/EFI/GREENJADE/INSTALL.txt" ::/EFI/GREENJADE/INSTALL.txt

rm -rf "$esp_dir"
sz=$(wc -c <"$out" | tr -d ' ')
echo "make-install-img: PASS img=$out size=${sz}B"
echo "  QEMU:  make ovmf  # or -drive file=$out,format=raw"
echo "  Real:  dd if=$out of=/dev/sdX bs=4M status=progress  # wipe disk!"
