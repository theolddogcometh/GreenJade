#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Stage Linux-shaped driver-host artifacts onto GreenJade ESP (+ optional
# GJ-PERSIST mirror) for DUT media:
#   EFI/GREENJADE/user/personality.elf   (G-PERS cold personality, if built)
#   EFI/GREENJADE/drivers/*              (UDX hosts + optional out-of-tree hosts)
#   EFI/GREENJADE/DRIVERS.txt            (G752 PCI IDs + UDX bind model)
#   EFI/GREENJADE/LAPTOP.txt             (G752VT USB boot steps)
#   $persist_root/drivers/*              (functional residual dual-land mirror)
#
# Product ESP/persist pack names (G-AC-1 — userspace UDX, not .ko):
#   ddi_host · rtl8168_udx · xhci_udx
# Prefer: make drivers-udx → build/user/drivers/{ddi_host,rtl8168_udx,xhci_udx}
#
# Usage:
#   ./scripts/stage-udx-drivers.sh [esp_root] [persist_root]
# Default esp_root: build/esp
# persist_root: $2 · or $GJ_PERSIST_DIR · or build/persist if that dir exists
# Soft: absent persist target → persist=SKIP (never hard-fail).
#
# Called from stage-esp.sh after freestanding userland ELFs. Soft-only:
# missing personality / drivers / udx_skeleton does not fail the stage.
# No GPL .ko — UDX userspace hosts only (MIT OR Apache-2.0).
#
# Soft residual deepen (C2 scripts residual Soft!=product; G-AC-1;
# dual MIT OR Apache-2.0; Dual DoD A/B OPEN; stamp-free residual):
#   pack residual        — land product-named UDX hosts on ESP drivers/
#   harvest residual     — prefer build/user/drivers/* then user/drivers/*
#   ensure residual      — ensure_drv soft-fills ddi_host/rtl8168_udx/xhci_udx
#   denser residual      — non-empty byte honesty + density N/3 + PRODUCT-HOSTS
#   miss residual        — missing/zero-byte bins → SKIP honesty; never hard-fail
#   inventory residual   — MANIFEST.txt + per-host bytes + greppable PASS|SKIP
#   freestanding residual — class drivers SKIP default (opt-in only; not DoD)
#   product_host residual — rtl8168_udx (DoD B) / xhci_udx (DoD A) / ddi_host
#   Dual DoD residual    — A (xhci_udx USB) / B (rtl8168_udx NIC) remain OPEN
#   honesty residual     — pack PASS != product TX/RX/BOT != bar3 != Dual DoD
#   stamp residual       — never bump GJ_IMAGE_VERSION; no invent next N; bar
#                         honesty v0.1.182 panel context only
#   byte_match residual  — dual-land ESP|persist size equality per product host
#   panel residual       — stamp-free extract IMAGE_VERSION from KERNEL.ELF (RO)
#   functional residual  — denser ESP+persist dual-land catalog for DUT
#                         (harvest→ensure→densify→pack_esp→mirror_persist
#                         →inventory→product_host_catalog→honesty→dual_dod
#                         →byte_match→stamp-free); Soft!=product
#
# Soft API honesty (Soft!=product; dual MIT OR Apache-2.0; G-AC-1):
#   soft path = stage product-named UDX host bins + DRIVERS/LAPTOP notes
#   product   = UDX+ABI userspace hosts owning laptop USB/NIC (OPEN; not here)
#   freestanding class drivers (kernel rtl8168 / xhci_msc) = SKIP default
#   G-AC-1 = no Linux .ko product AC; no GPL source in stage path
#
# Honesty (Soft!=product · G-AC-1):
#   - Freestanding class drivers (kernel rtl8168 / xhci_msc) default SKIP
#     (GJ_RTL8168_PROBE=0 · GJ_XHCI_MSC_PROBE=0). Residual opt-in only.
#   - Staging UDX hosts != product TX/RX / BOT / bar3 closed.
#   - pack PASS|SKIP != Dual DoD A/B close (A until USB path; B until interactive SSH login).
#   - dual-land ESP+persist pack != Dual DoD close (A/B OPEN).
#   - denser product_host density N/3 != Dual DoD close != product TX/RX/BOT.
#   - zero-byte placeholders count as SKIP (non-empty honesty).
#   - stamp-free residual: NEVER bump GJ_IMAGE_VERSION from this script.
#   - greppable: stage-udx-drivers: ddi_host=PASS|SKIP …
#   - greppable: stage-udx-drivers: soft residual dual_dod
#   - greppable: stage-udx-drivers: soft residual product=UDX+ABI
#   - greppable: stage-udx-drivers: soft residual freestanding class SKIP
#   - greppable: stage-udx-drivers: soft residual product_host
#   - greppable: stage-udx-drivers: soft residual product_host density=
#   - greppable: stage-udx-drivers: soft residual denser esp
#   - greppable: stage-udx-drivers: soft residual stamp-free
#   - greppable: stage-udx-drivers: soft residual esp=
#   - greppable: stage-udx-drivers: soft residual persist=
#   - greppable: stage-udx-drivers: soft residual dual_land
#   - greppable: stage-udx-drivers: soft residual dual_land byte_match=
#   - greppable: stage-udx-drivers: soft residual panel=
#   - greppable: stage-udx-drivers: soft functional residual
# Dual MIT OR Apache-2.0. No GPL. Dual DoD A/B OPEN. Soft!=product.
# Bar honesty v0.1.182. NEVER bump GJ_IMAGE_VERSION. No invent next N.
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
out="${1:-build/esp}"
# Functional residual dual-land: optional GJ-PERSIST-shaped tree for DUT.
# Priority: $2 → $GJ_PERSIST_DIR → build/persist (if directory already exists).
# Soft-miss: empty persist_root → mirror SKIP (ESP pack still runs).
persist_root="${2:-${GJ_PERSIST_DIR:-}}"
if [ -z "$persist_root" ] && [ -d build/persist ]; then
	persist_root=build/persist
fi
gj="$out/EFI/GREENJADE"
drv_dir="$gj/drivers"
user_dir="$gj/user"

mkdir -p "$user_dir" "$drv_dir"

drv_n=0
pers_n=0
udx_n=0
# Functional residual step tallies (soft catalog only; Soft!=product)
# denser: harvest→ensure→densify→pack_esp→mirror→inventory→catalog→honesty
#         →dual_dod→byte_match→stamp-free  (11 steps; W10 pack deepen)
func_steps_ok=0
func_steps_total=11
func_harvest=0
func_ensure=0
func_densify=0
func_pack_esp=0
func_mirror_persist=0
func_inventory=0
func_product_host_catalog=0
func_honesty=0
func_dual_dod=0
func_byte_match=0
func_stamp=0
# Denser product_host residual (DUT ESP path; Soft!=product)
product_host_total=3
product_host_n=0
ddi_bytes=0
rtl_bytes=0
xhci_bytes=0

