#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Generate kernel/include/gj/linux_pci_hostish_off.h — host-oracle offsets for
# RHEL-class struct pci_dev / device / resource (soft module-path only).
#
# Purpose
# -------
# Real host-built .ko (e.g. r8169) inline pci_resource_* / pci_set_drvdata /
# dma_set_mask against Linux field offsets. GreenJade freestanding soft
# pci_dev is a compact clean-room shape (Soft≠ABI). Strategy A uses a separate
# hostish probe blob; this header is the offset table for one lab kver.
#
# Law / honesty
# -------------
#   - Dual MIT OR Apache-2.0. No GPL *source* is committed to the GreenJade tree.
#   - This script only *uses* system kernel-devel headers on the lab host
#     (out-of-tree throwaway module under /tmp). MODULE_LICENSE("GPL") is
#     required by the host kbuild for a loadable .ko; we never insmod, and
#     the generated C is discarded after extract.
#   - Output header is dual-license clean-room *numbers* (offsets), not Linux
#     source. Soft ≠ ABI-stable product; Soft ≠ G-AC-1 bar3 claim.
#   - Re-run when staged r8169.ko kver changes (CONFIG / RH_KABI shift offsets).
#
# Approach (lab host with kernel-devel)
# -------------------------------------
# Mirrors /tmp/gj_off_mod/off2.c: build a tiny OOT module whose .rodata holds
#   const unsigned long gj_off[] = { offsetof(...), sizeof(...), ... };
# Extract the array from the object (no insmod). Emit #defines.
#
# Usage
# -----
#   ./scripts/gen-linux-pci-hostish-off.sh
#   ./scripts/gen-linux-pci-hostish-off.sh /path/to/out.h
#
# Env
# ---
#   GJ_LINUX_KVER     — uname -r override (default: running kernel)
#   GJ_LINUX_KBUILD   — path to kernel build tree
#                       (default: /lib/modules/$KVER/build)
#   GJ_HOSTISH_WORKDIR — scratch dir (default: /tmp/gj_hostish_off_$$)
#   GJ_HOSTISH_KEEP   — if non-empty, do not rm workdir
#
# Greppable
# ---------
#   gen-linux-pci-hostish-off: PASS|FAIL|SKIP
#
# Consumers (parent / freestanding fill — not this script)
# -------------------------------------------------------
#   kernel/include/gj/linux_pci_hostish_off.h
#   docs/PCI_DEV_SOFT_LAYOUT.md  (Strategy A hostish blob)
#
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"

out="${1:-$root/kernel/include/gj/linux_pci_hostish_off.h}"
kver="${GJ_LINUX_KVER:-$(uname -r 2>/dev/null || echo unknown)}"
kbuild="${GJ_LINUX_KBUILD:-/lib/modules/$kver/build}"
workdir="${GJ_HOSTISH_WORKDIR:-/tmp/gj_hostish_off_$$}"
date_stamp="$(date -u +%Y-%m-%d 2>/dev/null || echo unknown)"

die() {
	echo "gen-linux-pci-hostish-off: FAIL: $*" >&2
	exit 1
}

cleanup() {
	if [ -z "${GJ_HOSTISH_KEEP:-}" ] && [ -d "$workdir" ]; then
		rm -rf "$workdir"
	fi
}
trap cleanup EXIT INT HUP TERM

if [ ! -d "$kbuild" ]; then
	die "kernel build tree missing: $kbuild (install kernel-devel for $kver)"
fi
if [ ! -f "$kbuild/Makefile" ] && [ ! -f "$kbuild/Makefile.rhelver" ]; then
	# RHEL often has Makefile; accept either
	if [ ! -e "$kbuild/Module.symvers" ]; then
		die "unusable kbuild at $kbuild"
	fi
fi
if ! command -v make >/dev/null 2>&1; then
	die "make not found"
fi

mkdir -p "$workdir"
mkdir -p "$(dirname "$out")"

# Throwaway OOT module source — lives only under /tmp, never committed.
# Field list order must stay in lockstep with the shell assign + header emit below.
cat >"$workdir/off2.c" <<'EOF'
/* Throwaway lab oracle — not GreenJade product source. Never commit. */
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/device.h>
#include <linux/ioport.h>

/*
 * Fixed order — must match extract/assign order in gen-linux-pci-hostish-off.sh.
 * resource[0]/resource[1]/resource[2] are absolute pci_dev offsets.
 */
