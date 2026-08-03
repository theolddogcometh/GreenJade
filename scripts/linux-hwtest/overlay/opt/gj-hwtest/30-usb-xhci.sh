#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
# USB / xHCI inventory — especially useful on USB3-only chassis (e.g. G752VT).
set -eu
echo "--- usb tree ---"
if command -v lsusb >/dev/null 2>&1; then
	lsusb 2>/dev/null || true
	echo
	echo "--- lsusb -t ---"
	lsusb -t 2>/dev/null || true
else
	echo "lsusb missing"
fi
echo
echo "--- usb ports (bus-port, speed, SuperSpeed vs HS/FS) ---"
# sysfs: /sys/bus/usb/devices/N-M  → bus N, port path M; speed in Mbit/s
# Root hubs are usb1/usb2; real devices are like 1-4, 2-5, 1-8.1, …
for d in /sys/bus/usb/devices/*; do
	[ -e "$d" ] || continue
	base=$(basename "$d")
	# skip interfaces (e.g. 2-5:1.0) and usbN root hub nodes here
	case "$base" in
	*:*) continue ;;
	usb[0-9]*) continue ;;
	esac
	# only topology nodes with a busnum + devnum
	[ -f "$d/busnum" ] && [ -f "$d/devnum" ] || continue
	bus=$(cat "$d/busnum" 2>/dev/null || echo "?")
	devn=$(cat "$d/devnum" 2>/dev/null || echo "?")
	spd=$(cat "$d/speed" 2>/dev/null || echo "?")
	prod=$(cat "$d/product" 2>/dev/null || echo "")
	vend=$(cat "$d/idVendor" 2>/dev/null || echo "????")
	prodid=$(cat "$d/idProduct" 2>/dev/null || echo "????")
	# port number = last segment of topology id (e.g. 2-5 → port 5; 1-8.1 → port 1 under hub)
	port=${base##*-}
	port=${port%%.*}
	# classify by USB speed (Mbit/s as reported by kernel)
	case "$spd" in
	5000|10000|20000) tier="SuperSpeed" ;;
	480)              tier="HighSpeed" ;;
	12|1.5)           tier="Full/LowSpeed" ;;
	*)                tier="speed=${spd}M" ;;
	esac
	echo "bus${bus}-port${port}  sysfs=${base}  dev=${devn}  ${tier}(${spd}M)  ${vend}:${prodid}  ${prod}"
done
echo
echo "--- sysfs usb hosts (root hubs) ---"
for h in /sys/bus/usb/devices/usb*; do
	[ -e "$h" ] || continue
	base=$(basename "$h")
	bus=$(cat "$h/busnum" 2>/dev/null || echo "?")
	spd=$(cat "$h/speed" 2>/dev/null || echo "?")
	case "$spd" in
	5000|10000|20000) tier="SuperSpeed" ;;
	480)              tier="HighSpeed" ;;
	*)                tier="speed=${spd}M" ;;
	esac
	nports=$(cat "$h/maxchild" 2>/dev/null || echo "?")
	echo "$base bus=${bus} ${tier}(${spd}M) ports=${nports} product=$(cat "$h/product" 2>/dev/null || true)"
done
echo
echo "--- xhci pci devices ---"
for d in /sys/bus/pci/devices/*; do
	[ -f "$d/class" ] || continue
	cls=$(cat "$d/class" 2>/dev/null || echo "")
	# 0x0c0330 = serial bus / USB / xHCI
	case "$cls" in
	0x0c03*)
		bdf=$(basename "$d")
		drv=$(basename "$(readlink "$d/driver" 2>/dev/null || echo none)")
		vendor=$(cat "$d/vendor" 2>/dev/null || echo "")
		device=$(cat "$d/device" 2>/dev/null || echo "")
		# strip 0x for greppable id
		vid=${vendor#0x}
		did=${device#0x}
		echo "$bdf class=$cls driver=$drv pci_id=${vid}:${did}"
		;;
	esac
done
echo
echo "--- usb storage / disk by-id (usb) ---"
ls -la /dev/disk/by-id 2>/dev/null | grep -i usb || echo "(no usb by-id links)"
echo
echo "--- dmesg usb/xhci (tail) ---"
if command -v dmesg >/dev/null 2>&1; then
	dmesg 2>/dev/null | grep -iE 'xhci|usb [0-9]|new .*USB|Mass Storage|SuperSpeed' | tail -80 || true
else
	echo "dmesg unavailable"
fi