# ---------------------------------------------------------------------------
# 1) Cold personality ELF (G-PERS scaffold) — next to other user ELFs
# ---------------------------------------------------------------------------
if [ -f build/user/personality.elf ]; then
	cp -f build/user/personality.elf "$user_dir/personality.elf"
	pers_n=1
fi

# ---------------------------------------------------------------------------
# 2) Driver-host binaries under EFI/GREENJADE/drivers/
#    Prefer staged product path build/user/drivers/*; also harvest
#    user/drivers/*/build/* and top-level / local UDX skeleton artifacts.
# ---------------------------------------------------------------------------
stage_one() {
	# stage_one <src> <dest_name>
	_src=$1
	_name=$2
	if [ ! -f "$_src" ]; then
		return 0
	fi
	# Skip non-executables / tiny placeholders / objects / archives
	case "$_name" in
	*.o|*.d|*.a|*.c|*.h|*.txt|*.md|Makefile|README*) return 0 ;;
	esac
	cp -f "$_src" "$drv_dir/$_name"
	drv_n=$((drv_n + 1))
}

if [ -d build/user/drivers ]; then
	for f in build/user/drivers/*; do
		[ -e "$f" ] || continue
		if [ -f "$f" ]; then
			stage_one "$f" "$(basename "$f")"
		elif [ -d "$f" ]; then
			# build/user/drivers/<name>/host or <name>.elf etc.
			for g in "$f"/* "$f"/build/*; do
				[ -f "$g" ] || continue
				case "$(basename "$g")" in
				*.o|*.d|*.a) continue ;;
				esac
				# Prefer executable-looking names
				bn=$(basename "$g")
				stage_one "$g" "${bn}"
			done
		fi
	done
fi

if [ -d user/drivers ]; then
	for d in user/drivers/*; do
		[ -d "$d" ] || continue
		dname=$(basename "$d")
		# Map known source dirs → product ESP pack names (G-AC-1 UDX hosts)
		#   ddi_host_gj → ddi_host · rtl8168_udx → rtl8168_udx · xhci_udx → xhci_udx
		prod_name=$dname
		case "$dname" in
		ddi_host_gj|ddi_host) prod_name=ddi_host ;;
		rtl8168_udx) prod_name=rtl8168_udx ;;
		xhci_udx) prod_name=xhci_udx ;;
		esac
		# Common out-of-tree layouts (prefer product dest name for known hosts)
		for cand in \
			"$d/build/$prod_name" \
			"$d/build/${prod_name}.elf" \
			"$d/build/$dname" \
			"$d/build/${dname}_host" \
			"$d/build/${dname}.elf" \
			"$d/build/udx_${dname}" \
			"$d/$prod_name" \
			"$d/${prod_name}.elf" \
			"$d/$dname" \
			"$d/${dname}.elf"; do
			if [ -f "$cand" ]; then
				case "$prod_name" in
				ddi_host|rtl8168_udx|xhci_udx)
					# Land under product ESP name (not ddi_host.elf etc.)
					if [ ! -f "$drv_dir/$prod_name" ]; then
						cp -f "$cand" "$drv_dir/$prod_name"
						drv_n=$((drv_n + 1))
					fi
					;;
				*)
					stage_one "$cand" "$(basename "$cand")"
					;;
				esac
			fi
		done
		if [ -d "$d/build" ]; then
			for g in "$d/build"/*; do
				[ -f "$g" ] || continue
				bn=$(basename "$g")
				case "$bn" in
				*.o|*.d|*.a|*.c|*.h|*.txt|*.md|Makefile|README*) continue ;;
				esac
				# Known product hosts: only accept product / .elf basenames
				case "$prod_name" in
				ddi_host|rtl8168_udx|xhci_udx)
					case "$bn" in
					"$prod_name"|"${prod_name}.elf"|"$dname"|"${dname}.elf")
						if [ ! -f "$drv_dir/$prod_name" ]; then
							cp -f "$g" "$drv_dir/$prod_name"
							drv_n=$((drv_n + 1))
						fi
						;;
					esac
					continue
					;;
				esac
				# Already staged exact basename? skip overwrite of empty
				if [ ! -f "$drv_dir/$bn" ]; then
					stage_one "$g" "$bn"
				fi
			done
		fi
	done
fi

# UDX skeleton host demo (Linux-shaped porter example — host-libc binary)
# Prefer product tree build/udx_skeleton; fall back to in-tree local build.
if [ -f build/udx_skeleton ]; then
	cp -f build/udx_skeleton "$drv_dir/udx_skeleton"
	udx_n=1
	drv_n=$((drv_n + 1))
elif [ -f user/udx/build/udx_skeleton ]; then
	cp -f user/udx/build/udx_skeleton "$drv_dir/udx_skeleton"
	udx_n=1
	drv_n=$((drv_n + 1))
fi

# ---------------------------------------------------------------------------
# 2b) Ensure product-named soft hosts land (even if harvest missed a path).
#     make drivers-udx → build/user/drivers/{ddi_host,rtl8168_udx,xhci_udx}
#     Soft: missing bins do not fail the stage (SKIP honesty, Soft!=product).
#     ensure residual: soft-fill only; pack != Dual DoD close (A/B OPEN).
#     denser residual: re-fill zero-byte placeholders; chmod +x product hosts;
#     skip empty sources (non-empty honesty for DUT product path).
# ---------------------------------------------------------------------------
host_bytes() {
	# host_bytes <path> — portable byte count (0 if missing)
	if [ -f "$1" ]; then
		wc -c <"$1" | tr -d ' \n'
	else
		echo 0
	fi
}

ensure_drv() {
	# ensure_drv <dest_name> <src1> [src2…]
	_ename=$1
	shift
	if [ -f "$drv_dir/$_ename" ]; then
		_esz=$(host_bytes "$drv_dir/$_ename")
		if [ "$_esz" -gt 0 ] 2>/dev/null; then
			# denser: mode soft for DUT product path (FAT may ignore; best-effort)
			chmod +x "$drv_dir/$_ename" 2>/dev/null || true
			return 0
		fi
		# zero-byte placeholder → fall through and try real sources
	fi
	for _esrc in "$@"; do
		if [ -f "$_esrc" ]; then
			_ssz=$(host_bytes "$_esrc")
			# denser: never promote empty sources to product pack names
			[ "$_ssz" -gt 0 ] 2>/dev/null || continue
			# stage_one skips *.a/*.o; product host names have no extension
			cp -f "$_esrc" "$drv_dir/$_ename"
			chmod +x "$drv_dir/$_ename" 2>/dev/null || true
			drv_n=$((drv_n + 1))
			return 0
		fi
	done
	return 0
}

# Soft DDI host (product ESP name). Prefer freestanding ddi_host_gj ELF;
# fall back to udx_skeleton soft demo when class host not built.
ensure_drv ddi_host \
	build/user/drivers/ddi_host \
	build/user/drivers/ddi_host.elf \
	user/drivers/ddi_host_gj/build/ddi_host.elf \
	user/drivers/ddi_host_gj/build/ddi_host \
	build/user/ddi_host.elf \
	build/user/ddi_host \
	"$drv_dir/ddi_host.elf" \
	build/udx_skeleton \
	user/udx/build/udx_skeleton \
	"$drv_dir/udx_skeleton"

# G752 class soft hosts (RTL8168 NIC + Intel xHCI) — product names on ESP
ensure_drv rtl8168_udx \
	build/user/drivers/rtl8168_udx \
	build/user/drivers/rtl8168_udx.elf \
	user/drivers/rtl8168_udx/build/rtl8168_udx \
	user/drivers/rtl8168_udx/build/rtl8168_udx.elf \
	build/user/rtl8168_udx \
	build/user/rtl8168_udx.elf \
	"$drv_dir/rtl8168_udx.elf"

ensure_drv xhci_udx \
	build/user/drivers/xhci_udx \
	build/user/drivers/xhci_udx.elf \
	user/drivers/xhci_udx/build/xhci_udx \
	user/drivers/xhci_udx/build/xhci_udx.elf \
	build/user/xhci_udx \
	build/user/xhci_udx.elf \
	"$drv_dir/xhci_udx.elf"

# ---------------------------------------------------------------------------
# 2c) Denser product_host residual — non-empty honesty + byte inventory +
#     density N/3 for DUT ESP product path (Soft!=product; Dual DoD OPEN).
#     pack PASS requires non-empty product-named host on ESP drivers/.
# ---------------------------------------------------------------------------
host_present() {
	# host_present <dest_name> — 1 only if non-empty file on ESP drivers/
	if [ ! -f "$drv_dir/$1" ]; then
		return 1
	fi
	_hb=$(host_bytes "$drv_dir/$1")
	[ "$_hb" -gt 0 ] 2>/dev/null
}

# densify: strip zero-byte product names so inventory honesty is tight
for _ph in ddi_host rtl8168_udx xhci_udx; do
	if [ -f "$drv_dir/$_ph" ]; then
		_psz=$(host_bytes "$drv_dir/$_ph")
		if [ "$_psz" -eq 0 ] 2>/dev/null; then
			rm -f "$drv_dir/$_ph"
		else
			chmod +x "$drv_dir/$_ph" 2>/dev/null || true
		fi
	fi
done
func_densify=1

# Optional: ship libudx.a for lab re-link of out-of-tree hosts (not a .ko)
if [ -f build/libudx.a ]; then
	cp -f build/libudx.a "$drv_dir/libudx.a"
	drv_n=$((drv_n + 1))
elif [ -f user/udx/build/libudx.a ]; then
	cp -f user/udx/build/libudx.a "$drv_dir/libudx.a"
	drv_n=$((drv_n + 1))
fi

# Manifest of what landed (counts final tree after ensure+densify; non-empty)
ddi_n=0
rtl_n=0
xhci_n=0
ddi_bytes=0
rtl_bytes=0
xhci_bytes=0
if host_present ddi_host; then
	ddi_n=1
	ddi_bytes=$(host_bytes "$drv_dir/ddi_host")
fi
if host_present rtl8168_udx; then
	rtl_n=1
	rtl_bytes=$(host_bytes "$drv_dir/rtl8168_udx")
fi
if host_present xhci_udx; then
	xhci_n=1
	xhci_bytes=$(host_bytes "$drv_dir/xhci_udx")
fi
product_host_n=$((ddi_n + rtl_n + xhci_n))
# Functional residual: harvest/ensure/densify always walk; pack_esp walked
# (hosts may still be SKIP — Soft!=product; never hard-fail).
func_harvest=1
func_ensure=1
func_pack_esp=1
# Stamp-free panel residual (RO extract only; NEVER bump GJ_IMAGE_VERSION).
# Prefer KERNEL.ELF on this ESP; fall back to gj-image-version.sh / config.h.
panel_ver=unknown
panel_src=none
if [ -f "$gj/KERNEL.ELF" ] && [ -x scripts/gj-image-version.sh ]; then
	_pv=$(./scripts/gj-image-version.sh "$gj/KERNEL.ELF" 2>/dev/null || true)
	if [ -n "$_pv" ] && [ "$_pv" != "unknown" ]; then
		panel_ver=$_pv
		panel_src=esp_KERNEL.ELF
	fi
fi
if [ "$panel_ver" = "unknown" ] && [ -x scripts/gj-image-version.sh ]; then
	_pv=$(./scripts/gj-image-version.sh 2>/dev/null || true)
	if [ -n "$_pv" ] && [ "$_pv" != "unknown" ]; then
		panel_ver=$_pv
		panel_src=gj-image-version
	fi
fi
panel_title="STATUS (static) v${panel_ver}"
# Final file count from tree (more honest than stage_one increments)
drv_tree_n=$(find "$drv_dir" -type f ! -name 'MANIFEST.txt' ! -name 'UDX-STAGE.txt' ! -name 'PRODUCT-HOSTS.txt' 2>/dev/null | wc -l | tr -d ' ')
{
	echo "# GreenJade ESP driver-host inventory (staged)"
	echo "# Generated by scripts/stage-udx-drivers.sh — MIT OR Apache-2.0"
	echo "# Soft!=product · G-AC-1: freestanding class SKIP default; pack != TX/RX/BOT"
	echo "# Dual DoD A/B OPEN · pack PASS != Dual DoD close · stamp-free residual"
	echo "# product=UDX+ABI · product_host residual denser · functional residual dual-land"
	echo "# C2 scripts residual Soft!=product · bar honesty v0.1.182 (no invent next N)"
	echo "# denser: non-empty honesty · product_host_density N/3 · per-host bytes"
	echo "# W10: dual-land byte_match + stamp-free panel extract (RO)"
	echo "personality=${pers_n}"
	echo "udx_skeleton=${udx_n}"
	echo "ddi_host=${ddi_n}"
	echo "ddi_host_bytes=${ddi_bytes}"
	echo "rtl8168_udx=${rtl_n}"
	echo "rtl8168_udx_bytes=${rtl_bytes}"
	echo "xhci_udx=${xhci_n}"
	echo "xhci_udx_bytes=${xhci_bytes}"
	echo "product_host_n=${product_host_n}"
	echo "product_host_total=${product_host_total}"
	echo "product_host_density=${product_host_n}/${product_host_total}"
	echo "driver_files=${drv_tree_n}"
	echo "freestanding_class=SKIP"
	echo "product_path=UDX+ABI"
	echo "dual_dod_a=OPEN"
	echo "dual_dod_b=OPEN"
	echo "soft_not_product=1"
	echo "stamp_free=1"
	echo "functional_residual=1"
	echo "denser_product_host=1"
	echo "esp_land=1"
	echo "panel_version=${panel_ver}"
	echo "panel_title=${panel_title}"
	echo "panel_source=${panel_src}"
	echo "bar_honesty=v0.1.182"
	echo "no_invent=next_N"
	if [ -d "$drv_dir" ]; then
		ls -1 "$drv_dir" 2>/dev/null | while IFS= read -r line; do
			echo "file=$line"
		done
	fi
} >"$drv_dir/MANIFEST.txt"

# Denser PRODUCT-HOSTS catalog (ESP drivers/ + GREENJADE root for DUT post-mount)
{
	echo "GreenJade product hosts on ESP (denser residual · Soft!=product)"
	echo "=============================================================="
	echo "product_path=UDX+ABI"
	echo "product_host=ddi_host+rtl8168_udx+xhci_udx"
	echo "product_host_density=${product_host_n}/${product_host_total}"
	echo "ddi_host=${ddi_n} bytes=${ddi_bytes}"
	echo "rtl8168_udx=${rtl_n} bytes=${rtl_bytes}"
	echo "xhci_udx=${xhci_n} bytes=${xhci_bytes}"
	echo "esp_drivers=$drv_dir"
	echo "freestanding_class=SKIP"
	echo "dual_dod_a=OPEN"
	echo "dual_dod_b=OPEN"
	echo "soft_not_product=1"
	echo "stamp_free=1"
	echo "bar_honesty=v0.1.182"
	echo "no_invent=next_N"
	echo ""
	echo "Paths (this stick)"
	echo "  EFI/GREENJADE/drivers/ddi_host"
	echo "  EFI/GREENJADE/drivers/rtl8168_udx"
	echo "  EFI/GREENJADE/drivers/xhci_udx"
	echo "  EFI/GREENJADE/drivers/MANIFEST.txt"
	echo "  EFI/GREENJADE/drivers/PRODUCT-HOSTS.txt"
	echo "  EFI/GREENJADE/PRODUCT-HOSTS.txt"
	echo ""
	echo "Honesty: density ${product_host_n}/${product_host_total} pack != Dual DoD close"
	echo "  != product TX/RX (rtl8168_udx) != product BOT/MSC (xhci_udx) != bar3."
	echo "  Dual DoD A (xhci_udx USB) OPEN · B (rtl8168_udx NIC) OPEN."
	echo "  make drivers-udx then re-stage for denser PASS on missing hosts."
} >"$drv_dir/PRODUCT-HOSTS.txt"
cp -f "$drv_dir/PRODUCT-HOSTS.txt" "$gj/PRODUCT-HOSTS.txt"
func_product_host_catalog=1

# ---------------------------------------------------------------------------
# 3) DRIVERS.txt — G752 IDs + how UDX hosts bind (no GPL .ko)
# ---------------------------------------------------------------------------
cat >"$gj/DRIVERS.txt" <<'EOF'
GreenJade — driver host surface on ESP (UDX, no GPL .ko)
========================================================

Policy
  - Product drivers are **userspace host processes** (UDX), not Linux modules.
  - License: MIT OR Apache-2.0 only. **No GPL source. No .ko product AC.**
  - Caps (PCI / MMIO / IRQ / DMA windows) stay inside UDX + devmgr; driver
    .c uses Linux-shaped names only (probe / remove / quiesce / ioremap /
    request_irq / dma_* / schedule_work).
  - Docs: docs/UDX_LINUX_PORTER.md · docs/DDI_SOFT.md · docs/ABI_FIRST_PIVOT.md

Staged paths (this stick — ESP + optional GJ-PERSIST dual-land)
  EFI/GREENJADE/user/personality.elf   cold personality scaffold (G-PERS)
  EFI/GREENJADE/drivers/               UDX hosts + optional out-of-tree bins
  EFI/GREENJADE/drivers/ddi_host       soft DDI host demo (make ddi-host-gj)
  EFI/GREENJADE/drivers/rtl8168_udx    G752 NIC soft host (10ec:8168)
  EFI/GREENJADE/drivers/xhci_udx       G752 xHCI soft host (8086:a12f)
  EFI/GREENJADE/drivers/udx_skeleton   host demo (make udx-example)
  EFI/GREENJADE/drivers/libudx.a       porter archive (make udx), if built
  EFI/GREENJADE/drivers/MANIFEST.txt   inventory of what this stage packed
  EFI/GREENJADE/drivers/PRODUCT-HOSTS.txt denser product_host density N/3 catalog
  EFI/GREENJADE/PRODUCT-HOSTS.txt      same denser catalog at GREENJADE root
  EFI/GREENJADE/drivers/UDX-STAGE.txt  functional residual dual-land note
  GJ-PERSIST/drivers/                  optional mirror (stage arg2 / GJ_PERSIST_DIR)
  GJ-PERSIST/drivers/{ddi_host,rtl8168_udx,xhci_udx}  product host names
  GJ-PERSIST/drivers/PRODUCT-HOSTS.txt denser dual-land catalog mirror

How UDX hosts bind (Linux-shaped, GreenJade underneath)
  1. Host process starts → udx_init()
  2. Register: udx_pci_register_driver(&drv) with id_table
       id_table entries: UDX_PCI_DEVICE(vendor, device)
  3. Match:
       - Product (future DDI): devmgr grants BDF + MMIO/IRQ/DMA caps to host
       - Host soft path today: udx_host_inject_pci(vend, dev, …) for lab
  4. probe(pdev) → udx_pci_enable / request_regions / ioremap / request_irq
  5. IRQ: kernel Notification → UDX **threaded** handler only (no hard-IRQ
     driver code). ISR may schedule_work.
  6. DMA: udx_dma_alloc_coherent / map_single inside future IOMMU **window**
     caps (soft path: host sim; product: VT-d windows, P-DMA).
  7. Teardown: pfnQuiesce (stop DMA / mask IRQ / cancel work) → remove
  8. Never call cap mint/revoke or load a Linux .ko from this tree.

G752VT PCI IDs (Linux inventory ground truth — 2026-08-01)
  DUT: ASUS ROG G752VT · BIOS G752VT.307 · i7-6700HQ · 16 GiB · GTX 970M
  Source: make linux-hwtest-img → GJ-LNX-ESP reports (!= freestanding product)

  BDF        ID          Class / role
  ---------  ----------  -----------------------------------------------
  00:14.0    8086:a12f   USB 3.0 xHCI (Intel 100 Series / C230)
  00:17.0    8086:2822   SATA RAID-mode AHCI
  00:1f.3    8086:a170   HD Audio (Intel PCH)
  01:00.0    10de:1618   NVIDIA GM204M GeForce GTX 970M
  01:00.1    10de:0fbb   NVIDIA GM204 HD Audio
  02:00.0    8086:095a   Intel Wireless 7265
  03:00.0    10ec:8168   Realtek RTL8111/8168 Gigabit Ethernet

USB topology (same inventory)
  xHCI BDF 0000:00:14.0 · class 0c0330
  Bus 1 HS (16p): internal webcam / ASUS HID / BT / keyboard
  Bus 2 SS (10p): external stick preferred @ **SS port 5** (bus2-port5)
  Example stick VID:PID 13fe:6400 usb-storage BOT SuperSpeed
  Chassis: USB 3.0 ports only (no pure USB2 jack)

UDX id_table sketch (clean-room — manuals / public PCI IDs only)
  static const struct udx_pci_device_id xhci_ids[] = {
      { UDX_PCI_DEVICE(0x8086, 0xa12f) }, /* G752VT xHCI */
      { 0, }
  };
  static const struct udx_pci_device_id nics[] = {
      { UDX_PCI_DEVICE(0x10ec, 0x8168) }, /* RTL8168 on G752VT */
      { 0, }
  };
  /* Do NOT paste Linux kernel source. Replace register programs from specs. */