const unsigned long gj_off[] = {
	offsetof(struct pci_dev, bus_list),
	offsetof(struct pci_dev, bus),
	offsetof(struct pci_dev, devfn),
	offsetof(struct pci_dev, vendor),
	offsetof(struct pci_dev, device),
	offsetof(struct pci_dev, subsystem_vendor),
	offsetof(struct pci_dev, subsystem_device),
	offsetof(struct pci_dev, class),
	offsetof(struct pci_dev, revision),
	offsetof(struct pci_dev, driver),
	offsetof(struct pci_dev, dma_mask),
	offsetof(struct pci_dev, dev),
	offsetof(struct pci_dev, irq),
	offsetof(struct pci_dev, resource),
	offsetof(struct pci_dev, enable_cnt),
	sizeof(struct pci_dev),
	sizeof(struct device),
	sizeof(struct resource),
	offsetof(struct device, parent),
	offsetof(struct device, driver_data),
	offsetof(struct device, dma_mask),
	offsetof(struct device, coherent_dma_mask),
	offsetof(struct resource, start),
	offsetof(struct resource, end),
	offsetof(struct resource, name),
	offsetof(struct resource, flags),
	offsetof(struct pci_dev, resource[0]),
	offsetof(struct pci_dev, resource[1]),
	offsetof(struct pci_dev, resource[2]),
};

static int __init gj_hostish_off_init(void)
{
	/* Refuse load — we only need the object file for offsetof extract. */
	return -EINVAL;
}

static void __exit gj_hostish_off_exit(void)
{
}

module_init(gj_hostish_off_init);
module_exit(gj_hostish_off_exit);
/* Host kbuild requires a license tag for .ko; source is not committed. */
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("GreenJade throwaway pci_dev offsetof oracle (do not load)");
MODULE_AUTHOR("GreenJade lab");
EOF

cat >"$workdir/Makefile" <<EOF
obj-m += off2.o
EOF

echo "gen-linux-pci-hostish-off: building off2.o against $kbuild (kver=$kver)"
if ! make -C "$kbuild" M="$workdir" modules 2>"$workdir/make.err"; then
	echo "gen-linux-pci-hostish-off: make log (tail):" >&2
	tail -n 40 "$workdir/make.err" >&2 || true
	die "out-of-tree module build failed"
fi

obj="$workdir/off2.o"
if [ ! -f "$obj" ]; then
	die "missing $obj after build"
fi

# Extract gj_off[] (29 × unsigned long LE) from the relocatable object.
# Prefer Python3 (stdlib only); fall back to readelf -x + awk/od.
extract_py="$workdir/extract_gj_off.py"
cat >"$extract_py" <<'PY'
#!/usr/bin/env python3
"""Extract symbol gj_off (array of unsigned long) from a relocatable ELF .o."""
import struct
import sys

def u16(b, o):
    return struct.unpack_from("<H", b, o)[0]

def u32(b, o):
    return struct.unpack_from("<I", b, o)[0]

def u64(b, o):
    return struct.unpack_from("<Q", b, o)[0]

