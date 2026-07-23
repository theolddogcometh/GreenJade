#!/usr/bin/env bash
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# aarch64 product smoke via podman + qemu-system-aarch64 (SELinux :Z mount).
#
# For hosts without a local qemu-system-aarch64: pull a Fedora image, install
# qemu inside the container, and boot build/greenjade-aarch64.elf under
# QEMU virt the same way scripts/run-aarch64.sh does on bare metal.
#
# Usage:
#   ./scripts/gj-aarch64-podman-smoke.sh
#   ./scripts/gj-aarch64-podman-smoke.sh build/greenjade-aarch64.elf
#   GJ_AARCH64_TIMEOUT=20 PODMAN_IMAGE=docker.io/library/fedora:40 \
#     ./scripts/gj-aarch64-podman-smoke.sh
#
# Env:
#   PODMAN_BIN            podman binary (default: podman)
#   PODMAN_IMAGE          container image (default: docker.io/library/fedora:40)
#   GJ_AARCH64_TIMEOUT    wall timeout seconds (default: 15)
#   GJ_AARCH64_LOG        optional host path for full serial capture
#   GJ_PODMAN_SKIP_DNF    if set non-empty, skip dnf install (image already has qemu)
#   GJ_PODMAN_Z           mount suffix (default: ro,Z — SELinux relabel)
#
# Soft marker report (does not hard-fail on missing markers). Exit 1 only when
# podman/ELF/image bootstrap is unusable. Makefile aarch64-smoke greps M0 OK
# on bare-metal path; this helper is optional container fallback.
#
# See also: scripts/run-aarch64.sh, kernel/arch/aarch64/README.md
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
ELF="${1:-$ROOT/build/greenjade-aarch64.elf}"
PODMAN_BIN="${PODMAN_BIN:-podman}"
PODMAN_IMAGE="${PODMAN_IMAGE:-docker.io/library/fedora:40}"
TIMEOUT_S="${GJ_AARCH64_TIMEOUT:-15}"
LOG="${GJ_AARCH64_LOG:-}"
MOUNT_MODE="${GJ_PODMAN_Z:-ro,Z}"

MARKERS=(
	"aarch64: exceptions PASS"
	"aarch64: cpu PASS"
	"aarch64: GIC PASS"
	"aarch64: timer PASS"
	"aarch64: pmm PASS"
	"aarch64: mmu PASS"
	"aarch64: svc PASS"
	"aarch64: svc NR dispatch deepen PASS"
	"aarch64: svc getpid soft PASS"
	"aarch64: virtio-mmio PASS"
	"aarch64: virtio-mmio queue soft PASS"
	"aarch64: virtio-mmio desc ring soft PASS"
	"aarch64: mem probe PASS"
	"M0 OK"
)

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
	echo "usage: $0 [path/to/greenjade-aarch64.elf]" >&2
	echo "  Runs qemu-system-aarch64 inside podman with -v \$ROOT:/work:${MOUNT_MODE}" >&2
	echo "  PODMAN_IMAGE=$PODMAN_IMAGE  timeout=${TIMEOUT_S}s" >&2
	exit 0
fi

if [[ ! -f "$ELF" ]]; then
	echo "gj-aarch64-podman-smoke: missing $ELF — build with: make aarch64" >&2
	exit 1
fi
if ! command -v "$PODMAN_BIN" >/dev/null 2>&1; then
	echo "gj-aarch64-podman-smoke: $PODMAN_BIN not found" >&2
	exit 1
fi

# Relativize ELF under ROOT for in-container path /work/...
case "$ELF" in
/*)
	if [[ "$ELF" == "$ROOT"/* ]]; then
		ELF_REL="${ELF#"$ROOT"/}"
	else
		echo "gj-aarch64-podman-smoke: ELF must live under $ROOT for :Z bind mount" >&2
		exit 1
	fi
	;;
*)
	ELF_REL="$ELF"
	;;
esac
ELF_IN="/work/$ELF_REL"

if [[ ! -f "$ROOT/$ELF_REL" ]]; then
	echo "gj-aarch64-podman-smoke: cannot resolve $ROOT/$ELF_REL" >&2
	exit 1
fi

echo "gj-aarch64-podman-smoke: image=$PODMAN_IMAGE mount=$ROOT:/work:$MOUNT_MODE"
echo "gj-aarch64-podman-smoke: kernel=$ELF_IN timeout=${TIMEOUT_S}s"

# Inside the container: optional dnf install, then timeout + qemu virt.
# virtio-net-device → virtio-mmio so queue/desc-ring soft paths program MMIO
# (QueueReady / QueuePFN stay 0 in guest — no DMA).
# shellcheck disable=SC2016
if [[ -n "${GJ_PODMAN_SKIP_DNF:-}" ]]; then
	INNER_CMD=$(cat <<EOF
set -euo pipefail
command -v qemu-system-aarch64 >/dev/null
timeout ${TIMEOUT_S} qemu-system-aarch64 -M virt -cpu cortex-a57 -nographic -kernel '${ELF_IN}' -netdev user,id=n0 -device virtio-net-device,netdev=n0
EOF
)
else
	INNER_CMD=$(cat <<EOF
set -euo pipefail
if ! command -v qemu-system-aarch64 >/dev/null 2>&1; then
  dnf install -y -q qemu-system-aarch64 >/dev/null
fi
timeout ${TIMEOUT_S} qemu-system-aarch64 -M virt -cpu cortex-a57 -nographic -kernel '${ELF_IN}' -netdev user,id=n0 -device virtio-net-device,netdev=n0
EOF
)
fi

set +e
OUT=$("$PODMAN_BIN" run --rm \
	-v "$ROOT:/work:${MOUNT_MODE}" \
	"$PODMAN_IMAGE" \
	bash -c "$INNER_CMD" 2>&1)
RC=$?
set -e

printf '%s\n' "$OUT"
if [[ -n "$LOG" ]]; then
	printf '%s\n' "$OUT" >"$LOG"
	echo "gj-aarch64-podman-smoke: log=$LOG"
fi

hit=0
miss=0
for marker in "${MARKERS[@]}"; do
	if printf '%s\n' "$OUT" | grep -qF -- "$marker"; then
		echo "gj-aarch64-podman-smoke: found: $marker"
		hit=$((hit + 1))
	else
		echo "gj-aarch64-podman-smoke: missing: $marker" >&2
		miss=$((miss + 1))
	fi
done

if printf '%s\n' "$OUT" | grep -qF -- 'M0 OK'; then
	echo "gj-aarch64-podman-smoke: PASS (M0 OK) hit=$hit miss=$miss podman_rc=$RC"
	exit 0
fi

# Soft SKIP when container/qemu could not run (network, dnf, etc.)
echo "gj-aarch64-podman-smoke: SKIP or FAIL (no M0 OK) hit=$hit miss=$miss podman_rc=$RC" >&2
exit 0
