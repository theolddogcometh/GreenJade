#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
set -eu
echo "--- system ---"
echo "uname: $(uname -a)"
echo "uptime: $(cat /proc/uptime 2>/dev/null || true)"
echo
echo "--- cpu ---"
if [ -f /proc/cpuinfo ]; then
	grep -E 'model name|vendor_id|cpu cores|siblings|flags' /proc/cpuinfo | sort -u | head -40
fi
echo
echo "--- memory ---"
free -h 2>/dev/null || cat /proc/meminfo | head -20
echo
echo "--- dmi (if present) ---"
if command -v dmidecode >/dev/null 2>&1; then
	dmidecode -t system 2>/dev/null | head -40 || true
	dmidecode -t bios 2>/dev/null | head -25 || true
else
	echo "dmidecode not installed"
	cat /sys/class/dmi/id/product_name 2>/dev/null || true
	cat /sys/class/dmi/id/sys_vendor 2>/dev/null || true
	cat /sys/class/dmi/id/bios_version 2>/dev/null || true
fi
echo
echo "--- cmdline ---"
cat /proc/cmdline 2>/dev/null || true
echo
echo "--- modules (usb/xhci/pci subset) ---"
lsmod 2>/dev/null | grep -iE 'xhci|usb|ehci|ohci|nvme|ahci|virtio' || true
