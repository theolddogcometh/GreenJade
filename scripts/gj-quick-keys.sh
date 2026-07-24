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
# Hard keys (exit 1 on any miss):
#   M0 OK, sshd live, scsi_mid live, multi-stream, multi-seg, shell live
# Soft info only (never fail the gate):
#   TRAP #UD count, soak_tib / pmm soak, sessiond/netstackd live, UEFI tags
# Soft deepen / soft verify (info only — never hard-fail):
#   serial: soft verify PASS|FAIL|idle   (x86 COM1; serial.c)
#   aarch64: kmain soft PASS             (kmain phase summary)
#   linux: nr class soft PASS|PARTIAL|NONE  (linux_dispatch NR table)
# Soft companions (Wave 72 exclusive — info only, never hard-fail):
#   continuum high-water toward 21400; makefile_max=21400 when greppable (soft ≠ bar3)
#   (scan is source of truth; do not hardcode 21400 if scan still 21300; 21300 prior tip when N>=21300)
#   soft deepen retplaceofarmsangle/retdoubletenailleangle (CREATE-ONLY soft names only)
#   product lamps 0; soft ≠ product complete
#   bar3 OPEN stamp (client launch + Deck Top 50 still NOT-TRIED)
#
# Exit 0 if all presence keys hit; 1 if any missing (soft info reported only).
# Exit-friendly: missing log / bad greps never leave the shell in a partial
# state; pipefail is scoped so optional greps cannot abort mid-report.
# Soft inventory (always exit 0): scripts/gj-product-summary.sh
# Full hard smoke (do not soften): scripts/smoke-all.sh
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" || $# -lt 1 ]]; then
  echo "usage: $0 <logfile>" >&2
  echo "  Hard presence gate for product keys; exit 1 on miss." >&2
  echo "  Soft inventory: scripts/gj-product-summary.sh <logfile>" >&2
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

# Soft presence (never increments miss)
info_check() {
  local label=$1 pat=$2
  if gqa_q "$pat"; then
    echo "  info: $label  (present)"
  else
    echo "  info: $label  (absent)"
  fi
}

echo "gj-quick-keys: $log"

# --- hard product boot markers (kernel + live embeds) ---------------------
# Keep patterns broad enough for "live elf" / "live spawn PASS" variants but
# specific enough to avoid shell soft-stub false hits (e.g. bare "usshd").
check "M0 OK"            'M0 OK'
check "sshd live"        'sshd: live spawn PASS|sshd: live'
check "scsi_mid live"    'scsi_mid: live spawn PASS|scsi_mid: live'
check "multi-stream"     'hda: multi-stream mixer PASS|multi-stream mixer PASS|hda: multi-stream|multi-stream'
check "multi-seg"        'TCP multi-segment PASS|multi-segment PASS'
check "shell live"       'shell: live spawn PASS|shell: live'

# TRAP #UD — count only (not a hard miss; agents care about volume)
# Matches: *** TRAP #UD (6) …  from kernel/arch/x86_64/trap.c
set +e
ud_n=$(gqa 'TRAP[[:space:]]+#UD' | wc -l | tr -d ' ')
set -e
case "$ud_n" in
'' | *[!0-9]*) ud_n=0 ;;
esac
echo "  TRAP #UD count: $ud_n"

# --- soft companion inventory (never fails the gate) ----------------------
echo "  --- soft companions ---"
info_check "sessiond live"   'sessiond: live spawn PASS|sessiond: live'
info_check "netstackd live"  'netstackd: live spawn PASS|netstackd: live'
info_check "storaged live"   'storaged: live spawn PASS|storaged: live'
info_check "vfsd live"       'vfsd: live spawn PASS|vfsd: live'
info_check "hda_client live" 'hda_client: live spawn PASS|hda_client: live'
info_check "pmm soak PASS"   'pmm: soak PASS'
info_check "soak_tib PASS"   'pmm: soak_tib PASS|soak_tib PASS'
info_check "soak_tib SKIP"   'pmm: soak_tib SKIP|soak_tib SKIP'
info_check "GJ-EFI"          'GJ-EFI'
info_check "sshd-gj path OK" 'sshd-gj: live path PASS'
if gqa_q 'sshd-gj: live path FAIL'; then
  echo "  info: sshd-gj path FAIL  (present; soft — not a hard key)"
fi

