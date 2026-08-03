#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
set -eu
echo "--- pci (lspci) ---"
if command -v lspci >/dev/null 2>&1; then
	lspci -nn 2>/dev/null || lspci 2>/dev/null
	echo
	echo "--- pci USB controllers ---"
	lspci -nn | grep -iE 'usb|xhci|ehci|ohci|uhci' || echo "(none grepped)"
	echo
	echo "--- pci storage ---"
	lspci -nn | grep -iE 'nvm|sata|ahci|raid|scsi|non-volatile' || echo "(none grepped)"
	echo
	echo "--- pci network/display (soft inventory) ---"
	lspci -nn | grep -iE 'network|ethernet|vga|3d|display|audio' || true
else
	echo "lspci missing; sysfs walk"
	for d in /sys/bus/pci/devices/*; do
		[ -d "$d" ] || continue
		echo "$(basename "$d") vendor=$(cat "$d/vendor" 2>/dev/null) device=$(cat "$d/device" 2>/dev/null) class=$(cat "$d/class" 2>/dev/null)"
	done | head -80
fi