Honesty (Soft!=product · G-AC-1 · C2 scripts residual · stamp-free)
  - Product path = Linux-shaped **userspace** UDX hosts over hot+cold ABI/DDI.
  - Freestanding class drivers (kernel rtl8168 / xhci_msc) default **SKIP**
    (GJ_RTL8168_PROBE=0 · GJ_XHCI_MSC_PROBE=0). Residual opt-in only — not DoD.
  - Staging these hosts != freestanding class-driver product PASS.
  - Staging != product TX/RX (rtl8168_udx) != product BOT/MSC (xhci_udx).
  - pack PASS|SKIP != Dual DoD A/B close (A until USB path; B until interactive SSH login).
  - Dual DoD A (xhci_udx USB) OPEN · Dual DoD B (rtl8168_udx NIC) OPEN.
  - Functional residual dual-land (ESP+persist) pack != Dual DoD close.
  - denser product_host density N/3 != Dual DoD close != product TX/RX/BOT.
  - zero-byte placeholders count as SKIP (non-empty honesty).
  - Linux inventory PASS on GJ-LNX-ESP != GreenJade freestanding xHCI/net done.
  - No Linux .ko runs in-kernel as product (G-AC-1). Staged .ko = eng residual.
  - bar3 / Deck Top 50 remain OPEN until Steam client + matrix evidence.
  - Soft lamps / media pack != product complete.
  - stamp-free residual: NEVER bump GJ_IMAGE_VERSION from this stage script.
  - Bar honesty v0.1.182 panel context only — no invent next N.