def main():
    path = sys.argv[1]
    expect = int(sys.argv[2]) if len(sys.argv) > 2 else 29
    data = open(path, "rb").read()
    if data[:4] != b"\x7fELF":
        sys.stderr.write("not ELF\n")
        return 1
    ei_class = data[4]
    if ei_class != 2:
        sys.stderr.write("need ELF64\n")
        return 1
    e_shoff = u64(data, 40)
    e_shentsize = u16(data, 58)
    e_shnum = u16(data, 60)
    e_shstrndx = u16(data, 62)

    def shdr(i):
        o = e_shoff + i * e_shentsize
        return {
            "name": u32(data, o + 0),
            "type": u32(data, o + 4),
            "flags": u64(data, o + 8),
            "addr": u64(data, o + 16),
            "offset": u64(data, o + 24),
            "size": u64(data, o + 32),
            "link": u32(data, o + 40),
            "info": u32(data, o + 44),
            "addralign": u64(data, o + 48),
            "entsize": u64(data, o + 56),
        }

    shstr = shdr(e_shstrndx)
    shstrtab = data[shstr["offset"] : shstr["offset"] + shstr["size"]]

    def sec_name(sh):
        o = sh["name"]
        end = shstrtab.find(b"\0", o)
        return shstrtab[o:end].decode("ascii", "replace")

    sections = [shdr(i) for i in range(e_shnum)]
    symtab = None
    strtab = None
    for sh in sections:
        n = sec_name(sh)
        if n == ".symtab":
            symtab = sh
        elif n == ".strtab":
            strtab = sh
    if symtab is None or strtab is None:
        sys.stderr.write("missing .symtab/.strtab\n")
        return 1
    strings = data[strtab["offset"] : strtab["offset"] + strtab["size"]]
    entsz = symtab["entsize"] or 24
    found = None
    for i in range(symtab["size"] // entsz):
        o = symtab["offset"] + i * entsz
        st_name = u32(data, o)
        st_info = data[o + 4]
        st_shndx = u16(data, o + 6)
        st_value = u64(data, o + 8)
        st_size = u64(data, o + 16)
        end = strings.find(b"\0", st_name)
        name = strings[st_name:end].decode("ascii", "replace")
        if name == "gj_off":
            found = (st_shndx, st_value, st_size)
            break
    if found is None:
        sys.stderr.write("symbol gj_off not found\n")
        return 1
    shndx, value, size = found
    if shndx == 0 or shndx >= e_shnum:
        sys.stderr.write("bad shndx for gj_off\n")
        return 1
    sec = sections[shndx]
    file_off = sec["offset"] + value
    nbytes = size if size else expect * 8
    if nbytes < expect * 8:
        sys.stderr.write("gj_off too small: %d\n" % nbytes)
        return 1
    blob = data[file_off : file_off + expect * 8]
    vals = list(struct.unpack("<%dQ" % expect, blob))
    for v in vals:
        print(v)
    return 0

if __name__ == "__main__":
    sys.exit(main())
PY

N_OFF=29
vals_file="$workdir/vals.txt"
if command -v python3 >/dev/null 2>&1; then
	if ! python3 "$extract_py" "$obj" "$N_OFF" >"$vals_file"; then
		die "python extract of gj_off failed"
	fi
else
	# Fallback: nm + readelf hex dump of the section containing gj_off
	if ! command -v nm >/dev/null 2>&1 || ! command -v readelf >/dev/null 2>&1; then
		die "need python3 or nm+readelf to extract offsets"
	fi
	# Parse with od from the .o file offset via readelf -s / -S
	die "python3 required for robust extract (install python3)"
fi

c_lines=$(wc -l <"$vals_file" | tr -d ' ')
if [ "$c_lines" -lt "$N_OFF" ]; then
	die "expected $N_OFF values, got $c_lines"
fi

# shellcheck disable=SC2046
set -- $(cat "$vals_file")
off_bus_list=$1
off_bus=$2
off_devfn=$3
off_vendor=$4
off_device=$5
off_subsys_vend=$6
off_subsys_dev=$7
off_class=$8
off_revision=$9
off_driver=${10}
off_dma_mask=${11}
off_dev=${12}
off_irq=${13}
off_resource=${14}
off_enable_cnt=${15}
size_pci_dev=${16}
size_device=${17}
size_resource=${18}
dev_off_parent=${19}
dev_off_driver_data=${20}
dev_off_dma_mask=${21}
dev_off_coherent=${22}
res_off_start=${23}
res_off_end=${24}
res_off_name=${25}
res_off_flags=${26}
off_res0=${27}
off_res1=${28}
off_res2=${29}

hexu() {
	# print unsigned as 0xNNNNu (at least 4 hex digits after 0x when small)
	printf '0x%04xu' "$1"
}

# Sanity: early prefix matches RHEL 5.14 docs (Soft≠ABI; fail loud if wild).
if [ "$off_bus_list" != "0" ]; then
	die "unexpected bus_list offset $off_bus_list (want 0)"
fi
if [ "$off_vendor" != "60" ]; then
	# 0x3c — warn but still emit (other kvers may differ)
	echo "gen-linux-pci-hostish-off: note: vendor offset=$off_vendor (RHEL 5.14-class often 60/0x3c)" >&2
fi
if [ "$off_resource" != "$off_res0" ]; then
	die "resource base $off_resource != resource[0] $off_res0"
fi
if [ "$size_resource" -eq 0 ]; then
	die "sizeof(struct resource) is 0"
fi
if [ "$size_pci_dev" -lt 256 ]; then
	die "sizeof(pci_dev)=$size_pci_dev looks too small"
fi

# Blob pad: round sizeof(pci_dev) up to next 256 and ensure ≥ size+64
blob_bytes=$size_pci_dev
# shell arithmetic: next multiple of 256 if not already, else +256 for RH tail
rem=$((blob_bytes % 256))
if [ "$rem" -ne 0 ]; then
	blob_bytes=$((blob_bytes + (256 - rem)))
else
	blob_bytes=$((blob_bytes + 256))
fi
if [ "$blob_bytes" -lt $((size_pci_dev + 64)) ]; then
	blob_bytes=$((size_pci_dev + 256))
fi

{
	cat <<EOF
/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Host-oracle offsets for RHEL-class Linux struct pci_dev / device / resource.
 * Used only for soft module-path experiments (r8169.ko probe-shaped blob).
 * Soft ≠ ABI-stable product. Soft ≠ product AC (G-AC-1).
 * Do not claim layout matches any other kver / CONFIG / RH_KABI rebuild.
 *
 * Regenerated on a lab host with kernel-devel via:
 *   ./scripts/gen-linux-pci-hostish-off.sh
 *   GJ_LINUX_KVER=... GJ_LINUX_KBUILD=... ./scripts/gen-linux-pci-hostish-off.sh
 *
 * Measured: kver=${kver}
 * Date (UTC): ${date_stamp}
 * kbuild: ${kbuild}
 *
 * No Linux GPL source is in this header — only dual-license offset #defines
 * extracted via offsetof/sizeof from system headers on the build host.
 * See docs/PCI_DEV_SOFT_LAYOUT.md (Strategy A hostish blob).
 */
#pragma once

/* ---- sizeof ----------------------------------------------------------- */
#define LINUX_PCI_HOSTISH_SIZE_PCI_DEV     $(hexu "$size_pci_dev") /* ${size_pci_dev} */
#define LINUX_PCI_HOSTISH_SIZE_DEVICE      $(hexu "$size_device") /* ${size_device} */
#define LINUX_PCI_HOSTISH_SIZE_RESOURCE    $(hexu "$size_resource") /* ${size_resource} */

/* ---- struct pci_dev field offsets ------------------------------------- */
#define LINUX_PCI_HOSTISH_OFF_BUS_LIST     $(hexu "$off_bus_list")
#define LINUX_PCI_HOSTISH_OFF_BUS          $(hexu "$off_bus")
#define LINUX_PCI_HOSTISH_OFF_DEVFN        $(hexu "$off_devfn")
#define LINUX_PCI_HOSTISH_OFF_VENDOR       $(hexu "$off_vendor")
#define LINUX_PCI_HOSTISH_OFF_DEVICE       $(hexu "$off_device")
#define LINUX_PCI_HOSTISH_OFF_SUBSYS_VEND  $(hexu "$off_subsys_vend")
#define LINUX_PCI_HOSTISH_OFF_SUBSYS_DEV   $(hexu "$off_subsys_dev")
#define LINUX_PCI_HOSTISH_OFF_CLASS        $(hexu "$off_class")
#define LINUX_PCI_HOSTISH_OFF_REVISION     $(hexu "$off_revision")
#define LINUX_PCI_HOSTISH_OFF_DRIVER       $(hexu "$off_driver")
#define LINUX_PCI_HOSTISH_OFF_DMA_MASK     $(hexu "$off_dma_mask")
#define LINUX_PCI_HOSTISH_OFF_DEV          $(hexu "$off_dev") /* embedded struct device */
#define LINUX_PCI_HOSTISH_OFF_IRQ          $(hexu "$off_irq")
#define LINUX_PCI_HOSTISH_OFF_RESOURCE     $(hexu "$off_resource") /* resource[0] */
#define LINUX_PCI_HOSTISH_OFF_ENABLE_CNT   $(hexu "$off_enable_cnt")

/* Absolute offsets of resource[0..2] (BAR0..BAR2) */
#define LINUX_PCI_HOSTISH_OFF_RESOURCE_0   $(hexu "$off_res0")
#define LINUX_PCI_HOSTISH_OFF_RESOURCE_1   $(hexu "$off_res1")
#define LINUX_PCI_HOSTISH_OFF_RESOURCE_2   $(hexu "$off_res2")

/* resource[n] = RESOURCE + n * SIZE_RESOURCE */
#define LINUX_PCI_HOSTISH_OFF_RESOURCE_N(n) \\
    (LINUX_PCI_HOSTISH_OFF_RESOURCE + (unsigned)(n) * LINUX_PCI_HOSTISH_SIZE_RESOURCE)

/* ---- struct device relative to pdev->dev base (add OFF_DEV) ----------- */
#define LINUX_PCI_HOSTISH_DEV_OFF_PARENT            $(hexu "$dev_off_parent")
#define LINUX_PCI_HOSTISH_DEV_OFF_DRIVER_DATA       $(hexu "$dev_off_driver_data")
#define LINUX_PCI_HOSTISH_DEV_OFF_DMA_MASK          $(hexu "$dev_off_dma_mask") /* u64 * */
#define LINUX_PCI_HOSTISH_DEV_OFF_COHERENT_DMA_MASK $(hexu "$dev_off_coherent") /* u64 value */

/* Absolute pci_dev offsets for common device fields */
#define LINUX_PCI_HOSTISH_OFF_DEV_PARENT \\
    (LINUX_PCI_HOSTISH_OFF_DEV + LINUX_PCI_HOSTISH_DEV_OFF_PARENT)
#define LINUX_PCI_HOSTISH_OFF_DEV_DRIVER_DATA \\
    (LINUX_PCI_HOSTISH_OFF_DEV + LINUX_PCI_HOSTISH_DEV_OFF_DRIVER_DATA)
#define LINUX_PCI_HOSTISH_OFF_DEV_DMA_MASK \\
    (LINUX_PCI_HOSTISH_OFF_DEV + LINUX_PCI_HOSTISH_DEV_OFF_DMA_MASK)
#define LINUX_PCI_HOSTISH_OFF_DEV_COHERENT_DMA_MASK \\
    (LINUX_PCI_HOSTISH_OFF_DEV + LINUX_PCI_HOSTISH_DEV_OFF_COHERENT_DMA_MASK)

/* ---- struct resource relative to resource[n] base --------------------- */
#define LINUX_PCI_HOSTISH_RES_OFF_START  $(hexu "$res_off_start")
#define LINUX_PCI_HOSTISH_RES_OFF_END    $(hexu "$res_off_end")
#define LINUX_PCI_HOSTISH_RES_OFF_NAME   $(hexu "$res_off_name")
#define LINUX_PCI_HOSTISH_RES_OFF_FLAGS  $(hexu "$res_off_flags")

/* Soft pool / blob sizing for freestanding fill (parent implements fill). */
#define LINUX_PCI_HOSTISH_POOL           2u
#define LINUX_PCI_HOSTISH_BLOB_BYTES     $(hexu "$blob_bytes") /* ≥ sizeof(pci_dev) */

/* IORESOURCE_* low bits (host-compatible; soft fill may set these). */
#define LINUX_PCI_HOSTISH_IORESOURCE_IO   0x00000100ul
#define LINUX_PCI_HOSTISH_IORESOURCE_MEM  0x00000200ul

/*
 * Usage (freestanding hostish fill — not implemented here):
 *
 *   u8 *p = hostish_blob;
 *   memset(p, 0, LINUX_PCI_HOSTISH_BLOB_BYTES);
 *   *(u16 *)(p + LINUX_PCI_HOSTISH_OFF_VENDOR) = 0x10ec;
 *   *(u16 *)(p + LINUX_PCI_HOSTISH_OFF_DEVICE) = 0x8168;
 *   *(u32 *)(p + LINUX_PCI_HOSTISH_OFF_DEVFN)  = devfn;
 *   *(u64 *)(p + LINUX_PCI_HOSTISH_OFF_RESOURCE_0
 *              + LINUX_PCI_HOSTISH_RES_OFF_START) = bar0_phys;
 *   *(u64 *)(p + LINUX_PCI_HOSTISH_OFF_RESOURCE_0
 *              + LINUX_PCI_HOSTISH_RES_OFF_END)   = bar0_phys + bar0_len - 1;
 *   *(unsigned long *)(p + LINUX_PCI_HOSTISH_OFF_RESOURCE_0
 *              + LINUX_PCI_HOSTISH_RES_OFF_FLAGS) = LINUX_PCI_HOSTISH_IORESOURCE_MEM;
 *   *(void **)(p + LINUX_PCI_HOSTISH_OFF_DEV_DRIVER_DATA) = soft_priv;
 *   *(u64 **)(p + LINUX_PCI_HOSTISH_OFF_DEV_DMA_MASK) =
 *        (u64 *)(p + LINUX_PCI_HOSTISH_OFF_DMA_MASK);
 *
 * Gate real .ko probe behind an explicit flag; default remains soft EMU.
 */
EOF
} >"$out"

echo "gen-linux-pci-hostish-off: PASS wrote $out"
echo "gen-linux-pci-hostish-off: kver=$kver sizeof(pci_dev)=$size_pci_dev sizeof(device)=$size_device sizeof(resource)=$size_resource"
echo "gen-linux-pci-hostish-off: vendor=$(hexu "$off_vendor") dev=$(hexu "$off_dev") resource=$(hexu "$off_resource") enable_cnt=$(hexu "$off_enable_cnt")"

exit 0
