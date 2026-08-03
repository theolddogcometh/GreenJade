#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
# GreenJade Linux hardware test — full loop entrypoint.
# Mounts report area on the boot stick, runs probes, writes report, poweroff.
set -eu

GJ_HWTEST_DIR="${GJ_HWTEST_DIR:-/opt/gj-hwtest}"
GJ_REPORT_LABEL="${GJ_REPORT_LABEL:-GJ-LNX-ESP}"
GJ_REPORT_MNT="${GJ_REPORT_MNT:-/mnt/gj-report}"
GJ_REPORT_SUBDIR="${GJ_REPORT_SUBDIR:-GJ-HWTEST/reports}"
# Seconds to wait for disks/USB after boot (xHCI settle on real DUT)
GJ_SETTLE_SEC="${GJ_SETTLE_SEC:-8}"

log() {
	echo "gj-hwtest: $*"
}

die() {
	echo "gj-hwtest: FAIL: $*" >&2
	# Still try to leave a failure marker if mounted
	if [ -d "$GJ_REPORT_MNT/$GJ_REPORT_SUBDIR" ]; then
		echo "FAIL: $*" >"$GJ_REPORT_MNT/$GJ_REPORT_SUBDIR/LAST-FAIL.txt" || true
		sync || true
	fi
	exit 1
}

