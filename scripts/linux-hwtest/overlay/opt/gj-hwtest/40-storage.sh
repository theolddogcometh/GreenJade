#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
set -eu
echo "--- block devices ---"
if command -v lsblk >/dev/null 2>&1; then
	lsblk -o NAME,SIZE,TYPE,FSTYPE,LABEL,UUID,TRAN,MODEL,SERIAL,MOUNTPOINT 2>/dev/null || lsblk
else
	cat /proc/partitions
fi
echo
echo "--- blkid ---"
blkid 2>/dev/null || true
echo
echo "--- nvme list ---"
if command -v nvme >/dev/null 2>&1; then
	nvme list 2>/dev/null || true
else
	ls -la /dev/nvme* 2>/dev/null || echo "(no nvme nodes / nvme-cli)"
fi
echo
echo "--- smartctl (first disk, best-effort) ---"
if command -v smartctl >/dev/null 2>&1; then
	for d in /dev/nvme0 /dev/sda /dev/sdb; do
		[ -b "$d" ] || continue
		echo "smartctl -H $d:"
		smartctl -H "$d" 2>/dev/null | head -20 || true
		break
	done
else
	echo "smartctl not installed"
fi
