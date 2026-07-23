#!/usr/bin/env bash
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Run aarch64 product scaffold under QEMU virt (optional cross toolchain).
#
# Boots build/greenjade-aarch64.elf (or $1) on qemu-system-aarch64 -M virt,
# streams guest serial, optionally writes GJ_AARCH64_LOG, then prints a soft
# marker report. Soft report never fails the script — missing soft lines are
# stderr notes only. Makefile aarch64-smoke tees this script and greps M0 OK
# from the serial stream (product bar); do not hard-fail here on soft misses.
#
# Env:
#   QEMU_AARCH64          qemu binary (default: qemu-system-aarch64)
#   GJ_AARCH64_TIMEOUT    wall timeout seconds (default: 15)
#   GJ_AARCH64_LOG        optional host path for full serial capture
#
# Soft markers (guest UART; fixed-string grep substrings — trailing fields OK):
#   hard bring-up: exceptions/cpu/GIC/timer/pmm/mmu/svc/virtio-mmio/mem probe
#   soft deepen:   shared C, cpu/uart/psci/GIC SPI/timer tick/pmm multi/map,
#                  sched, shared pmm, svc NR/getpid, virtio queue/desc ring,
#                  kmain phase summary
# Product bar (make aarch64-smoke): M0 OK
#
# See also: scripts/gj-aarch64-podman-smoke.sh, kernel/arch/aarch64/README.md
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
ELF="${1:-$ROOT/build/greenjade-aarch64.elf}"
QEMU="${QEMU_AARCH64:-qemu-system-aarch64}"
LOG="${GJ_AARCH64_LOG:-}"

# Keep M0 OK last among product bars. Soft markers are deepen-only (soft
# report); fixed-string match so "… PASS foo=…" lines still count as hits.
MARKERS=(
  "aarch64: shared C kernel PASS"
  "aarch64: shared C soft PASS"
  "aarch64: exceptions PASS"
  "aarch64: cpu PASS"
  "aarch64: cpu soft PASS"
  "aarch64: uart soft PASS"
  "aarch64: uart hex soft PASS"
  "aarch64: psci PASS"
  "aarch64: psci features soft PASS"
  "aarch64: GIC PASS"
  "aarch64: GIC SPI soft PASS"
  "aarch64: timer PASS"
  "aarch64: timer tick soft PASS"
  "aarch64: pmm PASS"
  "aarch64: pmm multi soft PASS"
  "aarch64: pmm soft PASS"
  "aarch64: mmu PASS"
  "aarch64: mmu map soft PASS"
  "aarch64: shared sched PASS"
  "aarch64: shared pmm soft PASS"
  "aarch64: svc PASS"
  "aarch64: svc NR dispatch deepen PASS"
  "aarch64: svc getpid soft PASS"
  "aarch64: virtio-mmio PASS"
  "aarch64: virtio-mmio queue soft PASS"
  "aarch64: virtio-mmio desc ring soft PASS"
  "aarch64: mem probe PASS"
  "aarch64: kmain soft PASS"
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
  echo "run-aarch64: log=$LOG"
fi

# Soft report (do not fail the script — Makefile aarch64-smoke greps M0 OK).
hit=0
miss=0
for marker in "${MARKERS[@]}"; do
  if printf '%s\n' "$OUT" | grep -qF -- "$marker"; then
    echo "run-aarch64: found: $marker"
    hit=$((hit + 1))
  else
    echo "run-aarch64: missing: $marker" >&2
    miss=$((miss + 1))
  fi
done

if printf '%s\n' "$OUT" | grep -qF -- 'M0 OK'; then
  echo "run-aarch64: PASS (M0 OK) hit=$hit miss=$miss qemu_rc=$RC"
else
  echo "run-aarch64: soft note (no M0 OK) hit=$hit miss=$miss qemu_rc=$RC" >&2
fi
exit 0