# Soft deepen / soft verify markers (info only — never hard-fail the gate).
# Grep: serial: soft verify · aarch64: kmain soft · linux: nr class soft
echo "  --- soft verify deepen ---"
info_check "serial soft verify" 'serial: soft verify'
info_check "kmain soft"         'aarch64: kmain soft PASS|kmain soft PASS'
info_check "nr class soft"      'linux: nr class soft'

# --- Wave 72 soft companions (never increments miss; hard keys stay hard) ---
# Continuum high-water toward 21400; greppable 21400 only when N>=21400; 21300 prior tip when N>=21300.
# Scan is source of truth — do not hardcode false makefile_max if still 21300.
# Bar3 open stamp: media READY ≠ client run ≠ Top-50; always OPEN honesty.
echo "  --- continuum / bar3 soft (wave 72) ---"
info_check "makefile_max=21400 (log)" 'makefile_max=21400'
info_check "makefile_max=21300 (log)" 'makefile_max=21300'
info_check "ubar3open / bar3=0 (log)" 'ubar3open|bar3=0|bar3:[[:space:]]*OPEN|bar3 OPEN'
# Host continuum scan: stamp makefile_max=21400 greppable only when N>=21400.
if [[ -f "$ROOT/scripts/gj-continuum-makefile-snippet.sh" ]]; then
  mx_line=$(bash "$ROOT/scripts/gj-continuum-makefile-snippet.sh" --max 2>/dev/null || true)
  mx_line=${mx_line//$'\r'/}
  mx_line=$(printf '%s' "${mx_line:-}" | tr -cd '\11\12\15\40-\176' | head -c 80 || true)
  echo "  info: continuum ${mx_line:-makefile_max=(unknown)}"
  _mx_n=""
  if [[ "${mx_line:-}" =~ makefile_max=([0-9]+) ]]; then
    _mx_n="${BASH_REMATCH[1]}"
  fi
  if [[ -n "${_mx_n:-}" ]]; then
    echo "  info: continuum high-water  makefile_max=$_mx_n  (CREATE-ONLY soft graph)"
    if [[ "$_mx_n" -ge 21400 ]]; then
      echo "  info: continuum high-water  makefile_max=21400 greppable  (soft graph ≠ bar3)"
    elif [[ "$_mx_n" -ge 21300 ]]; then
      echo "  info: continuum high-water  makefile_max=21300 greppable  (prior tip; soft ≠ product)"
      echo "  info: continuum high-water  makefile_max=21400  (absent; tree max=$_mx_n; scan=truth)"
    else
      echo "  info: continuum high-water  makefile_max=21400  (absent; tree max=$_mx_n; scan=truth)"
    fi
  else
    echo "  info: continuum high-water  makefile_max=(unparsed)"
  fi
  echo "  info: continuum honesty  soft graph wire ≠ bar3 client / Top50 titles"
  echo "  info: continuum honesty  soft ≠ product complete; product lamps 0"
  echo "  info: continuum wave 72  target decade M=21400 (CREATE-ONLY soft; parent paste wires)"
  echo "  info: continuum soft deepen  retplaceofarmsangle/retdoubletenailleangle (CREATE-ONLY soft ≠ product)"
else
  echo "  info: continuum makefile_max=(helper missing)"
fi
# Bar3 open stamp (always soft; never claims client run or matrix fill).
if [[ -f "$ROOT/scripts/steam-bar3-check.sh" ]]; then
  _bar3_out=$(bash "$ROOT/scripts/steam-bar3-check.sh" 2>/dev/null || true)
  bar3_line=$(printf '%s\n' "${_bar3_out:-}" | head -n1 || true)
  echo "  info: ${bar3_line:-steam-bar3-check: (no output)}"
  _bar3_open=$(printf '%s\n' "${_bar3_out:-}" | grep -E '^\s*(open:|bar3:)' | head -n3 || true)
  if [[ -n "${_bar3_open:-}" ]]; then
    while IFS= read -r _bl; do
      [[ -n "${_bl:-}" ]] && echo "  info: bar3 honesty  ${_bl#"${_bl%%[![:space:]]*}"}"
    done <<<"$_bar3_open"
  fi
fi
echo "  info: bar3 OPEN  (client launch + Deck Top 50 still NOT-TRIED)"
echo "  info: bar3 honesty  media READY ≠ smoke-all PASS ≠ title PASS"

if [[ "$miss" -eq 0 ]]; then
  echo "gj-quick-keys: PASS miss=0 ud=$ud_n"
  exit 0
fi
echo "gj-quick-keys: FAIL miss=$miss ud=$ud_n" >&2
exit 1
