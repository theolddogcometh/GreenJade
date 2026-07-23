#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Boot build/greenjade-live.iso under QEMU (hardware-test media smoke).
# Forwards host :2222 → guest :22 for product freestanding sshd; console on serial.
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
iso="${1:-build/greenjade-live.iso}"

if [ ! -f "$iso" ]; then
	echo "missing $iso — run: make live-iso" >&2
	exit 1
fi

QEMU_BIN="${QEMU_BIN:-}"
if [ -z "$QEMU_BIN" ]; then
	if [ -x /usr/libexec/qemu-kvm ]; then
		QEMU_BIN=/usr/libexec/qemu-kvm
	elif command -v qemu-system-x86_64 >/dev/null 2>&1; then
		QEMU_BIN="$(command -v qemu-system-x86_64)"
	else
		echo "qemu not found" >&2
		exit 1
	fi
fi

GJ_MEM="${GJ_MEM:-2G}"
GJ_SMP="${GJ_SMP:-1}"
echo "run-live-iso: $QEMU_BIN iso=$iso mem=$GJ_MEM" >&2
# shellcheck disable=SC2086
exec "$QEMU_BIN" \
	-machine q35 \
	-cpu max \
	-m "$GJ_MEM" \
	-smp "$GJ_SMP" \
	-cdrom "$iso" \
	-boot d \
	-serial stdio \
	-display none \
	-device virtio-net-pci,netdev=n0 \
	-netdev user,id=n0,hostfwd=tcp::2222-:22
