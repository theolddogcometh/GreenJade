#!/bin/sh
# SPDX-License-Identifier: MIT
#
# Boot GreenJade under QEMU. On Rocky/RHEL, the system emulator is
# /usr/libexec/qemu-kvm (package qemu-kvm). Multiboot2 ELF is loaded via a
# temporary GRUB ISO because RHEL QEMU 10 -kernel expects PVH/Linux images.
#
# Soft helpers (optional paths — never hard-fail the boot):
#   soft_dd_zero   create sparse/raw disk images with fallback bs=
#   soft_have_dev  probe whether QEMU knows a -device model (nvme, …)
#   NVME_ARGS      only attached when the emulator ships the model
#   IOMMU_ARGS     only when GJ_INTEL_IOMMU=1
#
# Env:
#   QEMU_BIN            override emulator path
#   GJ_SMP              vCPUs (default 1)
#   GJ_MEM              RAM size (default 2G)
#   GJ_INTEL_IOMMU=1    attach -device intel-iommu,intremap=on
#
# After a run, soft-scan serial with:
#   ./scripts/gj-product-summary.sh /tmp/gj.log   # soft exit 0
#   ./scripts/gj-quick-keys.sh /tmp/gj.log        # hard miss exit 1
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
elf="${1:-$root/build/greenjade.elf}"

if [ ! -f "$elf" ]; then
    echo "missing $elf — run make first" >&2
    exit 1
fi

# --- soft helpers -----------------------------------------------------------

# soft_dd_zero <file> <MiB>
# Create a zero-filled raw image. Prefer status=none; fall back for busybox dd.
# Always returns 0 when either path succeeds; exits non-zero only if both fail.
soft_dd_zero() {
    _f=$1
    _mb=$2
    if dd if=/dev/zero of="$_f" bs=1M count="$_mb" status=none 2>/dev/null; then
        return 0
    fi
    dd if=/dev/zero of="$_f" bs=1048576 count="$_mb" 2>/dev/null
}

# soft_have_dev <model>
# True if "$QEMU_BIN -device help" lists the model as a whole word.
# Soft: never aborts the script; caller skips the device on miss.
soft_have_dev() {
    _model=$1
    "$QEMU_BIN" -device help 2>&1 |
        grep -Eiq "(^|[[:space:]])${_model}([[:space:]]|$)"
}

# Resolve QEMU binary — prefer system qemu-kvm when installed
QEMU_BIN="${QEMU_BIN:-}"
if [ -z "$QEMU_BIN" ]; then
    if [ -x /usr/libexec/qemu-kvm ]; then
        QEMU_BIN=/usr/libexec/qemu-kvm
    elif command -v qemu-system-x86_64 >/dev/null 2>&1; then
        QEMU_BIN="$(command -v qemu-system-x86_64)"
    elif [ -x "$HOME/.local/bin/qemu-system-x86_64" ]; then
        QEMU_BIN="$HOME/.local/bin/qemu-system-x86_64"
    elif [ -x "$HOME/.local/qemu-kvm/usr/libexec/qemu-kvm" ]; then
        QEMU_BIN="$HOME/.local/qemu-kvm/usr/libexec/qemu-kvm"
    else
        echo "qemu-kvm not found. Install: sudo dnf install -y qemu-kvm" >&2
        exit 1
    fi
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

# Build a tiny Multiboot2 ISO (RHEL qemu -kernel rejects Multiboot2 ELF)
iso_dir="${TMPDIR:-/tmp}/greenjade-iso.$$"
iso_file="${TMPDIR:-/tmp}/greenjade.$$.iso"
disk_file="${TMPDIR:-/tmp}/greenjade-disk.$$.img"
scsi_disk="${TMPDIR:-/tmp}/greenjade-scsi.$$.img"
nvme_disk="${TMPDIR:-/tmp}/greenjade-nvme.$$.img"
cleanup() {
    rm -rf "$iso_dir" "$iso_file" "$disk_file" "$scsi_disk" "$nvme_disk" 2>/dev/null || true
}
trap cleanup EXIT INT TERM

# 16 MiB raw disk for virtio-blk; separate 8 MiB for virtio-scsi; 32 MiB NVMe T1
soft_dd_zero "$disk_file" 16
soft_dd_zero "$scsi_disk" 8
soft_dd_zero "$nvme_disk" 32

mkdir -p "$iso_dir/boot/grub"
cp "$elf" "$iso_dir/boot/greenjade.elf"
cat >"$iso_dir/boot/grub/grub.cfg" <<'EOF'
set timeout=0
set default=0
menuentry "GreenJade" {
    multiboot2 /boot/greenjade.elf
    boot
}
EOF

if ! command -v grub2-mkrescue >/dev/null 2>&1 && ! command -v grub-mkrescue >/dev/null 2>&1; then
    echo "grub2-mkrescue not found (needed for Multiboot2 on RHEL QEMU)." >&2
    exit 1
fi
MKRESCUE="$(command -v grub2-mkrescue 2>/dev/null || command -v grub-mkrescue)"
"$MKRESCUE" -o "$iso_file" "$iso_dir" >/dev/null 2>&1

# Optional: GJ_SMP=N (default 1), GJ_MEM=size (default 2G)
# Optional: GJ_INTEL_IOMMU=1 adds -device intel-iommu,intremap=on (VT-d live)
GJ_SMP="${GJ_SMP:-1}"
GJ_MEM="${GJ_MEM:-2G}"
IOMMU_ARGS=""
if [ "${GJ_INTEL_IOMMU:-0}" = "1" ]; then
    IOMMU_ARGS="-device intel-iommu,intremap=on"
    echo "run-qemu: intel-iommu enabled" >&2
fi

echo "run-qemu: using $QEMU_BIN smp=$GJ_SMP mem=$GJ_MEM" >&2

# Soft T1 NVMe: only attach when the QEMU build includes the model (not all
# RHEL qemu-kvm splits ship -device nvme). Probe via soft_have_dev; skip soft.
NVME_ARGS=""
if soft_have_dev nvme; then
    NVME_ARGS="-drive if=none,id=nvme0,file=$nvme_disk,format=raw -device nvme,serial=gj-nvme0,drive=nvme0"
    echo "run-qemu: nvme device enabled" >&2
else
    echo "run-qemu: nvme device model unavailable (soft skip)" >&2
fi

# Virtio PCI devices for discovery smoke (net/gpu/blk); no host NIC backend needed
# for enumeration. Present path (A1) will bind later.
# shellcheck disable=SC2086
exec "$QEMU_BIN" $QEMU_L_ARGS \
    -machine q35 \
    -cpu max \
    -m "$GJ_MEM" \
    -smp "$GJ_SMP" \
    -cdrom "$iso_file" \
    -boot d \
    -device virtio-net-pci,netdev=n0 \
    -netdev user,id=n0 \
    -device virtio-gpu-pci \
    -device virtio-keyboard-pci \
    -drive if=none,id=vd0,file="$disk_file",format=raw \
    -device virtio-blk-pci,drive=vd0 \
    -device virtio-scsi-pci,id=scsi0 \
    -drive if=none,id=sd0,file="$scsi_disk",format=raw \
    -device scsi-hd,drive=sd0,bus=scsi0.0 \
    $NVME_ARGS \
    -device intel-hda \
    -device hda-duplex \
    $IOMMU_ARGS \
    -serial stdio \
    -display none \
    -no-reboot \
    -no-shutdown
