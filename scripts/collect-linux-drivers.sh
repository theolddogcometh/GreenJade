#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Collect Linux kernel modules + firmware from the *lab host* into a staging
# tree for greenjade-hwtest.img (GJ-PERSIST + ESP notes).
#
# Purpose (ABI-first / operator direction 2026-08):
#   It is unrealistic to reimplement every class driver in freestanding C.
#   Operators collect the Linux modules their hardware needs; GreenJade
#   focuses on Linux ABI + DDI so those drivers can be hosted later.
#
# Honesty:
#   - Staged .ko files are **host Linux** binaries (often GPL). They are
#     **not** GreenJade dual-license product code and are **not** linked into
#     KERNEL.ELF by default. Product AC remains: no GPL *source* in the
#     GreenJade tree and no claim that shipping .ko alone is bar3/T1 product.
#   - GreenJade does **not** yet load .ko at boot (module loader OPEN).
#     This tree is inventory + staging for ABI/module-path development.
#
# Layout (out_dir):
#   modules/       — compressed originals kept (.ko.xz/.gz/.zst) AND plain
#                    ELF .ko alongside (unxz -k -c style decompress)
#   modules_raw/   — plain ELF .ko only (mirror of decompressed)
#   meta/RAW-OK.txt — relative paths of every plain ELF produced
#   firmware/ meta/ NEEDED-DRIVERS.txt — as before
#
# Optional embed is FORBIDDEN without an explicit operator sentence.
# Default dest is abandoned/kernel/proc (not product-linked).
#
# G752VT xHCI note (PCI 8086:a12f):
#   Preferred modules: xhci_pci + xhci_hcd (+ usbcore stack).
#   Many distros build xhci_* / usbcore as *builtin* → no .ko to stage; meta
#   records BUILTIN and NEEDED-DRIVERS notes presence. Soft kernel path then
#   SKIP (builtin). usb-storage / uas are often *modular* — collect stages them
#   and may auto-embed usb-storage for soft multi-mod MSC leaf smoke.
#
# Usage:
#   ./scripts/collect-linux-drivers.sh [out_dir]
# Default out_dir: build/linux-drivers
#
# Env:
#   GJ_LINUX_KVER   — uname -r override
#   GJ_LINUX_MODROOT — /lib/modules/$KVER override
#   GJ_LINUX_FWROOT  — /lib/firmware override
#
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
out="${1:-build/linux-drivers}"
kver="${GJ_LINUX_KVER:-$(uname -r 2>/dev/null || echo unknown)}"
modroot="${GJ_LINUX_MODROOT:-/lib/modules/$kver}"
fwroot="${GJ_LINUX_FWROOT:-/lib/firmware}"

mkdir -p "$out/modules" "$out/modules_raw" "$out/firmware" "$out/meta"

# G752VT + generic laptop bring-up set (PCI ID → preferred module names).
# Expand from linux-hwtest reports when available.
cat >"$out/meta/G752VT-MAP.txt" <<'EOF'
# PCI ID          class hint              preferred modules (comma = alts)
8086:a12f         xHCI Intel 100 Series   xhci_pci,xhci_hcd
10ec:8168         Realtek RTL8111/8168    r8169
13fe:6400         USB MSC (Kingston-class) usb_storage,uas,usbcore,usb_common
8086:2822         AHCI (if present)       ahci,libahci,libata
10de:1618         NVIDIA GPU (if present) nvidia (proprietary out-of-tree — not auto-copied)
EOF

# Core module basenames to harvest (deps resolved via modinfo when present).
# shellcheck disable=SC2086
CORE_MODS="
r8169
realtek
libphy
mdio_devres
xhci_pci
xhci_hcd
usbcore
usb_common
usb_storage
uas
ehci_hcd
ehci_pci
ahci
libahci
libata
nvme
nvme_core
scsi_mod
sd_mod
hid
hid_generic
usbhid
virtio_net
virtio_pci
virtio_ring
virtio
"

