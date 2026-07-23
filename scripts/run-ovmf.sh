#!/bin/sh
# SPDX-License-Identifier: MIT
#
# Boot GreenJade.efi under QEMU + OVMF (product firmware path).
# Builds a GPT disk with an EFI System Partition containing BOOTX64.EFI.
# Expect serial: GJ-EFI  (and soft companions: KERNEL.ELF loaded, ExitBootServices,
# source=UEFI, M0 OK when Multiboot kernel is staged).
#
# Soft helpers (optional paths — never hard-fail the boot when extras miss):
#   soft_dd_zero     create sparse/raw disk images with fallback bs=
#   soft_pick_qemu   first usable emulator from a short candidate list
#   soft_have_dev    probe whether QEMU knows a -device model
#   soft_stage_esp   copy optional product files onto ESP when present
#   KERNEL.ELF       staged only if build/greenjade.elf exists
#   user ELFs        staged only if build/esp or build/user trees exist
#   IOMMU_ARGS       only when GJ_INTEL_IOMMU=1 and intel-iommu model exists
#
# These helpers keep OVMF smoke (scripts/smoke-all.sh) green on hosts that
# split QEMU packages or lack a Multiboot kernel build. Do not turn optional
# ESP contents or devices into hard requirements here.
#
# Env:
#   QEMU_BIN            override emulator path
#   OVMF_CODE           override OVMF code FD
#   OVMF_VARS_SRC       override OVMF vars template FD
#   GJ_SMP              vCPUs (default 1)
#   GJ_MEM              RAM size (default 512M — firmware path is light)
#   GJ_INTEL_IOMMU=1    attach -device intel-iommu when the model exists
#   GJ_OVMF_ESP_MB      ESP image size MiB (default 64)
#
# Hard gates (exit 1): missing EFI, missing QEMU, missing OVMF, missing
# mtools / GPT tools.
#
# After a run, soft-scan serial with:
#   ./scripts/gj-product-summary.sh /tmp/gj-ovmf.log
#   ./scripts/gj-quick-keys.sh /tmp/gj-ovmf.log   # GJ-EFI is soft info there
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
efi="${1:-$root/build/GreenJade.efi}"

if [ ! -f "$efi" ]; then
    echo "missing $efi — run: make greenjade.efi" >&2
    exit 1
fi

# --- soft helpers -----------------------------------------------------------

# soft_dd_zero <file> <MiB>
# Create a zero-filled raw image. Prefer status=none; fall back for busybox dd.
soft_dd_zero() {
    _f=$1
    _mb=$2
    if dd if=/dev/zero of="$_f" bs=1M count="$_mb" status=none 2>/dev/null; then
        return 0
    fi
    if dd if=/dev/zero of="$_f" bs=1048576 count="$_mb" 2>/dev/null; then
        return 0
    fi
    dd if=/dev/zero of="$_f" bs=1024 count=$((_mb * 1024)) 2>/dev/null
}

# soft_have_dev <model>
# True if "$QEMU_BIN -device help" lists the model as a whole word. Soft: never
# aborts; caller skips the device on miss.
soft_have_dev() {
    _model=$1
    "$QEMU_BIN" -device help 2>&1 |
        grep -Eiq "(^|[[:space:]])${_model}([[:space:]]|$)"
}

# soft_pick_qemu — first usable emulator; empty string if none found.
soft_pick_qemu() {
    if [ -n "${QEMU_BIN:-}" ] && [ -x "$QEMU_BIN" ]; then
        echo "$QEMU_BIN"
        return 0
    fi
    if [ -x /usr/libexec/qemu-kvm ]; then
        echo /usr/libexec/qemu-kvm
        return 0
    fi
    if command -v qemu-system-x86_64 >/dev/null 2>&1; then
        command -v qemu-system-x86_64
        return 0
    fi
    if [ -x "$HOME/.local/bin/qemu-system-x86_64" ]; then
        echo "$HOME/.local/bin/qemu-system-x86_64"
        return 0
    fi
    if [ -x "$HOME/.local/qemu-kvm/usr/libexec/qemu-kvm" ]; then
        echo "$HOME/.local/qemu-kvm/usr/libexec/qemu-kvm"
        return 0
    fi
    return 1
}

