#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
set -eu
echo "--- dmesg (last 200 lines) ---"
if command -v dmesg >/dev/null 2>&1; then
	dmesg 2>/dev/null | tail -200
else
	echo "dmesg unavailable"
fi
echo
echo "--- dmesg errors/warn (sample) ---"
if command -v dmesg >/dev/null 2>&1; then
	dmesg 2>/dev/null | grep -iE 'error|fail|warn|oops|bug|usb|xhci|iommu|nvme|ahci' | tail -100 || true
fi