# Decompress one staged compressed module into plain ELF .ko.
# Keeps the compressed original; also writes modules_raw/<name>.ko.
# Returns 0 if a plain .ko is present (already or freshly made).
decompress_one_mod() {
	comp="$1"
	[ -f "$comp" ] || return 1
	base=$(basename "$comp")
	plain=""
	case "$base" in
	*.ko.xz)  plain=${base%.xz} ;;
	*.ko.gz)  plain=${base%.gz} ;;
	*.ko.zst) plain=${base%.zst} ;;
	*.ko)     plain=$base ;;
	*)        return 1 ;;
	esac
	dest="$out/modules/$plain"
	rawdest="$out/modules_raw/$plain"

	if [ "$base" = "$plain" ]; then
		# Already plain ELF; mirror into modules_raw for inventory.
		if [ ! -f "$rawdest" ] || [ "$comp" -nt "$rawdest" ]; then
			cp -f "$comp" "$rawdest"
		fi
		return 0
	fi

	# Compressed: produce modules/<name>.ko alongside and modules_raw copy.
	if [ -f "$dest" ] && [ ! "$comp" -nt "$dest" ]; then
		if [ ! -f "$rawdest" ] || [ "$dest" -nt "$rawdest" ]; then
			cp -f "$dest" "$rawdest"
		fi
		return 0
	fi

	ok=0
	case "$base" in
	*.ko.xz)
		if command -v unxz >/dev/null 2>&1; then
			# unxz -k -c: keep source, write plain stream
			if unxz -k -c "$comp" >"$dest"; then
				ok=1
			else
				rm -f "$dest"
			fi
		elif command -v xz >/dev/null 2>&1; then
			if xz -dc "$comp" >"$dest"; then
				ok=1
			else
				rm -f "$dest"
			fi
		fi
		;;
	*.ko.gz)
		if command -v gzip >/dev/null 2>&1; then
			if gzip -dc "$comp" >"$dest"; then
				ok=1
			else
				rm -f "$dest"
			fi
		elif command -v gunzip >/dev/null 2>&1; then
			if gunzip -c "$comp" >"$dest"; then
				ok=1
			else
				rm -f "$dest"
			fi
		fi
		;;
	*.ko.zst)
		if command -v zstd >/dev/null 2>&1; then
			if zstd -dc "$comp" >"$dest"; then
				ok=1
			else
				rm -f "$dest"
			fi
		fi
		;;
	esac

	if [ "$ok" -eq 1 ] && [ -f "$dest" ]; then
		cp -f "$dest" "$rawdest"
		return 0
	fi
	echo "DECOMPRESS_FAIL $base" >>"$out/meta/DECOMPRESS-FAIL.txt"
	return 1
}