# soft_mcopy <esp@@start> <host> <esp-path>
# Copy when host path exists; soft skip otherwise (never hard-fail).
soft_mcopy() {
    _img=$1
    _host=$2
    _dest=$3
    if [ -f "$_host" ]; then
        mcopy -o -i "$_img" "$_host" "$_dest" 2>/dev/null || \
            echo "run-ovmf: soft mcopy miss $_dest (host present, mcopy failed)" >&2
        return 0
    fi
    return 0
}

QEMU_BIN="$(soft_pick_qemu || true)"
if [ -z "$QEMU_BIN" ]; then
    echo "qemu-kvm not found. Install: sudo dnf install -y qemu-kvm" >&2
    exit 1
fi

# Firmware data dir (only needed for user-local extract; system uses /usr/share)
QEMU_L_ARGS=""
if [ -d "$HOME/.local/qemu-kvm/usr/share/qemu-kvm" ]; then
    case "$QEMU_BIN" in
        "$HOME/.local/"*)
            QEMU_L_ARGS="-L $HOME/.local/qemu-kvm/usr/share/qemu-kvm"
            ;;
    esac
fi

OVMF_CODE="${OVMF_CODE:-}"
OVMF_VARS_SRC="${OVMF_VARS_SRC:-}"
# Prefer non-secboot code/vars for product smoke (secboot may reject unsigned EFI).
for c in \
    /usr/share/edk2/ovmf/OVMF_CODE.fd \
    /usr/share/OVMF/OVMF_CODE.fd \
    /usr/share/edk2/ovmf/OVMF_CODE.cc.fd \
    /usr/share/qemu/OVMF_CODE.fd \
    /usr/share/OVMF/OVMF_CODE_4M.fd \
    /usr/share/edk2/ovmf/OVMF_CODE.secboot.fd \
    /usr/share/OVMF/OVMF_CODE.secboot.fd
do
    if [ -z "$OVMF_CODE" ] && [ -f "$c" ]; then
        OVMF_CODE="$c"
    fi
done
for v in \
    /usr/share/edk2/ovmf/OVMF_VARS.fd \
    /usr/share/OVMF/OVMF_VARS.fd \
    /usr/share/qemu/OVMF_VARS.fd \
    /usr/share/OVMF/OVMF_VARS_4M.fd \
    /usr/share/edk2/ovmf/OVMF_VARS.secboot.fd \
    /usr/share/OVMF/OVMF_VARS.secboot.fd
do
    if [ -z "$OVMF_VARS_SRC" ] && [ -f "$v" ]; then
        OVMF_VARS_SRC="$v"
    fi
done
if [ -z "$OVMF_CODE" ] || [ -z "$OVMF_VARS_SRC" ]; then
    echo "OVMF firmware not found (install edk2-ovmf)" >&2
    exit 1
fi
case "$OVMF_CODE" in
    *secboot*)
        echo "run-ovmf: soft note using secboot OVMF code (unsigned EFI may fail)" >&2
        ;;
esac

need() {
    command -v "$1" >/dev/null 2>&1 || {
        echo "missing tool: $1" >&2
        exit 1
    }
}
need mformat
need mcopy
need mmd

esp="${TMPDIR:-/tmp}/greenjade-esp.$$.img"
vars="${TMPDIR:-/tmp}/greenjade-ovmf-vars.$$.fd"
cleanup() {
    rm -f "$esp" "$vars" 2>/dev/null || true
}
trap cleanup EXIT INT TERM

cp "$OVMF_VARS_SRC" "$vars"

