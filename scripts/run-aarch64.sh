#!/usr/bin/env bash
# SPDX-License-Identifier: MIT OR Apache-2.0
# Run aarch64 product scaffold under QEMU virt (optional cross toolchain).
# Greppable markers from the guest — see kernel/arch/aarch64/README.md
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
ELF="${1:-$ROOT/build/greenjade-aarch64.elf}"
QEMU="${QEMU_AARCH64:-qemu-system-aarch64}"
LOG="${GJ_AARCH64_LOG:-}"

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

if [[ ! -f "$ELF" ]]; then
  echo "run-aarch64: missing $ELF — build with: make aarch64" >&2
  exit 1
fi
if ! command -v "$QEMU" >/dev/null 2>&1; then
  echo "run-aarch64: $QEMU not found" >&2
  exit 1
fi

# virtio-net-device lands on virtio-mmio (user netdev); exercises queue +
# desc-ring soft programming with QueueReady/QueuePFN left 0 (no DMA).
echo "run-aarch64: $QEMU -M virt -cpu cortex-a57 -nographic -kernel $ELF -device virtio-net-device"
# Capture serial so callers can grep; always stream to stdout.
set +e
OUT=$(timeout "${GJ_AARCH64_TIMEOUT:-15}" "$QEMU" \
  -M virt -cpu cortex-a57 -nographic -kernel "$ELF" \
  -netdev user,id=n0 -device virtio-net-device,netdev=n0 2>&1)
RC=$?
set -e
printf '%s\n' "$OUT"
if [[ -n "$LOG" ]]; then
  printf '%s\n' "$OUT" >"$LOG"
fi

# Soft report (do not fail the script — Makefile aarch64-smoke greps M0 OK).
for marker in "${MARKERS[@]}"; do
  if printf '%s\n' "$OUT" | grep -qF -- "$marker"; then
    echo "run-aarch64: found: $marker"
  else
    echo "run-aarch64: missing: $marker" >&2
  fi
done
exit 0