# Sweep modules/: ensure plain .ko alongside every compressed file; write RAW-OK.
ensure_raw_modules() {
	: >"$out/meta/RAW-OK.txt"
	: >"$out/meta/DECOMPRESS-FAIL.txt"
	mkdir -p "$out/modules" "$out/modules_raw"

	# Decompress any compressed artifacts first.
	for f in "$out/modules"/*; do
		[ -f "$f" ] || continue
		base=$(basename "$f")
		case "$base" in
		*.ko.xz|*.ko.gz|*.ko.zst)
			decompress_one_mod "$f" || true
			;;
		*.ko)
			decompress_one_mod "$f" || true
			;;
		esac
	done

	# List unique plain ELF paths (prefer modules/, note modules_raw twin).
	for f in "$out/modules"/*.ko; do
		[ -f "$f" ] || continue
		base=$(basename "$f")
		case "$base" in
		*.ko.xz|*.ko.gz|*.ko.zst) continue ;;
		esac
		# Record both staged locations when present.
		echo "modules/$base" >>"$out/meta/RAW-OK.txt"
		if [ -f "$out/modules_raw/$base" ]; then
			echo "modules_raw/$base" >>"$out/meta/RAW-OK.txt"
		fi
	done
}

copy_one_mod() {
	name="$1"
	# Skip empties / junk from depends parsing
	case "$name" in
	""|"name:"|*" "*) return 1 ;;
	esac
	# Already have a file for this basename?
	if ls "$out/modules/${name}.ko" "$out/modules/${name}.ko.xz" \
		"$out/modules/${name}.ko.gz" "$out/modules/${name}.ko.zst" \
		2>/dev/null | grep -q .; then
		# Ensure plain ELF exists alongside any compressed copy.
		for existing in \
			"$out/modules/${name}.ko.xz" \
			"$out/modules/${name}.ko.gz" \
			"$out/modules/${name}.ko.zst" \
			"$out/modules/${name}.ko"
		do
			[ -f "$existing" ] || continue
			decompress_one_mod "$existing" || true
			break
		done
		return 0
	fi
	path=""
	if command -v modinfo >/dev/null 2>&1; then
		path=$(modinfo -n "$name" 2>/dev/null || true)
	fi
	if [ -z "$path" ] || [ ! -f "$path" ]; then
		# Search under modroot
		path=$(find "$modroot" -type f \( \
			-name "${name}.ko" -o -name "${name}.ko.xz" -o \
			-name "${name}.ko.gz" -o -name "${name}.ko.zst" \) \
			2>/dev/null | head -1 || true)
	fi
	if [ -n "$path" ] && [ -f "$path" ]; then
		base=$(basename "$path")
		cp -f "$path" "$out/modules/$base"
		# Keep compressed original; also materialize plain ELF .ko.
		decompress_one_mod "$out/modules/$base" || true
		return 0
	fi
	# Builtin or missing
	if command -v modinfo >/dev/null 2>&1 && modinfo -n "$name" 2>/dev/null | grep -qi builtin; then
		echo "BUILTIN $name" >>"$out/meta/BUILTIN.txt"
		return 0
	fi
	echo "MISSING $name" >>"$out/meta/MISSING.txt"
	return 1
}

resolve_deps() {
	name="$1"
	command -v modinfo >/dev/null 2>&1 || return 0
	deps=$(modinfo -F depends "$name" 2>/dev/null | tr ',' ' ' || true)
	for d in $deps; do
		[ -n "$d" ] || continue
		copy_one_mod "$d" || true
		# one-level recursive soft
		sub=$(modinfo -F depends "$d" 2>/dev/null | tr ',' ' ' || true)
		for s in $sub; do
			[ -n "$s" ] || continue
			copy_one_mod "$s" || true
		done
	done
}

: >"$out/meta/BUILTIN.txt"
: >"$out/meta/MISSING.txt"
: >"$out/meta/COPIED.txt"

for m in $CORE_MODS; do
	[ -n "$m" ] || continue
	if copy_one_mod "$m"; then
		echo "COPIED_OR_BUILTIN $m" >>"$out/meta/COPIED.txt"
		resolve_deps "$m"
	fi
done

# Also pull anything currently bound to G752-class PCI if host is the DUT
if command -v lspci >/dev/null 2>&1; then
	{
		echo "# Host lspci -k snapshot ($(date -u -Iseconds 2>/dev/null || date -u))"
		lspci -nnk 2>/dev/null || lspci -k 2>/dev/null || true
	} >"$out/meta/HOST-LSPCI-K.txt" || true
	# Extract "Kernel driver in use" / "Kernel modules" lines for our IDs
	for id in "8086:a12f" "10ec:8168" "8086:2822"; do
		lspci -nnk 2>/dev/null | awk -v id="$id" '
			index($0, id) { hit=1; print; next }
			hit && /^[0-9a-f]/..:/{ hit=0 }
			hit { print }
		' >>"$out/meta/HOST-MATCHED.txt" 2>/dev/null || true
	done
fi

# Firmware blobs commonly needed by r8169 / xHCI (best-effort)
for pat in \
	"rtl_nic/*" \
	"rtl_nic/rtl8168*" \
	"i915/*" \
	"nvidia/*" \
	; do
	# shellcheck disable=SC2086
	for f in $fwroot/$pat; do
		[ -f "$f" ] || continue
		rel=${f#"$fwroot"/}
		mkdir -p "$out/firmware/$(dirname "$rel")"
		cp -f "$f" "$out/firmware/$rel" 2>/dev/null || true
	done
done 2>/dev/null || true

# Materialize plain ELF .ko alongside compressed; write meta/RAW-OK.txt
ensure_raw_modules

# Classify one CORE_MOD name: PRESENT (.ko staged) | BUILTIN | MISSING | UNKNOWN
# Accepts underscore or hyphen (usb_storage ↔ usb-storage.ko on disk).
mod_presence() {
	name="$1"
	name_hy=$(printf '%s' "$name" | tr '_' '-')
	name_us=$(printf '%s' "$name" | tr '-' '_')
	for n in "$name" "$name_hy" "$name_us"; do
		if [ -f "$out/modules/${n}.ko" ] || [ -f "$out/modules_raw/${n}.ko" ] ||
			ls "$out/modules/${n}.ko.xz" "$out/modules/${n}.ko.gz" \
				"$out/modules/${n}.ko.zst" 2>/dev/null | grep -q .; then
			echo "PRESENT"
			return 0
		fi
	done
	if [ -s "$out/meta/BUILTIN.txt" ] &&
		grep -qE "^BUILTIN (${name}|${name_hy}|${name_us})\$" "$out/meta/BUILTIN.txt"; then
		echo "BUILTIN"
		return 0
	fi
	if [ -s "$out/meta/MISSING.txt" ] &&
		grep -qE "^MISSING (${name}|${name_hy}|${name_us})\$" "$out/meta/MISSING.txt"; then
		echo "MISSING"
		return 0
	fi
	echo "UNKNOWN"
}

# G752VT xHCI (8086:a12f) — explicit presence oracle for soft module path.
# Host often has xhci_pci/xhci_hcd/usbcore as *builtin* → no .ko for embed.
# usb-storage / uas are often modular (MSC leaf) even when HC is builtin.
xhci_pci_st=$(mod_presence xhci_pci)
xhci_hcd_st=$(mod_presence xhci_hcd)
usbcore_st=$(mod_presence usbcore)
usb_common_st=$(mod_presence usb_common)
usb_storage_st=$(mod_presence usb_storage)
uas_st=$(mod_presence uas)
r8169_st=$(mod_presence r8169)

{
	echo "# GreenJade collect — xHCI soft-path presence (PCI 8086:a12f)"
	echo "# Generated: $(date -u -Iseconds 2>/dev/null || date -u)"
	echo "# Host kver: $kver"
	echo "# Soft≠product. No GPL source in tree. Builtin ⇒ no plain .ko to embed."
	echo
	echo "PCI_ID 8086:a12f"
	echo "CLASS  xHCI Intel 100 Series / C230 (G752VT-class)"
	echo "PREFERRED_MODS xhci_pci,xhci_hcd,usbcore,usb_common"
	echo "MULTI_MOD_ORDER usb_common usbcore xhci_hcd xhci_pci usb_storage"
	echo "  (ideal Linux insmod order for stick MSC; soft path stubs this order)"
	echo
	echo "xhci_pci=$xhci_pci_st"
	echo "xhci_hcd=$xhci_hcd_st"
	echo "usbcore=$usbcore_st"
	echo "usb_common=$usb_common_st"
	echo "usb_storage=$usb_storage_st"
	echo "uas=$uas_st"
	echo "r8169=$r8169_st"
	echo
	case "$xhci_pci_st" in
	PRESENT)
		echo "SOFT_PATH note: plain xhci_pci.ko staged — optional embed via"
		echo "  ./scripts/embed-linux-mod.sh xhci_pci"
		echo "  (Makefile may pick up kernel/proc/xhci_pci_mod_blob.S if present)"
		echo "greppable: collect-linux-drivers: xhci 8086:a12f xhci_pci=PRESENT"
		;;
	BUILTIN)
		echo "SOFT_PATH note: xhci_pci is BUILTIN on this host — no .ko to copy."
		echo "  Soft kernel path: SKIP (builtin). STATUS: MOD XHCI PCI SKIP BUILTIN."
		echo "  USB linux path OPEN for host-controller .ko until collected from a"
		echo "  kver/distro that builds xhci_pci as a module (all el9 kvers here:"
		echo "  builtin). Leaf usb-storage may still be modular — see below."
		echo "greppable: collect-linux-drivers: xhci 8086:a12f xhci_pci=BUILTIN"
		;;
	MISSING)
		echo "SOFT_PATH note: xhci_pci MISSING — collect from another kver/distro."
		echo "greppable: collect-linux-drivers: xhci 8086:a12f xhci_pci=MISSING"
		;;
	*)
		echo "SOFT_PATH note: xhci_pci status UNKNOWN — re-run collect / check modroot."
		echo "greppable: collect-linux-drivers: xhci 8086:a12f xhci_pci=UNKNOWN"
		;;
	esac
	echo "greppable: collect-linux-drivers: xhci 8086:a12f xhci_hcd=$xhci_hcd_st"
	echo "greppable: collect-linux-drivers: xhci 8086:a12f usbcore=$usbcore_st"
	echo "greppable: collect-linux-drivers: usb_storage=$usb_storage_st"
	echo "greppable: collect-linux-drivers: uas=$uas_st"
	if [ "$usb_storage_st" = "PRESENT" ]; then
		echo
		echo "MSC leaf: usb-storage.ko PRESENT — optional soft embed:"
		echo "  ./scripts/embed-linux-mod.sh usb-storage"
		echo "  → kernel/proc/usb_storage_mod_blob.S (weak; Makefile if present)"
		echo "  Soft boot: main: soft linux_module usb_storage path …"
		echo "  Honesty: MSC class alone cannot enumerate a stick without HC+usbcore."
	fi
} >"$out/meta/XHCI-STATUS.txt"

# USB stack inventory (HC + MSC leaf) — peer to XHCI-STATUS for D8 honesty.
{
	echo "# GreenJade collect — USB linux module path (HC + MSC leaf)"
	echo "# Generated: $(date -u -Iseconds 2>/dev/null || date -u)"
	echo "# Host kver: $kver"
	echo "# Soft≠product. G-AC-1. No GPL source in tree."
	echo
	echo "MULTI_MOD_ORDER usb_common usbcore xhci_hcd xhci_pci usb_storage"
	echo "PCI_HC 8086:a12f"
	echo
	echo "usb_common=$usb_common_st"
	echo "usbcore=$usbcore_st"
	echo "xhci_hcd=$xhci_hcd_st"
	echo "xhci_pci=$xhci_pci_st"
	echo "usb_storage=$usb_storage_st"
	echo "uas=$uas_st"
	echo
	if [ "$xhci_pci_st" = "BUILTIN" ] && [ "$usbcore_st" = "BUILTIN" ]; then
		echo "HOST_REALITY: xHCI + usbcore BUILTIN (no HC .ko on this host/kver)."
		echo "  Soft STATUS: MOD XHCI PCI SKIP BUILTIN / USB linux path OPEN builtin"
		echo "  for controller; MSC leaf may still load soft if usb-storage.ko staged."
	fi
	if [ "$usb_storage_st" = "PRESENT" ]; then
		echo "HOST_REALITY: usb-storage modular PRESENT — stage + optional embed OK."
		echo "  Next for stick write via Linux modules: need modular (or soft) HC"
		echo "  stack first; freestanding xhci_msc remains lab BOT path."
	fi
	echo "greppable: collect-linux-drivers: usb path OPEN builtin hc=$xhci_pci_st msc=$usb_storage_st"
} >"$out/meta/USB-STATUS.txt"

# NEEDED-DRIVERS.txt — install checklist for operators
{
	echo "GreenJade — needed Linux drivers checklist (operator collect)"
	echo "=============================================================="
	echo "Generated: $(date -u -Iseconds 2>/dev/null || date -u)"
	echo "Host kver: $kver"
	echo "Modroot:   $modroot"
	echo
	echo "Policy"
	echo "  GreenJade product path: Linux ABI + DDI so Linux-shaped / collected"
	echo "  drivers can be hosted — not reimplementing every class driver."
	echo "  Staged .ko are host binaries for ABI/module-path *development*."
	echo "  Module load on freestanding GreenJade: OPEN (not yet product)."
	echo "  GPL source is NOT imported into the GreenJade source tree."
	echo
	echo "G752VT minimum set (collect these for install)"
	echo "  PCI 8086:a12f  →  xhci_pci + xhci_hcd (+ usbcore stack)"
	echo "  PCI 10ec:8168  →  r8169 (+ phy helpers as deps)"
	echo "  USB MSC stick  →  usb_storage and/or uas"
	echo "  Storage        →  ahci / nvme as present on DUT"
	echo
	echo "PCI 8086:a12f xHCI presence (this host collect)"
	echo "  xhci_pci=$xhci_pci_st  xhci_hcd=$xhci_hcd_st"
	echo "  usbcore=$usbcore_st  usb_common=$usb_common_st"
	echo "  usb_storage=$usb_storage_st  uas=$uas_st"
	echo "  r8169=$r8169_st  (NIC peer; soft path separate)"
	echo "  multi-mod order: usb_common → usbcore → xhci_hcd → xhci_pci → usb_storage"
	if [ "$xhci_pci_st" = "BUILTIN" ] || [ "$xhci_hcd_st" = "BUILTIN" ]; then
		echo "  NOTE: host often has xHCI as *builtin* (no .ko). Soft module path"
		echo "  greps: main: soft linux_module xhci path SKIP builtin"
		echo "  STATUS: USB linux path OPEN builtin (HC); collect HC .ko from"
		echo "  a distro/kver that builds modules, then: embed-linux-mod.sh xhci_pci"
	elif [ "$xhci_pci_st" = "PRESENT" ]; then
		echo "  NOTE: xhci_pci.ko PRESENT — optional: embed-linux-mod.sh xhci_pci"
		echo "  Soft path may load_mem if xhci_pci_mod_blob.S is linked."
	fi
	if [ "$usb_storage_st" = "PRESENT" ]; then
		echo "  NOTE: usb-storage.ko PRESENT — optional: embed-linux-mod.sh usb-storage"
		echo "  Soft boot may load leaf MSC module (deps still need usbcore/HC)."
	fi
	echo "  meta detail: meta/XHCI-STATUS.txt  meta/USB-STATUS.txt"
	echo "  greppable: collect-linux-drivers: xhci 8086:a12f xhci_pci=$xhci_pci_st"
	echo "  greppable: collect-linux-drivers: usb_storage=$usb_storage_st"
	echo
	echo "How to collect on a working Linux install of the same machine"
	echo "  1. Boot Linux on the DUT (or identical HW)."
	echo "  2. sudo ./scripts/linux-hwtest/... or: lspci -nnk > pci-k.txt"
	echo "  3. On lab host: ./scripts/collect-linux-drivers.sh"
	echo "  4. make hwtest-img  # packs build/linux-drivers → GJ-PERSIST"
	echo "  5. Or copy build/linux-drivers onto stick: linux-drivers/"
	echo
	echo "On-media layout (after hwtest-img)"
	echo "  GJ-PERSIST/linux-drivers/modules/     — .ko + .ko.xz (both kept)"
	echo "  GJ-PERSIST/linux-drivers/modules_raw/ — plain ELF .ko only"
	echo "  GJ-PERSIST/linux-drivers/firmware/    — soft firmware blobs"
	echo "  GJ-PERSIST/linux-drivers/meta/        — maps + RAW-OK + XHCI-STATUS"
	echo "  EFI/GREENJADE/NEEDED-DRIVERS.txt      — this checklist (ESP copy)"
	echo "  ESP /linux-drivers/modules/r8169.ko  — UEFI soft media (D4; make hwtest-img)"
	echo
	echo "Staged modules (this run)"
	if ls "$out/modules"/* >/dev/null 2>&1; then
		ls -1 "$out/modules" | sed 's/^/  /'
	else
		echo "  (none — host modules missing or all builtin; see meta/)"
	fi
	echo
	echo "Plain ELF (RAW-OK) paths"
	if [ -s "$out/meta/RAW-OK.txt" ]; then
		sed 's/^/  /' "$out/meta/RAW-OK.txt"
	else
		echo "  (none — decompress failed or no modules; see meta/)"
	fi
	echo
	echo "Builtin on host (no file to copy)"
	if [ -s "$out/meta/BUILTIN.txt" ]; then
		# Unique lines only (deps may re-hit the same BUILTIN name)
		sort -u "$out/meta/BUILTIN.txt" | sed 's/^/  /'
	else
		echo "  (none recorded)"
	fi
	echo
	echo "Missing on host (user must collect from another kver/distro)"
	if [ -s "$out/meta/MISSING.txt" ]; then
		sort -u "$out/meta/MISSING.txt" | sed 's/^/  /'
	else
		echo "  (none)"
	fi
	echo
	echo "Greppable: collect-linux-drivers: PASS"
	echo "Greppable: collect-linux-drivers: xhci 8086:a12f xhci_pci=$xhci_pci_st"
} >"$out/NEEDED-DRIVERS.txt"

cp -f "$out/NEEDED-DRIVERS.txt" "$out/meta/NEEDED-DRIVERS.txt"

n_mod=$(find "$out/modules" -type f 2>/dev/null | wc -l | tr -d ' ')
n_raw=$(find "$out/modules_raw" -type f -name '*.ko' 2>/dev/null | wc -l | tr -d ' ')
n_fw=$(find "$out/firmware" -type f 2>/dev/null | wc -l | tr -d ' ')
echo "collect-linux-drivers: PASS out=$out kver=$kver modules=$n_mod raw_ko=$n_raw firmware=$n_fw"
echo "  checklist: $out/NEEDED-DRIVERS.txt"
echo "  raw list:  $out/meta/RAW-OK.txt"
echo "  xhci:      $out/meta/XHCI-STATUS.txt (8086:a12f xhci_pci=$xhci_pci_st)"
echo "  usb:       $out/meta/USB-STATUS.txt (usb_storage=$usb_storage_st)"
echo "  greppable: collect-linux-drivers: PASS"
echo "  greppable: collect-linux-drivers: xhci 8086:a12f xhci_pci=$xhci_pci_st"
echo "  greppable: collect-linux-drivers: usb_storage=$usb_storage_st"

# In-kernel .ko / rtl_nic fw embeds were moved to ./abandoned and are
# not linked. Do not regenerate kernel/proc/*_blob.S here.
echo "collect-linux-drivers: skip kernel embed (abandoned; media-only collect)"
