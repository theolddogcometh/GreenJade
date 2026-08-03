#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
# Soft inventory of GreenJade media labels if present on the same or other disks.
set -eu
echo "--- GreenJade-related labels ---"
if command -v blkid >/dev/null 2>&1; then
	# Product media: GREENJADE / GJ-PERSIST. This image: GJ-LNX-ESP / GJ-LNX-ROOT.
	blkid | grep -iE 'GREENJADE|GJ-PERSIST|GJ-LINUX|GJ-HWTEST|GJ-LNX' || echo "(no GJ labels in blkid)"
fi
echo
echo "--- try mount GJ-PERSIST (read-only inventory) ---"
mnt=/mnt/gj-persist-ro
mkdir -p "$mnt"
if blkid -L GJ-PERSIST >/dev/null 2>&1; then
	dev=$(blkid -L GJ-PERSIST)
	echo "GJ-PERSIST device: $dev"
	if mount -o ro "$dev" "$mnt" 2>/dev/null; then
		echo "contents:"
		ls -la "$mnt" 2>/dev/null | head -40
		if [ -f "$mnt/steam/STATUS" ]; then
			echo "steam STATUS=$(cat "$mnt/steam/STATUS")"
		fi
		umount "$mnt" 2>/dev/null || true
	else
		echo "mount GJ-PERSIST failed (fstype?)"
		blkid "$dev" || true
	fi
else
	echo "label GJ-PERSIST not found"
fi
echo
echo "--- try GREENJADE ESP (read-only) ---"
if blkid -L GREENJADE >/dev/null 2>&1; then
	dev=$(blkid -L GREENJADE)
	echo "GREENJADE device: $dev"
	mkdir -p /mnt/gj-esp-ro
	if mount -o ro "$dev" /mnt/gj-esp-ro 2>/dev/null; then
		ls -la /mnt/gj-esp-ro/EFI/GREENJADE 2>/dev/null | head -30 || true
		if [ -f /mnt/gj-esp-ro/EFI/GREENJADE/BOOT.LOG ]; then
			echo "BOOT.LOG (last 40 lines):"
			tail -40 /mnt/gj-esp-ro/EFI/GREENJADE/BOOT.LOG
		fi
		if [ -f /mnt/gj-esp-ro/EFI/GREENJADE/KLOG.TXT ]; then
			echo "KLOG.TXT size=$(wc -c </mnt/gj-esp-ro/EFI/GREENJADE/KLOG.TXT)"
			# printable strings only
			strings /mnt/gj-esp-ro/EFI/GREENJADE/KLOG.TXT 2>/dev/null | head -40 || true
		fi
		umount /mnt/gj-esp-ro 2>/dev/null || true
	fi
else
	echo "label GREENJADE not found (this image uses GJ-LNX-ESP)"
fi
