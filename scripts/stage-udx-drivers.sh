#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Stage Linux-shaped driver-host artifacts onto a GreenJade ESP tree:
#   EFI/GREENJADE/user/personality.elf   (G-PERS cold personality, if built)
#   EFI/GREENJADE/drivers/*              (UDX hosts + optional out-of-tree hosts)
#   EFI/GREENJADE/DRIVERS.txt            (G752 PCI IDs + UDX bind model)
#   EFI/GREENJADE/LAPTOP.txt             (G752VT USB boot steps)
#
# Usage:
#   ./scripts/stage-udx-drivers.sh [esp_root]
# Default esp_root: build/esp
#
# Called from stage-esp.sh after freestanding userland ELFs. Soft-only:
# missing personality / drivers / udx_skeleton does not fail the stage.
# No GPL .ko — UDX userspace hosts only (MIT OR Apache-2.0).
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
out="${1:-build/esp}"
gj="$out/EFI/GREENJADE"
drv_dir="$gj/drivers"
user_dir="$gj/user"

mkdir -p "$user_dir" "$drv_dir"

drv_n=0
pers_n=0
udx_n=0

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
		# Common out-of-tree layouts
		for cand in \
			"$d/build/$dname" \
			"$d/build/${dname}_host" \
			"$d/build/${dname}.elf" \
			"$d/build/udx_${dname}" \
			"$d/$dname" \
			"$d/${dname}.elf"; do
			if [ -f "$cand" ]; then
				stage_one "$cand" "$(basename "$cand")"
			fi
		done
		if [ -d "$d/build" ]; then
			for g in "$d/build"/*; do
				[ -f "$g" ] || continue
				bn=$(basename "$g")
				case "$bn" in
				*.o|*.d|*.a|*.c|*.h) continue ;;
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
#     Soft: missing bins do not fail the stage.
# ---------------------------------------------------------------------------
ensure_drv() {
	# ensure_drv <dest_name> <src1> [src2…]
	_ename=$1
	shift
	if [ -f "$drv_dir/$_ename" ]; then
		return 0
	fi
	for _esrc in "$@"; do
		if [ -f "$_esrc" ]; then
			stage_one "$_esrc" "$_ename"
			return 0
		fi
	done
	return 0
}

# Soft DDI host demo (product ESP name for udx_skeleton soft path)
ensure_drv ddi_host \
	build/user/drivers/ddi_host \
	build/udx_skeleton \
	user/udx/build/udx_skeleton \
	"$drv_dir/udx_skeleton"

# G752 class soft hosts (RTL8168 NIC + Intel xHCI)
ensure_drv rtl8168_udx \
	build/user/drivers/rtl8168_udx \
	user/drivers/rtl8168_udx/build/rtl8168_udx

ensure_drv xhci_udx \
	build/user/drivers/xhci_udx \
	user/drivers/xhci_udx/build/xhci_udx

# Optional: ship libudx.a for lab re-link of out-of-tree hosts (not a .ko)
if [ -f build/libudx.a ]; then
	cp -f build/libudx.a "$drv_dir/libudx.a"
	drv_n=$((drv_n + 1))
elif [ -f user/udx/build/libudx.a ]; then
	cp -f user/udx/build/libudx.a "$drv_dir/libudx.a"
	drv_n=$((drv_n + 1))
fi

# Manifest of what landed (counts final tree after ensure_drv)
ddi_n=0
rtl_n=0
xhci_n=0
[ -f "$drv_dir/ddi_host" ] && ddi_n=1
[ -f "$drv_dir/rtl8168_udx" ] && rtl_n=1
[ -f "$drv_dir/xhci_udx" ] && xhci_n=1
{
	echo "# GreenJade ESP driver-host inventory (staged)"
	echo "# Generated by scripts/stage-udx-drivers.sh — MIT OR Apache-2.0"
	echo "personality=${pers_n}"
	echo "udx_skeleton=${udx_n}"
	echo "ddi_host=${ddi_n}"
	echo "rtl8168_udx=${rtl_n}"
	echo "xhci_udx=${xhci_n}"
	echo "driver_files=${drv_n}"
	if [ -d "$drv_dir" ]; then
		ls -1 "$drv_dir" 2>/dev/null | while IFS= read -r line; do
			echo "file=$line"
		done
	fi
} >"$drv_dir/MANIFEST.txt"

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

Staged paths (this stick)
  EFI/GREENJADE/user/personality.elf   cold personality scaffold (G-PERS)
  EFI/GREENJADE/drivers/               UDX hosts + optional out-of-tree bins
  EFI/GREENJADE/drivers/ddi_host       soft DDI host demo (make ddi-host-gj)
  EFI/GREENJADE/drivers/rtl8168_udx    G752 NIC soft host (10ec:8168)
  EFI/GREENJADE/drivers/xhci_udx       G752 xHCI soft host (8086:a12f)
  EFI/GREENJADE/drivers/udx_skeleton   host demo (make udx-example)
  EFI/GREENJADE/drivers/libudx.a       porter archive (make udx), if built
  EFI/GREENJADE/drivers/MANIFEST.txt   inventory of what this stage packed

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
  Source: make linux-hwtest-img → GJ-LNX-ESP reports (≠ freestanding product)

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

Honesty
  - Staging these hosts ≠ freestanding class-driver product PASS.
  - Linux inventory PASS on GJ-LNX-ESP ≠ GreenJade freestanding xHCI/net done.
  - bar3 / Deck Top 50 remain OPEN until Steam client + matrix evidence.
  - In-kernel xhci_msc / rtl8168 are soft scaffolds only (not T1 product AC).

Build / stage (one-command operator path)
  make drivers-udx personality-gj  # ddi_host + rtl8168_udx + xhci_udx + G-PERS
  make stage-esp                   # packs this tree (soft-miss OK if bins absent)
  make hwtest-img                  # when mtools/sgdisk present — USB GPT image
  # also: make udx && make udx-example  # libudx.a + udx_skeleton alone
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
                  (make linux-hwtest-img). Inventory PASS ≠ freestanding done.

Honesty
  Media STATUS=READY / staged drivers ≠ Steam client ≠ bar3 closed.
  Deck Top 50 remains NOT-TRIED until real client/title runs on GreenJade.
EOF

echo "stage-udx-drivers: PASS personality=${pers_n} udx_skeleton=${udx_n} ddi_host=${ddi_n} rtl8168_udx=${rtl_n} xhci_udx=${xhci_n} driver_files=${drv_n} path=$gj"
