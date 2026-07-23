#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Boot build/greenjade-live.iso under QEMU (hardware-test media smoke).
# Forwards host :2222 → guest :22 for product freestanding sshd; console on serial.
#
# Soft helpers (optional paths — never hard-fail the boot when extras miss):
#   soft_pick_qemu   first usable emulator from a short candidate list
#   soft_have_dev    probe whether QEMU knows a -device model
#   NET_ARGS         virtio-net + user netdev (soft skip if model missing)
#   HDA_ARGS         intel-hda + hda-duplex when both models exist
#   IOMMU_ARGS       only when GJ_INTEL_IOMMU=1 and intel-iommu model exists
#
# Live ISO is the Multiboot2 + El Torito product media path (make live-iso).
# Do not turn optional audio/IOMMU into hard requirements here.
#
# Env:
#   QEMU_BIN            override emulator path
#   GJ_SMP              vCPUs (default 1)
#   GJ_MEM              RAM size (default 2G)
#   GJ_SSH_HOST_PORT    host port for sshd forward (default 2222)
#   GJ_INTEL_IOMMU=1    attach -device intel-iommu when the model exists
#   GJ_LIVE_HDA=0       set 0 to skip soft HDA attach (default 1 = try)
#   GJ_LIVE_NET=0       set 0 to skip soft virtio-net (default 1 = try)
#
# Hard gates (exit 1): missing ISO, missing QEMU.
#
# After a run, soft-scan serial with:
#   ./scripts/gj-product-summary.sh /tmp/gj-live.log
#   ./scripts/gj-quick-keys.sh /tmp/gj-live.log
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
iso="${1:-build/greenjade-live.iso}"

if [ ! -f "$iso" ]; then
	echo "missing $iso — run: make live-iso" >&2
	exit 1
fi

# --- soft helpers -----------------------------------------------------------

# soft_have_dev <model>
# True if "$QEMU_BIN -device help" lists the model as a whole word.
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

QEMU_BIN="$(soft_pick_qemu || true)"
if [ -z "$QEMU_BIN" ]; then
	echo "qemu not found. Install: sudo dnf install -y qemu-kvm" >&2
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

GJ_MEM="${GJ_MEM:-2G}"
GJ_SMP="${GJ_SMP:-1}"
SSH_PORT="${GJ_SSH_HOST_PORT:-2222}"

# Soft virtio-net + user netdev with sshd hostfwd (product freestanding path).
NET_ARGS=""
if [ "${GJ_LIVE_NET:-1}" != "0" ] && soft_have_dev virtio-net-pci; then
	NET_ARGS="-device virtio-net-pci,netdev=n0 -netdev user,id=n0,hostfwd=tcp::${SSH_PORT}-:22"
	echo "run-live-iso: virtio-net hostfwd :${SSH_PORT}->22" >&2
else
	echo "run-live-iso: virtio-net soft skip (model off or GJ_LIVE_NET=0)" >&2
fi

# Soft HDA for audio probe on live media (matches Multiboot smoke surface).
HDA_ARGS=""
if [ "${GJ_LIVE_HDA:-1}" != "0" ] && soft_have_dev intel-hda; then
	if soft_have_dev hda-duplex; then
		HDA_ARGS="-device intel-hda -device hda-duplex"
		echo "run-live-iso: HDA duplex enabled" >&2
	else
		HDA_ARGS="-device intel-hda"
		echo "run-live-iso: intel-hda only (hda-duplex soft skip)" >&2
	fi
else
	echo "run-live-iso: HDA soft skip" >&2
fi

IOMMU_ARGS=""
if [ "${GJ_INTEL_IOMMU:-0}" = "1" ]; then
	if soft_have_dev intel-iommu; then
		IOMMU_ARGS="-device intel-iommu,intremap=on"
		echo "run-live-iso: intel-iommu enabled" >&2
	else
		echo "run-live-iso: intel-iommu model unavailable (soft skip)" >&2
	fi
fi

echo "run-live-iso: $QEMU_BIN iso=$iso mem=$GJ_MEM smp=$GJ_SMP" >&2
# shellcheck disable=SC2086
exec "$QEMU_BIN" $QEMU_L_ARGS \
	-machine q35 \
	-cpu max \
	-m "$GJ_MEM" \
	-smp "$GJ_SMP" \
	-cdrom "$iso" \
	-boot d \
	-serial stdio \
	-display none \
	$NET_ARGS \
	$HDA_ARGS \
	$IOMMU_ARGS \
	-no-reboot \
	-no-shutdown
