#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Fix GRUB on an already-written Linux HWTEST stick or image ESP without
# full rebuild (when firmware drops to grub>).
#
# Usage:
#   sudo ./scripts/linux-hwtest/repair-esp-boot.sh /dev/sdX1
#   sudo ./scripts/linux-hwtest/repair-esp-boot.sh /dev/sdX   # finds ESP
#   sudo ./scripts/linux-hwtest/repair-esp-boot.sh build/linux-hwtest.img
#
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/../.." && pwd)"
target="${1:-}"
if [ -z "$target" ]; then
	echo "usage: $0 /dev/sdX1|/dev/sdX|linux-hwtest.img" >&2
	exit 1
fi
if [ "$(id -u)" -ne 0 ]; then
	echo "need root" >&2
	exit 1
fi

MNT=$(mktemp -d)
LOOP=""
cleanup() {
	umount "$MNT" 2>/dev/null || true
	[ -n "$LOOP" ] && losetup -d "$LOOP" 2>/dev/null || true
	rmdir "$MNT" 2>/dev/null || true
}
trap cleanup EXIT

esp=""
if [ -f "$target" ]; then
	LOOP=$(losetup -f --show -P "$target")
	sleep 0.3
	esp="${LOOP}p1"
elif [ -b "$target" ]; then
	case "$target" in
	*[0-9]|*p[0-9]|*p[0-9][0-9])
		esp=$target
		;;
	*)
		for c in "${target}1" "${target}p1"; do
			[ -b "$c" ] && esp=$c && break
		done
		;;
	esac
fi
if [ -z "$esp" ] || [ ! -b "$esp" ]; then
	echo "cannot find ESP block device from $target" >&2
	exit 1
fi

mount "$esp" "$MNT"
mkdir -p "$MNT/EFI/BOOT" "$MNT/grub" "$MNT/GJ-HWTEST/reports"

# Prefer grub-mkimage from host if available; else keep existing BOOTX64
if command -v grub-mkimage >/dev/null 2>&1; then
	grub-mkimage -O x86_64-efi -o "$MNT/EFI/BOOT/BOOTX64.EFI" -p /EFI/BOOT \
		fat part_gpt part_msdos ext2 linux normal configfile \
		search search_label search_fs_file search_fs_uuid gzio echo ls \
		boot chain reboot halt all_video efi_gop loadenv \
		&& echo "repair: wrote BOOTX64.EFI via host grub-mkimage"
else
	echo "repair: host has no grub-mkimage; keeping existing BOOTX64.EFI"
fi

if [ ! -f "$MNT/vmlinuz-lts" ] || [ ! -f "$MNT/initramfs-lts" ]; then
	echo "repair: WARN missing vmlinuz-lts or initramfs-lts on ESP" >&2
	ls -la "$MNT" >&2 || true
fi

cfg='set default=0
set timeout=3
if search --file --no-floppy --set=root /vmlinuz-lts ; then
  true
elif search --label --no-floppy --set=root GJ-LNX-ESP ; then
  true
fi
menuentry "GreenJade Linux HWTEST (auto report + poweroff)" {
	if search --file --no-floppy --set=root /vmlinuz-lts ; then true ; fi
	linux /vmlinuz-lts root=LABEL=GJ-LNX-ROOT rootfstype=ext4 rw modules=sd-mod,usb-storage,uas,xhci-pci,xhci-hcd,ext4,crc32c rootdelay=10 console=tty0 gj_hwtest=1
	initrd /initramfs-lts
}
menuentry "GreenJade Linux HWTEST (verbose)" {
	if search --file --no-floppy --set=root /vmlinuz-lts ; then true ; fi
	linux /vmlinuz-lts root=LABEL=GJ-LNX-ROOT rootfstype=ext4 rw modules=sd-mod,usb-storage,uas,xhci-pci,xhci-hcd,ext4,crc32c rootdelay=15 console=tty0
	initrd /initramfs-lts
}
'
printf '%s\n' "$cfg" >"$MNT/EFI/BOOT/grub.cfg"
printf '%s\n' "$cfg" >"$MNT/grub/grub.cfg"
sync
echo "repair: PASS $esp"
echo "  has: $(ls "$MNT/EFI/BOOT/BOOTX64.EFI" "$MNT/vmlinuz-lts" "$MNT/initramfs-lts" 2>&1)"
echo "  cfg: $MNT/EFI/BOOT/grub.cfg"