# GPT ESP — size soft-overridable; default 64 MiB
ESP_MB="${GJ_OVMF_ESP_MB:-64}"
soft_dd_zero "$esp" "$ESP_MB"
if command -v sgdisk >/dev/null 2>&1; then
    sgdisk -o -n 1:2048:0 -t 1:ef00 -c 1:EFI "$esp" >/dev/null
elif command -v parted >/dev/null 2>&1; then
    parted -s "$esp" mklabel gpt mkpart ESP fat32 1MiB 100% set 1 esp on
else
    echo "need sgdisk or parted for GPT ESP" >&2
    exit 1
fi

START=$((2048 * 512))
mformat -i "$esp@@$START" -F -v GREENJADE ::
mmd -i "$esp@@$START" ::/EFI
mmd -i "$esp@@$START" ::/EFI/BOOT
mmd -i "$esp@@$START" ::/EFI/GREENJADE 2>/dev/null || true
mmd -i "$esp@@$START" ::/EFI/GREENJADE/user 2>/dev/null || true
mcopy -o -i "$esp@@$START" "$efi" ::/EFI/BOOT/BOOTX64.EFI

# Soft stage Multiboot kernel on ESP for product load path (UEFI loader next).
if [ -f "$root/build/greenjade.elf" ]; then
    mcopy -o -i "$esp@@$START" "$root/build/greenjade.elf" \
        ::/EFI/GREENJADE/KERNEL.ELF
    echo "run-ovmf: staged EFI/GREENJADE/KERNEL.ELF on ESP" >&2
else
    echo "run-ovmf: soft skip KERNEL.ELF (build/greenjade.elf absent)" >&2
fi

# Soft stage freestanding user ELFs when present (product ESP path). Prefer
# staged build/esp tree; fall back to build/user/*.elf. Misses are soft.
if [ -d "$root/build/esp/EFI/GREENJADE/user" ]; then
    for _u in init shell sessiond netstackd storaged vfsd sshd scsi_mid hda_client; do
        soft_mcopy "$esp@@$START" \
            "$root/build/esp/EFI/GREENJADE/user/${_u}.elf" \
            "::/EFI/GREENJADE/user/${_u}.elf"
    done
    echo "run-ovmf: soft staged user ELFs from build/esp (when present)" >&2
else
    for _u in init shell sessiond netstackd storaged vfsd sshd scsi_mid hda_client; do
        soft_mcopy "$esp@@$START" \
            "$root/build/user/${_u}.elf" \
            "::/EFI/GREENJADE/user/${_u}.elf"
    done
fi

GJ_SMP="${GJ_SMP:-1}"
GJ_MEM="${GJ_MEM:-512M}"
IOMMU_ARGS=""
if [ "${GJ_INTEL_IOMMU:-0}" = "1" ]; then
    if soft_have_dev intel-iommu; then
        IOMMU_ARGS="-device intel-iommu,intremap=on"
        echo "run-ovmf: intel-iommu enabled" >&2
    else
        echo "run-ovmf: intel-iommu model unavailable (soft skip)" >&2
    fi
fi

echo "run-ovmf: $QEMU_BIN code=$OVMF_CODE efi=$efi smp=$GJ_SMP mem=$GJ_MEM" >&2

# shellcheck disable=SC2086
exec "$QEMU_BIN" $QEMU_L_ARGS \
    -machine q35 \
    -cpu max \
    -m "$GJ_MEM" \
    -smp "$GJ_SMP" \
    -drive if=pflash,format=raw,unit=0,readonly=on,file="$OVMF_CODE" \
    -drive if=pflash,format=raw,unit=1,file="$vars" \
    -drive if=none,id=esp0,file="$esp",format=raw \
    -device virtio-blk-pci,drive=esp0,bootindex=0 \
    -net none \
    $IOMMU_ARGS \
    -serial stdio \
    -display none \
    -no-reboot \
    -no-shutdown
