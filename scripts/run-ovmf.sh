#!/bin/sh
# SPDX-License-Identifier: MIT
#
# Boot GreenJade.efi under QEMU + OVMF (product firmware path).
# Builds a GPT disk with an EFI System Partition containing BOOTX64.EFI.
# Expect serial: GJ-EFI
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
efi="${1:-$root/build/GreenJade.efi}"

if [ ! -f "$efi" ]; then
    echo "missing $efi — run: make greenjade.efi" >&2
    exit 1
fi

QEMU_BIN="${QEMU_BIN:-}"
if [ -z "$QEMU_BIN" ]; then
    if [ -x /usr/libexec/qemu-kvm ]; then
        QEMU_BIN=/usr/libexec/qemu-kvm
    elif command -v qemu-system-x86_64 >/dev/null 2>&1; then
        QEMU_BIN="$(command -v qemu-system-x86_64)"
    else
        echo "qemu-kvm not found" >&2
        exit 1
    fi
fi

OVMF_CODE="${OVMF_CODE:-}"
OVMF_VARS_SRC="${OVMF_VARS_SRC:-}"
for c in \
    /usr/share/edk2/ovmf/OVMF_CODE.fd \
    /usr/share/OVMF/OVMF_CODE.fd
do
    if [ -z "$OVMF_CODE" ] && [ -f "$c" ]; then
        OVMF_CODE="$c"
    fi
done
for v in \
    /usr/share/edk2/ovmf/OVMF_VARS.fd \
    /usr/share/OVMF/OVMF_VARS.fd
do
    if [ -z "$OVMF_VARS_SRC" ] && [ -f "$v" ]; then
        OVMF_VARS_SRC="$v"
    fi
done
if [ -z "$OVMF_CODE" ] || [ -z "$OVMF_VARS_SRC" ]; then
    echo "OVMF firmware not found (install edk2-ovmf)" >&2
    exit 1
fi

need() {
    command -v "$1" >/dev/null 2>&1 || {
        echo "missing tool: $1" >&2
        exit 1
    }
}
need mformat
need mcopy
need mmd

esp="${TMPDIR:-/tmp}/greenjade-esp.$$.img"
vars="${TMPDIR:-/tmp}/greenjade-ovmf-vars.$$.fd"
cleanup() {
    rm -f "$esp" "$vars" 2>/dev/null || true
}
trap cleanup EXIT INT TERM

cp "$OVMF_VARS_SRC" "$vars"

# 64 MiB GPT disk, partition 1 = ESP (type EF00) starting at LBA 2048
dd if=/dev/zero of="$esp" bs=1M count=64 status=none 2>/dev/null || \
    dd if=/dev/zero of="$esp" bs=1048576 count=64 2>/dev/null
if command -v sgdisk >/dev/null 2>&1; then
    sgdisk -o -n 1:2048:0 -t 1:ef00 -c 1:EFI "$esp" >/dev/null
elif command -v parted >/dev/null 2>&1; then
    parted -s "$esp" mklabel gpt mkpart ESP fat32 1MiB 100% set 1 esp on
else
    echo "need sgdisk or parted for GPT ESP" >&2
    exit 1
fi

START=$((2048 * 512))
mformat -i "$esp@@$START" -F -v GREENJADE ::
mmd -i "$esp@@$START" ::/EFI
mmd -i "$esp@@$START" ::/EFI/BOOT
mmd -i "$esp@@$START" ::/EFI/GREENJADE 2>/dev/null || true
mcopy -o -i "$esp@@$START" "$efi" ::/EFI/BOOT/BOOTX64.EFI
# Stage Multiboot kernel on ESP for product load path (UEFI loader next).
if [ -f "$root/build/greenjade.elf" ]; then
    mcopy -o -i "$esp@@$START" "$root/build/greenjade.elf" \
        ::/EFI/GREENJADE/KERNEL.ELF
    echo "run-ovmf: staged EFI/GREENJADE/KERNEL.ELF on ESP" >&2
fi

echo "run-ovmf: $QEMU_BIN code=$OVMF_CODE efi=$efi" >&2

# shellcheck disable=SC2086
exec "$QEMU_BIN" \
    -machine q35 \
    -cpu max \
    -m 512M \
    -drive if=pflash,format=raw,unit=0,readonly=on,file="$OVMF_CODE" \
    -drive if=pflash,format=raw,unit=1,file="$vars" \
    -drive if=none,id=esp0,file="$esp",format=raw \
    -device virtio-blk-pci,drive=esp0,bootindex=0 \
    -net none \
    -serial stdio \
    -display none \
    -no-reboot \
    -no-shutdown