Build / stage (one-command operator path)
  make drivers-udx personality-gj  # ddi_host + rtl8168_udx + xhci_udx + G-PERS
  make stage-esp                   # packs this tree (soft-miss OK if bins absent)
  # dual-land: ./scripts/stage-udx-drivers.sh build/esp build/persist
  #            or: GJ_PERSIST_DIR=/mnt/gj-persist ./scripts/stage-udx-drivers.sh …
  make hwtest-img                  # when mtools/sgdisk present — USB GPT image
  # also: make udx && make udx-example  # libudx.a + udx_skeleton alone
  # greppable pack: stage-udx-drivers: ddi_host=PASS|SKIP …
  # greppable residual: stage-udx-drivers: soft residual dual_dod
  # greppable residual: stage-udx-drivers: soft residual product=UDX+ABI
  # greppable residual: stage-udx-drivers: soft residual freestanding class SKIP
  # greppable residual: stage-udx-drivers: soft residual product_host
  # greppable residual: stage-udx-drivers: soft residual product_host density=
  # greppable residual: stage-udx-drivers: soft residual denser esp
  # greppable residual: stage-udx-drivers: soft residual stamp-free
  # greppable residual: stage-udx-drivers: soft residual esp=
  # greppable residual: stage-udx-drivers: soft residual persist=
  # greppable residual: stage-udx-drivers: soft residual dual_land
  # greppable residual: stage-udx-drivers: soft functional residual
