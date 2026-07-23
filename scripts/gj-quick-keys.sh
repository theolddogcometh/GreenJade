#!/usr/bin/env bash
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# Grep a boot/smoke log for key GreenJade product lines (agent-friendly).
#
# Usage:
#   ./scripts/gj-quick-keys.sh <logfile>
#   ./scripts/run-qemu.sh build/greenjade.elf 2>&1 | tee /tmp/gj.log
#   ./scripts/gj-quick-keys.sh /tmp/gj.log
#
# Keys: sshd live, scsi_mid live, multi-stream, M0 OK, TRAP #UD count
# Exit 0 if all presence keys hit; 1 if any missing (UD count is reported only).
# Exit-friendly: missing log / bad greps never leave the shell in a partial
# state; pipefail is scoped so optional greps cannot abort mid-report.
# Soft inventory (always exit 0): scripts/gj-product-summary.sh
set -euo pipefail

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" || $# -lt 1 ]]; then
  echo "usage: $0 <logfile>" >&2
  exit 2
fi

log=$1
if [[ ! -f "$log" ]]; then
  echo "gj-quick-keys: missing log: $log" >&2
  exit 1
fi

# Binary-safe greps (serial logs may have NULs). Soft on miss (|| true).
gqa() { grep -a -E "$@" "$log" 2>/dev/null || true; }
gqa_q() { grep -a -q -E "$1" "$log" 2>/dev/null; }

miss=0
check() {
  local label=$1 pat=$2
  # Isolate optional grep from pipefail / set -e
  if gqa_q "$pat"; then
    echo "  ok: $label"
  else
    echo "  MISS: $label  (/$pat/)"
    miss=$((miss + 1))
  fi
}

echo "gj-quick-keys: $log"

# Product boot markers (kernel + live embeds)
check "M0 OK"            'M0 OK'
check "sshd live"        'sshd: live'
check "scsi_mid live"    'scsi_mid: live'
check "multi-stream"     'multi-stream'

# TRAP #UD — count only (not a hard miss; agents care about volume)
# Matches: *** TRAP #UD (6) …  from kernel/arch/x86_64/trap.c
set +e
ud_n=$(gqa 'TRAP[[:space:]]+#UD' | wc -l | tr -d ' ')
set -e
case "$ud_n" in
'' | *[!0-9]*) ud_n=0 ;;
esac
echo "  TRAP #UD count: $ud_n"

if [[ "$miss" -eq 0 ]]; then
  echo "gj-quick-keys: PASS"
  exit 0
fi
echo "gj-quick-keys: FAIL miss=$miss" >&2
exit 1