find_report_dev() {
	# Prefer labeled ESP from this image
	if command -v blkid >/dev/null 2>&1; then
		blkid -L "$GJ_REPORT_LABEL" 2>/dev/null && return 0
	fi
	# Fallback: first vfat partition with EFI/BOOT
	for p in /dev/disk/by-label/* /dev/sd*[0-9] /dev/nvme*p[0-9] /dev/vd*[0-9]; do
		[ -b "$p" ] || continue
		ft=$(blkid -o value -s TYPE "$p" 2>/dev/null || true)
		[ "$ft" = "vfat" ] || [ "$ft" = "msdos" ] || continue
		echo "$p"
		return 0
	done
	return 1
}

mount_report() {
	mkdir -p "$GJ_REPORT_MNT"
	dev=$(find_report_dev) || die "no report/ESP partition (label $GJ_REPORT_LABEL)"
	log "report device: $dev"
	if ! mount -t vfat -o rw,utf8,flush "$dev" "$GJ_REPORT_MNT" 2>/dev/null; then
		mount -o rw "$dev" "$GJ_REPORT_MNT" || die "mount $dev failed"
	fi
	mkdir -p "$GJ_REPORT_MNT/$GJ_REPORT_SUBDIR"
	# Keep a copy of the runner for host inspection
	cp -a "$GJ_HWTEST_DIR"/*.sh "$GJ_REPORT_MNT/GJ-HWTEST/" 2>/dev/null || true
}

umount_report() {
	sync || true
	umount "$GJ_REPORT_MNT" 2>/dev/null || umount -l "$GJ_REPORT_MNT" 2>/dev/null || true
}

stamp() {
	date -u +%Y%m%dT%H%MZ 2>/dev/null || date +%Y%m%d-%H%M%S
}

# Write greppable KEY-FACTS.txt + minimal SUMMARY.json next to LATEST.txt
write_key_facts() {
	report_path="$1"
	out_dir="$2"
	result_line="$3"
	failures="$4"
	keyf="$out_dir/KEY-FACTS.txt"
	sumj="$out_dir/SUMMARY.json"

	dmi_product=$(cat /sys/class/dmi/id/product_name 2>/dev/null | tr -d '\n' || true)
	dmi_vendor=$(cat /sys/class/dmi/id/sys_vendor 2>/dev/null | tr -d '\n' || true)
	bios_ver=$(cat /sys/class/dmi/id/bios_version 2>/dev/null | tr -d '\n' || true)
	utc=$(date -u -Iseconds 2>/dev/null || date -u)

	# First USB-class PCI xHCI-ish controller BDF + vendor:device
	xhci_bdf=""
	xhci_pci_id=""
	for d in /sys/bus/pci/devices/*; do
		[ -f "$d/class" ] || continue
		cls=$(cat "$d/class" 2>/dev/null || echo "")
		case "$cls" in
		0x0c0330|0x0c03*)
			xhci_bdf=$(basename "$d")
			v=$(cat "$d/vendor" 2>/dev/null || echo "")
			i=$(cat "$d/device" 2>/dev/null || echo "")
			xhci_pci_id="${v#0x}:${i#0x}"
			break
			;;
		esac
	done

	# Prefer SuperSpeed mass-storage stick; fall back to any usb-storage
	stick_sysfs=""
	stick_bus_port=""
	stick_vid_pid=""
	stick_speed=""
	stick_product=""
	for d in /sys/bus/usb/devices/*; do
		[ -e "$d" ] || continue
		base=$(basename "$d")
		case "$base" in
		*:*|usb*) continue ;;
		esac
		[ -d "$d" ] || continue
		# usb-storage or uas bound on an interface under this device?
		found=0
		for iface in "$d"/*:*; do
			[ -e "$iface" ] || continue
			drv=$(basename "$(readlink "$iface/driver" 2>/dev/null || echo "")")
			case "$drv" in
			usb-storage|uas) found=1; break ;;
			esac
		done
		[ "$found" -eq 1 ] || continue
		spd=$(cat "$d/speed" 2>/dev/null || echo "")
		vend=$(cat "$d/idVendor" 2>/dev/null || echo "????")
		prodid=$(cat "$d/idProduct" 2>/dev/null || echo "????")
		prod=$(cat "$d/product" 2>/dev/null | tr -d '\n' || true)
		# Prefer SuperSpeed (5000+)
		case "$spd" in
		5000|10000|20000)
			stick_sysfs=$base
			stick_bus_port=$base
			stick_vid_pid="${vend}:${prodid}"
			stick_speed="SuperSpeed"
			stick_product=$prod
			break
			;;
		*)
			if [ -z "$stick_sysfs" ]; then
				stick_sysfs=$base
				stick_bus_port=$base
				stick_vid_pid="${vend}:${prodid}"
				case "$spd" in
				480) stick_speed="HighSpeed" ;;
				*) stick_speed="${spd}M" ;;
				esac
				stick_product=$prod
			fi
			;;
		esac
	done

	# Human port form: 2-5 → bus2-port5
	stick_bus_port_pretty=""
	if [ -n "$stick_bus_port" ]; then
		b=${stick_bus_port%%-*}
		rest=${stick_bus_port#*-}
		p=${rest%%.*}
		stick_bus_port_pretty="bus${b}-port${p}"
	fi

	# Also pull lspci one-liner if available
	xhci_lspci=""
	if command -v lspci >/dev/null 2>&1; then
		xhci_lspci=$(lspci -nn 2>/dev/null | grep -iE 'USB controller|xhci' | head -1 | tr -s ' ' || true)
	fi

	{
		echo "# GreenJade Linux hwtest KEY-FACTS (greppable)"
		echo "utc=$utc"
		echo "result=$result_line"
		echo "probe_failures=$failures"
		echo "dmi_vendor=$dmi_vendor"
		echo "dmi_product=$dmi_product"
		echo "bios_version=$bios_ver"
		echo "xhci_bdf=$xhci_bdf"
		echo "xhci_pci_id=$xhci_pci_id"
		echo "xhci_lspci=$xhci_lspci"
		echo "stick_sysfs=$stick_sysfs"
		echo "stick_bus_port=$stick_bus_port"
		echo "stick_bus_port_pretty=$stick_bus_port_pretty"
		echo "stick_vid_pid=$stick_vid_pid"
		echo "stick_speed=$stick_speed"
		echo "stick_product=$stick_product"
		echo "report=$(basename "$report_path")"
	} >"$keyf"

	# Minimal JSON (no jq; escape double-quotes in strings lightly)
	json_esc() {
		printf '%s' "$1" | sed 's/\\/\\\\/g; s/"/\\"/g'
	}
	{
		echo "{"
		echo "  \"utc\": \"$(json_esc "$utc")\","
		echo "  \"result\": \"$(json_esc "$result_line")\","
		echo "  \"probe_failures\": $failures,"
		echo "  \"dmi_product\": \"$(json_esc "$dmi_product")\","
		echo "  \"bios_version\": \"$(json_esc "$bios_ver")\","
		echo "  \"xhci_bdf\": \"$(json_esc "$xhci_bdf")\","
		echo "  \"xhci_pci_id\": \"$(json_esc "$xhci_pci_id")\","
		echo "  \"stick_bus_port\": \"$(json_esc "$stick_bus_port")\","
		echo "  \"stick_bus_port_pretty\": \"$(json_esc "$stick_bus_port_pretty")\","
		echo "  \"stick_vid_pid\": \"$(json_esc "$stick_vid_pid")\","
		echo "  \"stick_speed\": \"$(json_esc "$stick_speed")\""
		echo "}"
	} >"$sumj"

	# Also copy to ESP root for one-glance host mount
	cp -f "$keyf" "$GJ_REPORT_MNT/GJ-HWTEST-KEY-FACTS.txt" 2>/dev/null || true
}

main() {
	export PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"

	log "settle ${GJ_SETTLE_SEC}s (USB/PCI)..."
	sleep "$GJ_SETTLE_SEC" || true

	# Ensure devices exist
	if command -v udevadm >/dev/null 2>&1; then
		udevadm settle --timeout=30 2>/dev/null || true
	elif command -v mdev >/dev/null 2>&1; then
		mdev -s 2>/dev/null || true
	fi

	mount_report
	ts=$(stamp)
	host=$(hostname 2>/dev/null || echo unknown)
	report="$GJ_REPORT_MNT/$GJ_REPORT_SUBDIR/report-${host}-${ts}.txt"
	latest="$GJ_REPORT_MNT/$GJ_REPORT_SUBDIR/LATEST.txt"
	status="$GJ_REPORT_MNT/$GJ_REPORT_SUBDIR/STATUS"
	rep_dir="$GJ_REPORT_MNT/$GJ_REPORT_SUBDIR"

	log "writing $report"
	{
		echo "============================================================"
		echo "GreenJade Linux hardware test report"
		echo "============================================================"
		echo "utc:      $(date -u -Iseconds 2>/dev/null || date -u)"
		echo "host:     $host"
		echo "kernel:   $(uname -a)"
		echo "cmdline:  $(cat /proc/cmdline 2>/dev/null || true)"
		echo "script:   $GJ_HWTEST_DIR/run-all.sh"
		echo "============================================================"
		echo
	} >"$report"

	# Ordered probes (each appends; non-zero does not abort the loop)
	rc_total=0
	for s in \
		10-system.sh \
		20-pci.sh \
		30-usb-xhci.sh \
		40-storage.sh \
		50-dmesg.sh \
		60-greenjade-stick.sh \
		70-needed-drivers.sh
	do
		path="$GJ_HWTEST_DIR/$s"
		if [ ! -x "$path" ] && [ -f "$path" ]; then
			chmod +x "$path" || true
		fi
		if [ ! -f "$path" ]; then
			echo "SKIP missing $s" | tee -a "$report"
			continue
		fi
		echo "" | tee -a "$report"
		echo "######## BEGIN $s ########" | tee -a "$report"
		if sh "$path" >>"$report" 2>&1; then
			echo "######## END $s rc=0 ########" | tee -a "$report"
		else
			rc=$?
			echo "######## END $s rc=$rc ########" | tee -a "$report"
			rc_total=$((rc_total + 1))
		fi
		# Standalone collect list for operators (also on ESP root)
		if [ "$s" = "70-needed-drivers.sh" ]; then
			sh "$path" >"$rep_dir/NEEDED-DRIVERS.txt" 2>&1 || true
			cp -f "$rep_dir/NEEDED-DRIVERS.txt" \
				"$GJ_REPORT_MNT/GJ-HWTEST-NEEDED-DRIVERS.txt" 2>/dev/null || true
			log "NEEDED-DRIVERS written (collect for GreenJade install media)"
		fi
	done

	if [ "$rc_total" -eq 0 ]; then
		result_line="PASS"
	else
		result_line="PARTIAL"
	fi

	{
		echo
		echo "============================================================"
		echo "summary: probe_failures=$rc_total"
		if [ "$rc_total" -eq 0 ]; then
			echo "RESULT: PASS (probes completed; not a product claim)"
		else
			echo "RESULT: PARTIAL ($rc_total probe section(s) non-zero)"
		fi
		echo "honesty: Linux hwtest ≠ GreenJade freestanding bar3/Top50"
		echo "============================================================"
	} | tee -a "$report"

	cp -f "$report" "$latest"
	if [ "$rc_total" -eq 0 ]; then
		echo "PASS" >"$status"
	else
		echo "PARTIAL" >"$status"
	fi
	# Human-readable one-liner on ESP root
	echo "gj-hwtest $ts $host failures=$rc_total — see $GJ_REPORT_SUBDIR/" \
		>"$GJ_REPORT_MNT/GJ-HWTEST-RESULT.txt"

	# Greppable short facts (xhci bdf, stick bus/port, …)
	write_key_facts "$report" "$rep_dir" "$result_line" "$rc_total" || true
	log "KEY-FACTS: $(grep -E '^(xhci_|stick_)' "$rep_dir/KEY-FACTS.txt" 2>/dev/null | tr '\n' ' ' || true)"

	sync
	log "report done; unmount + poweroff"
	umount_report

	# Complete loop: shut down the machine
	if command -v poweroff >/dev/null 2>&1; then
		poweroff -f
	elif command -v halt >/dev/null 2>&1; then
		halt -f
	else
		echo o >/proc/sysrq-trigger 2>/dev/null || true
	fi
}

# If anything explodes, still try to poweroff so the loop ends
trap 'log "trap: force poweroff"; umount_report 2>/dev/null; poweroff -f 2>/dev/null || halt -f 2>/dev/null || true' EXIT INT TERM

main "$@"
# poweroff should not return
sleep 5
poweroff -f 2>/dev/null || true