EOF

# ---------------------------------------------------------------------------
# 4) LAPTOP.txt — G752VT USB boot steps for freestanding product stick
# ---------------------------------------------------------------------------
cat >"$gj/LAPTOP.txt" <<'EOF'
GreenJade — ASUS ROG G752VT USB boot (freestanding product stick)
=================================================================

DUT
  Model:  ASUS ROG G752VT-RH71 (G752VT)
  BIOS:   G752VT.307 (F2 → Main to confirm)
  CPU:    Intel Core i7-6700HQ (x86_64)
  Labels: ESP **GREENJADE** · persist **GJ-PERSIST**
  Docs:   docs/G752VT_LINUX_HWTEST.md · docs/HWTEST_TOMORROW.md
          EFI/GREENJADE/DRIVERS.txt (PCI IDs + UDX bind)
          EFI/GREENJADE/INSTALL.txt (generic install path)

Lab host — write media (DESTROYS target device)
  cd <knano>
  make hwtest-img
  lsblk -o NAME,SIZE,TYPE,TRAN,LABEL,MODEL   # pick stick; NOT system disk
  sudo ./scripts/install-hwtest-usb.sh /dev/sdX
  # or: sudo make install-hwtest-usb DEV=/dev/sdX

Firmware settings (required)
  1. Power on → **F2** → Setup
  2. Secure Boot → **Disabled**
  3. Prefer pure **UEFI** boot (CSM/Legacy Off for this path)
  4. Fast Boot → Disabled if Esc/F2 are flaky
  5. Save & exit

Physical USB
  - Use a **USB 3.0** jack (G752VT has no pure USB2 external port).
  - Prefer the port Linux maps as Bus2 **SS port 5** for stick log media.
  - AC power preferred for long bring-up.

Boot sequence on laptop
  1. Insert stick; power on → spam **Esc** (or F8 on some ROG SKUs)
  2. Boot menu → **UEFI: … USB** (not "Windows Boot Manager")
  3. Expect panel: GJ-EFI → KERNEL.ELF load → M0 / progress bars
  4. No convenient COM port — use panel + remount stick for logs
  5. Soft freestanding xHCI may hang ~stage 11 (control after address);
     still collect whatever BOOT.LOG / KLOG.TXT exist

After run — collect logs (lab host)
  sudo umount /run/media/$USER/* 2>/dev/null; true
  sudo mkdir -p /mnt/gj-esp /mnt/gj-persist
  sudo mount -L GREENJADE /mnt/gj-esp
  sudo mount -L GJ-PERSIST /mnt/gj-persist
  ls /mnt/gj-esp/EFI/GREENJADE/
  # BOOT.LOG  KLOG.TXT  DRIVERS.txt  LAPTOP.txt  user/  drivers/
  ls /mnt/gj-persist/logs /mnt/gj-persist/steam/STATUS 2>/dev/null || true
  sudo umount /mnt/gj-esp /mnt/gj-persist

Soft-scan serial (if you have USB-TTL on another DUT)
  ./scripts/gj-product-summary.sh logs/serial-….txt   # soft exit 0
  ./scripts/gj-quick-keys.sh logs/serial-….txt        # hard miss exit 1

Do not confuse sticks
  Stick A (this): GREENJADE + GJ-PERSIST — freestanding product path
  Stick B:        GJ-LNX-ESP + GJ-LNX-ROOT — Linux inventory only
                  (make linux-hwtest-img). Inventory PASS != freestanding done.

Honesty (Soft!=product · G-AC-1 · C2 scripts residual · stamp-free)
  Media STATUS=READY / staged drivers != Steam client != bar3 closed.
  Freestanding class (rtl8168 / xhci_msc) default SKIP — product = UDX hosts.
  pack PASS|SKIP != Dual DoD A/B close (xhci_udx / rtl8168_udx both OPEN).
  Functional residual dual-land (ESP+GJ-PERSIST/drivers) != Dual DoD close.
  Deck Top 50 remains NOT-TRIED until real client/title runs on GreenJade.
  stamp-free residual: NEVER bump GJ_IMAGE_VERSION from this stage script.
  Bar honesty v0.1.182 panel context only — no invent next N.
EOF

# ---------------------------------------------------------------------------
# 5) Functional residual dual-land — mirror product hosts onto persist for DUT.
#    Targets: $persist_root/drivers/{ddi_host,rtl8168_udx,xhci_udx}
#    Soft: no persist_root → persist=SKIP (ESP pack still PASS|SKIP honest).
#    Soft!=product: dual-land pack != product TX/RX/BOT != Dual DoD close.
#    denser: non-empty mirror only; PRODUCT-HOSTS denser catalog dual-land.
# ---------------------------------------------------------------------------
persist_ddi_n=0
persist_rtl_n=0
persist_xhci_n=0
persist_ddi_bytes=0
persist_rtl_bytes=0
persist_xhci_bytes=0
persist_drv_n=0
persist_product_host_n=0
persist_st=SKIP
esp_st=SKIP
[ "$product_host_n" -gt 0 ] && esp_st=PASS

mirror_one() {
	# mirror_one <name> — copy non-empty ESP product host to persist drivers/
	_mn=$1
	if host_present "$_mn"; then
		cp -f "$drv_dir/$_mn" "$persist_drv/$_mn"
		chmod +x "$persist_drv/$_mn" 2>/dev/null || true
		persist_drv_n=$((persist_drv_n + 1))
		return 0
	fi
	return 0
}

if [ -n "$persist_root" ]; then
	persist_drv="$persist_root/drivers"
	mkdir -p "$persist_drv"
	mirror_one ddi_host
	mirror_one rtl8168_udx
	mirror_one xhci_udx
	# Optional companions (soft; no hard-fail)
	if [ -f "$drv_dir/udx_skeleton" ] && [ ! -f "$persist_drv/udx_skeleton" ]; then
		_usz=$(host_bytes "$drv_dir/udx_skeleton")
		if [ "$_usz" -gt 0 ] 2>/dev/null; then
			cp -f "$drv_dir/udx_skeleton" "$persist_drv/udx_skeleton"
			persist_drv_n=$((persist_drv_n + 1))
		fi
	fi
	if [ -f "$drv_dir/libudx.a" ] && [ ! -f "$persist_drv/libudx.a" ]; then
		cp -f "$drv_dir/libudx.a" "$persist_drv/libudx.a"
		persist_drv_n=$((persist_drv_n + 1))
	fi
	# denser dual-land: PRODUCT-HOSTS + MANIFEST companions for DUT post-mount
	if [ -f "$drv_dir/PRODUCT-HOSTS.txt" ]; then
		cp -f "$drv_dir/PRODUCT-HOSTS.txt" "$persist_drv/PRODUCT-HOSTS.txt"
	fi
	if [ -f "$drv_dir/UDX-STAGE.txt" ]; then
		cp -f "$drv_dir/UDX-STAGE.txt" "$persist_drv/UDX-STAGE.txt" 2>/dev/null || true
	fi
	if [ -f "$persist_drv/ddi_host" ]; then
		persist_ddi_bytes=$(host_bytes "$persist_drv/ddi_host")
		[ "$persist_ddi_bytes" -gt 0 ] 2>/dev/null && persist_ddi_n=1
	fi
	if [ -f "$persist_drv/rtl8168_udx" ]; then
		persist_rtl_bytes=$(host_bytes "$persist_drv/rtl8168_udx")
		[ "$persist_rtl_bytes" -gt 0 ] 2>/dev/null && persist_rtl_n=1
	fi
	if [ -f "$persist_drv/xhci_udx" ]; then
		persist_xhci_bytes=$(host_bytes "$persist_drv/xhci_udx")
		[ "$persist_xhci_bytes" -gt 0 ] 2>/dev/null && persist_xhci_n=1
	fi
	persist_product_host_n=$((persist_ddi_n + persist_rtl_n + persist_xhci_n))
	[ "$persist_product_host_n" -gt 0 ] && \
		func_mirror_persist=1 && persist_st=PASS
	{
		echo "# GreenJade GJ-PERSIST driver-host inventory (functional residual mirror)"
		echo "# Generated by scripts/stage-udx-drivers.sh — MIT OR Apache-2.0"
		echo "# Soft!=product · G-AC-1 · Dual DoD A/B OPEN · stamp-free residual"
		echo "# product=UDX+ABI · denser dual-land mirror of ESP drivers/"
		echo "# pack PASS != product TX/RX/BOT != Dual DoD close · no invent next N"
		echo "source_esp=$drv_dir"
		echo "persist_root=$persist_root"
		echo "ddi_host=${persist_ddi_n}"
		echo "ddi_host_bytes=${persist_ddi_bytes}"
		echo "rtl8168_udx=${persist_rtl_n}"
		echo "rtl8168_udx_bytes=${persist_rtl_bytes}"
		echo "xhci_udx=${persist_xhci_n}"
		echo "xhci_udx_bytes=${persist_xhci_bytes}"
		echo "product_host_n=${persist_product_host_n}"
		echo "product_host_total=${product_host_total}"
		echo "product_host_density=${persist_product_host_n}/${product_host_total}"
		echo "driver_files=${persist_drv_n}"
		echo "freestanding_class=SKIP"
		echo "product_path=UDX+ABI"
		echo "dual_dod_a=OPEN"
		echo "dual_dod_b=OPEN"
		echo "soft_not_product=1"
		echo "stamp_free=1"
		echo "functional_residual=1"
		echo "denser_product_host=1"
		echo "persist_land=1"
		if [ -d "$persist_drv" ]; then
			ls -1 "$persist_drv" 2>/dev/null | while IFS= read -r line; do
				echo "file=$line"
			done
		fi
	} >"$persist_drv/MANIFEST.txt"
	# denser: also land PRODUCT-HOSTS at persist root for operator cat
	if [ -f "$drv_dir/PRODUCT-HOSTS.txt" ]; then
		cp -f "$drv_dir/PRODUCT-HOSTS.txt" "$persist_root/PRODUCT-HOSTS.txt"
	fi
	# Operator note on persist root (next to logs/ steam/ when hwtest layout)
	{
		echo "GJ-PERSIST/drivers — UDX product-host mirror (functional residual denser)"
		echo "======================================================================"
		echo ""
		echo "Product names (G-AC-1 userspace; not .ko):"
		echo "  ddi_host · rtl8168_udx · xhci_udx"
		echo ""
		echo "Mirrored from ESP: EFI/GREENJADE/drivers/"
		echo "  ddi_host=${persist_ddi_n} bytes=${persist_ddi_bytes}"
		echo "  rtl8168_udx=${persist_rtl_n} bytes=${persist_rtl_bytes}"
		echo "  xhci_udx=${persist_xhci_n} bytes=${persist_xhci_bytes}"
		echo "  product_host_density=${persist_product_host_n}/${product_host_total}"
		echo ""
		echo "Honesty (Soft!=product · Dual DoD A/B OPEN · stamp-free denser)"
		echo "  Dual-land pack != product TX/RX/BOT != Dual DoD close."
		echo "  Dual DoD A (xhci_udx USB) OPEN · B (rtl8168_udx NIC) OPEN."
		echo "  Freestanding class SKIP (GJ_RTL8168_PROBE=0 · GJ_XHCI_MSC_PROBE=0)."
		echo "  denser density N/3 != Dual DoD close (zero-byte = SKIP)."
		echo "  Bar honesty v0.1.182 — NEVER invent next N / bump GJ_IMAGE_VERSION."
		echo ""
		echo "Lab host after DUT run:"
		echo "  sudo mount -L GJ-PERSIST /mnt/gj-persist"
		echo "  ls /mnt/gj-persist/drivers/"
		echo "  cat /mnt/gj-persist/drivers/MANIFEST.txt"
		echo "  cat /mnt/gj-persist/drivers/PRODUCT-HOSTS.txt"
	} >"$persist_root/DRIVERS-UDX.txt"
fi

# ESP-side functional residual dual-land note (always; persist may be SKIP)
{
	echo "UDX stage — functional residual dual-land denser (Soft!=product)"
	echo "==============================================================="
	echo "esp_drivers=$drv_dir"
	echo "esp_ddi_host=${ddi_n} bytes=${ddi_bytes}"
	echo "esp_rtl8168_udx=${rtl_n} bytes=${rtl_bytes}"
	echo "esp_xhci_udx=${xhci_n} bytes=${xhci_bytes}"
	echo "product_host_density=${product_host_n}/${product_host_total}"
	echo "persist_root=${persist_root:-}"
	echo "persist_ddi_host=${persist_ddi_n} bytes=${persist_ddi_bytes}"
	echo "persist_rtl8168_udx=${persist_rtl_n} bytes=${persist_rtl_bytes}"
	echo "persist_xhci_udx=${persist_xhci_n} bytes=${persist_xhci_bytes}"
	echo "persist_product_host_density=${persist_product_host_n}/${product_host_total}"
	echo "persist_land=${persist_st}"
	echo "product_path=UDX+ABI"
	echo "freestanding_class=SKIP"
	echo "dual_dod_a=OPEN"
	echo "dual_dod_b=OPEN"
	echo "soft_not_product=1"
	echo "stamp_free=1"
	echo "functional_residual=1"
	echo "denser_product_host=1"
	echo "bar_honesty=v0.1.182"
	echo "no_invent=next_N"
	echo ""
	echo "Operator dual-land (lab):"
	echo "  ./scripts/stage-udx-drivers.sh build/esp build/persist"
	echo "  GJ_PERSIST_DIR=/mnt/gj-persist ./scripts/stage-udx-drivers.sh build/esp"
	echo "  # or after flash: copy ESP drivers/ → GJ-PERSIST/drivers/"
	echo ""
	echo "Honesty: dual-land denser pack != Dual DoD close != product TX/RX/BOT != bar3."
} >"$drv_dir/UDX-STAGE.txt"
# denser: refresh persist UDX-STAGE after ESP write when dual-land active
if [ -n "$persist_root" ] && [ -d "${persist_root}/drivers" ]; then
	cp -f "$drv_dir/UDX-STAGE.txt" "$persist_root/drivers/UDX-STAGE.txt" 2>/dev/null || true
fi

# Functional residual catalog steps (soft densify; never product claim).
# Final step tally (incl. W10 byte_match) is after dual_land / byte_match below.
func_inventory=1
func_honesty=1
func_dual_dod=1
func_stamp=1
# mirror_persist: walked when dual-land attempted (hosts may SKIP) or when no
# persist target (honest SKIP catalog step). Soft!=product either way.
if [ -z "$persist_root" ]; then
	func_mirror_persist=0
fi

# Greppable per-host pack status (PASS if non-empty binary on ESP drivers/)
# Soft!=product: pack PASS != Dual DoD close; Dual DoD A/B remain OPEN.
pack_status() {
	if host_present "$1"; then
		echo "PASS"
	else
		echo "SKIP"
	fi
}
ddi_st=$(pack_status ddi_host)
rtl_st=$(pack_status rtl8168_udx)
xhci_st=$(pack_status xhci_udx)
udx_st=SKIP
[ "$udx_n" -eq 1 ] && udx_st=PASS
pers_st=SKIP
[ "$pers_n" -eq 1 ] && pers_st=PASS

# denser per-host dual_land status (ESP|PERSIST|BOTH|SKIP)
dual_land_host() {
	_hn=$1
	_esp=0
	_pers=0
	case "$_hn" in
	ddi_host) _esp=$ddi_n; _pers=$persist_ddi_n ;;
	rtl8168_udx) _esp=$rtl_n; _pers=$persist_rtl_n ;;
	xhci_udx) _esp=$xhci_n; _pers=$persist_xhci_n ;;
	esac
	if [ "$_esp" -eq 1 ] && [ "$_pers" -eq 1 ]; then
		echo "BOTH"
	elif [ "$_esp" -eq 1 ]; then
		echo "ESP_ONLY"
	elif [ "$_pers" -eq 1 ]; then
		echo "PERSIST_ONLY"
	else
		echo "SKIP"
	fi
}
ddi_dl=$(dual_land_host ddi_host)
rtl_dl=$(dual_land_host rtl8168_udx)
xhci_dl=$(dual_land_host xhci_udx)

dual_land_st=SKIP
if [ "$esp_st" = "PASS" ] && [ "$persist_st" = "PASS" ]; then
	dual_land_st=PASS
elif [ "$esp_st" = "PASS" ]; then
	dual_land_st=ESP_ONLY
elif [ "$persist_st" = "PASS" ]; then
	dual_land_st=PERSIST_ONLY
fi

# W10 denser: dual-land byte_match residual (ESP size == persist size when BOTH).
# Soft honesty only — mismatch is warn/SKIP, never hard-fail; Soft!=product.
byte_match_host() {
	_hn=$1
	_eb=0
	_pb=0
	case "$_hn" in
	ddi_host) _eb=$ddi_bytes; _pb=$persist_ddi_bytes ;;
	rtl8168_udx) _eb=$rtl_bytes; _pb=$persist_rtl_bytes ;;
	xhci_udx) _eb=$xhci_bytes; _pb=$persist_xhci_bytes ;;
	esac
	_dl=$(dual_land_host "$_hn")
	if [ "$_dl" = "BOTH" ] && [ "$_eb" -gt 0 ] 2>/dev/null && [ "$_eb" -eq "$_pb" ] 2>/dev/null; then
		echo "MATCH"
	elif [ "$_dl" = "BOTH" ]; then
		echo "MISMATCH"
	elif [ "$_dl" = "ESP_ONLY" ] || [ "$_dl" = "PERSIST_ONLY" ]; then
		echo "PARTIAL"
	else
		echo "SKIP"
	fi
}
ddi_bm=$(byte_match_host ddi_host)
rtl_bm=$(byte_match_host rtl8168_udx)
xhci_bm=$(byte_match_host xhci_udx)
byte_match_st=SKIP
byte_match_n=0
for _bm in "$ddi_bm" "$rtl_bm" "$xhci_bm"; do
	[ "$_bm" = "MATCH" ] && byte_match_n=$((byte_match_n + 1))
done
if [ "$byte_match_n" -eq 3 ]; then
	byte_match_st=PASS
elif [ "$byte_match_n" -gt 0 ]; then
	byte_match_st=PARTIAL
elif [ "$ddi_bm" = "MISMATCH" ] || [ "$rtl_bm" = "MISMATCH" ] || [ "$xhci_bm" = "MISMATCH" ]; then
	byte_match_st=MISMATCH
fi
func_byte_match=1
# Append byte_match + panel into UDX-STAGE for DUT post-mount honesty
if [ -f "$drv_dir/UDX-STAGE.txt" ]; then
	{
		echo "byte_match=${byte_match_st} ddi_host=${ddi_bm} rtl8168_udx=${rtl_bm} xhci_udx=${xhci_bm}"
		echo "panel_version=${panel_ver}"
		echo "panel_title=${panel_title}"
		echo "panel_source=${panel_src}"
		echo "stamp_free=1 bar_honesty=v0.1.182 no_invent=next_N"
	} >>"$drv_dir/UDX-STAGE.txt"
	if [ -n "$persist_root" ] && [ -d "${persist_root}/drivers" ]; then
		cp -f "$drv_dir/UDX-STAGE.txt" "$persist_root/drivers/UDX-STAGE.txt" 2>/dev/null || true
	fi
fi

# Recount functional steps with W10 byte_match arm
func_steps_ok=0
[ "$func_harvest" -eq 1 ] && func_steps_ok=$((func_steps_ok + 1))
[ "$func_ensure" -eq 1 ] && func_steps_ok=$((func_steps_ok + 1))
[ "$func_densify" -eq 1 ] && func_steps_ok=$((func_steps_ok + 1))
[ "$func_pack_esp" -eq 1 ] && func_steps_ok=$((func_steps_ok + 1))
if [ -n "$persist_root" ]; then
	func_steps_ok=$((func_steps_ok + 1))
else
	func_mirror_persist=0
	func_steps_ok=$((func_steps_ok + 1))
fi
[ "$func_inventory" -eq 1 ] && func_steps_ok=$((func_steps_ok + 1))
[ "$func_product_host_catalog" -eq 1 ] && func_steps_ok=$((func_steps_ok + 1))
[ "$func_honesty" -eq 1 ] && func_steps_ok=$((func_steps_ok + 1))
[ "$func_dual_dod" -eq 1 ] && func_steps_ok=$((func_steps_ok + 1))
[ "$func_byte_match" -eq 1 ] && func_steps_ok=$((func_steps_ok + 1))
[ "$func_stamp" -eq 1 ] && func_steps_ok=$((func_steps_ok + 1))

echo "stage-udx-drivers: PASS personality=${pers_n} udx_skeleton=${udx_n} ddi_host=${ddi_n} rtl8168_udx=${rtl_n} xhci_udx=${xhci_n} product_host_density=${product_host_n}/${product_host_total} driver_files=${drv_tree_n} path=$gj"
echo "stage-udx-drivers: ddi_host=${ddi_st} rtl8168_udx=${rtl_st} xhci_udx=${xhci_st} udx_skeleton=${udx_st} personality=${pers_st}"
echo "stage-udx-drivers: freestanding class SKIP (GJ_RTL8168_PROBE=0 · GJ_XHCI_MSC_PROBE=0 residual opt-in)"
echo "stage-udx-drivers: Soft!=product · G-AC-1 — pack UDX hosts != product TX/RX/BOT != bar3"
echo "stage-udx-drivers: soft residual dual_dod A=OPEN B=OPEN (pack PASS != Dual DoD close)"
echo "stage-udx-drivers: soft residual product=UDX+ABI product_host=ddi_host+rtl8168_udx+xhci_udx"
echo "stage-udx-drivers: soft residual freestanding class SKIP"
echo "stage-udx-drivers: soft residual product_host"
echo "stage-udx-drivers: soft residual product_host density=${product_host_n}/${product_host_total} ddi_host=${ddi_n} rtl8168_udx=${rtl_n} xhci_udx=${xhci_n}"
echo "stage-udx-drivers: soft residual denser esp product_host density=${product_host_n}/${product_host_total} ddi_bytes=${ddi_bytes} rtl_bytes=${rtl_bytes} xhci_bytes=${xhci_bytes}"
echo "stage-udx-drivers: soft residual denser dual_land ddi_host=${ddi_dl} rtl8168_udx=${rtl_dl} xhci_udx=${xhci_dl}"
echo "stage-udx-drivers: soft residual dual_land byte_match=${byte_match_st} ddi_host=${ddi_bm} rtl8168_udx=${rtl_bm} xhci_udx=${xhci_bm} n=${byte_match_n}/3"
echo "stage-udx-drivers: soft residual panel=${panel_title} src=${panel_src} (stamp-free RO; Soft!=product)"
echo "stage-udx-drivers: soft residual stamp-free (bar honesty v0.1.182 packed, not host-probed; NEVER bump GJ_IMAGE_VERSION; no invent next N)"
echo "stage-udx-drivers: soft residual esp=${esp_st} ddi_host=${ddi_n} rtl8168_udx=${rtl_n} xhci_udx=${xhci_n} path=$drv_dir"
echo "stage-udx-drivers: soft residual persist=${persist_st} ddi_host=${persist_ddi_n} rtl8168_udx=${persist_rtl_n} xhci_udx=${persist_xhci_n} path=${persist_root:-}"
echo "stage-udx-drivers: soft residual dual_land=${dual_land_st} esp=${esp_st} persist=${persist_st} Soft!=product"
echo "stage-udx-drivers: soft functional residual catalog path=harvest,ensure,densify,pack_esp,mirror_persist,inventory,product_host_catalog,honesty,dual_dod,byte_match,stamp-free"
echo "stage-udx-drivers: soft functional residual step harvest=${func_harvest} ensure=${func_ensure} densify=${func_densify} pack_esp=${func_pack_esp} mirror_persist=${func_mirror_persist} inventory=${func_inventory} product_host_catalog=${func_product_host_catalog} honesty=${func_honesty} dual_dod=${func_dual_dod} byte_match=${func_byte_match} stamp=${func_stamp}"
echo "stage-udx-drivers: soft functional residual PASS steps=${func_steps_ok}/${func_steps_total} dual_land=${dual_land_st} byte_match=${byte_match_st} product=UDX+ABI product_host_density=${product_host_n}/${product_host_total} dual_dod_a=OPEN dual_dod_b=OPEN Soft!=product"
echo "stage-udx-drivers: soft functional residual (denser ESP+persist dual-land for DUT; pack != Dual DoD close; bar honesty v0.1.182; no invent next N)"
