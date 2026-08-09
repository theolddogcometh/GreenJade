#!/bin/sh
# SPDX-License-Identifier: MIT OR Apache-2.0
#
# GreenJade — Assurance Lite automated checks (docs/ASSURANCE_LITE.md).
#
# Verifies permanent hazard rules and claim-honesty greps against the tree.
# This is L1 process evidence only — Soft≠product; not Dual DoD close; not
# DO-178C certification. L3 DoD still needs stamped flash + host probes.
#
# L1 surfaces (deepened):
#   H1/H3 permanent hazards
#   Freestanding SKIP defaults (GJ_RTL8168_PROBE=0 · GJ_XHCI_MSC_PROBE=0)
#     + #ifndef guards · class-driver source gates (rtl8168/xhci_msc/net_l2)
#     + main.c freestanding SKIP hold lamps
#   freestanding_no_exec / RUN_INIT=0 soft module policy (G-AC-1)
#     + all_ko_skip_exec / stage_only denser
#   UDX product path honesty (userspace hosts; not freestanding/.ko product)
#     + surface headers · runtime src · xhci_udx README · host .c honesty
#     + OPEN_UDX / product=UDX+ABI · virtio T0 direction lamps
#   Soft≠product · dual MIT/Apache · no Dual DoD close from L1 alone
#   Flash bar identity: GJ_IMAGE_VERSION (test what you fly; stamp ≠ DoD close)
#
# Usage:
#   ./scripts/gj-assurance-check.sh           # human report; exit 1 on hard fail
#   ./scripts/gj-assurance-check.sh --quiet   # exit code only
#
# Env:
#   GJ_ROOT  — repo root (default: parent of scripts/)
#
# Greppable: assurance: lite | gj-assurance-check | H1 | H3 | Soft!=product
#            freestanding_no_exec | freestanding SKIP | UDX | G-AC-1
#            GJ_RTL8168_PROBE | GJ_XHCI_MSC_PROBE | test what you fly
set -eu
root="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
cd "$root"
quiet=0
for arg in "$@"; do
	case "$arg" in
	-q|--quiet) quiet=1 ;;
	-h|--help)
		sed -n '2,28p' "$0" | sed 's/^# \{0,1\}//'
		exit 0
		;;
	*)
		echo "gj-assurance-check: unknown option: $arg" >&2
		exit 2
		;;
	esac
done

log() {
	if [ "$quiet" -eq 0 ]; then
		printf '%s\n' "$*"
	fi
}

fail=0
warn=0
pass=0

note_pass() {
	pass=$((pass + 1))
	log "  PASS  $1"
}

note_fail() {
	fail=$((fail + 1))
	log "  FAIL  $1"
}

note_warn() {
	warn=$((warn + 1))
	log "  WARN  $1"
}

# rg if available, else grep -R
has_rg=0
if command -v rg >/dev/null 2>&1; then
	has_rg=1
fi

# Parse "#define NAME <digits>" default under optional #ifndef guard.
# $1=file $2=macro → prints the numeric default or empty.
define_num() {
	_f=$1
	_m=$2
	[ -f "$_f" ] || return 1
	sed -n "s/^#define[[:space:]][[:space:]]*${_m}[[:space:]][[:space:]]*\\([0-9][0-9]*\\).*/\\1/p" "$_f" \
		| head -n 1
}

# --- H1: net_eth_poll must not be *called* from IRQ-class units ------------
log "assurance: lite H1 net_eth_poll IRQ ban"
h1_paths="kernel/arch/x86_64/timer.c kernel/arch/x86_64/apic.c kernel/arch/x86_64/x2apic.c kernel/drv/irq_msix.c"
h1_bad=0
for f in $h1_paths; do
	if [ ! -f "$f" ]; then
		continue
	fi
	# Real call site only: net_eth_poll(); — not comment mentions "net_eth_poll ("
	if grep -nE '(^|[^[:alnum:]_])net_eth_poll[[:space:]]*\([^;]*\)[[:space:]]*;' "$f" \
		>/dev/null 2>&1; then
		note_fail "H1 call in $f"
		h1_bad=1
	fi
done
if [ "$h1_bad" -eq 0 ]; then
	note_pass "H1 no net_eth_poll(); in timer/apic/x2apic/irq_msix"
fi

# sched must own poll
if grep -nE '(^|[^[:alnum:]_])net_eth_poll[[:space:]]*\([^;]*\)[[:space:]]*;' \
	kernel/sched/thread.c >/dev/null 2>&1; then
	note_pass "H1 scheduler_run path calls net_eth_poll (run-loop owner)"
else
	note_warn "H1 scheduler_run has no net_eth_poll(); call — check Dual DoD B poll ownership"
fi

# --- H3: process death thr_exit + trampoline dead AS -----------------------
log "assurance: lite H3 clone/sibling death order"
if grep -n 'thread_exit_process' kernel/proc/process.c >/dev/null 2>&1 \
	&& grep -n 'thread_exit_process' kernel/sched/thread.c >/dev/null 2>&1; then
	note_pass "H3 thread_exit_process present (process + thread)"
else
	note_fail "H3 missing thread_exit_process wiring"
fi
if grep -n 'skip user' kernel/sched/thread.c >/dev/null 2>&1 \
	|| grep -n 'dead AS' kernel/sched/thread.c >/dev/null 2>&1; then
	note_pass "H3 trampoline dead-AS skip residual present"
else
	note_warn "H3 trampoline dead-AS skip string not found"
fi

# --- Image stamp + assurance docs ------------------------------------------
# Flash bar honesty: stamp identity only — Soft!=product; not Dual DoD close.
# Current lab bar example: v2026.08.04.72 (config.h; re-flash for L3).
log "assurance: lite config / docs"
if [ -f kernel/include/gj/config.h ] \
	&& grep -q 'GJ_IMAGE_VERSION' kernel/include/gj/config.h; then
	ver=$(sed -n 's/^#define[[:space:]][[:space:]]*GJ_IMAGE_VERSION[[:space:]][[:space:]]*"\([^"]*\)".*/\1/p' \
		kernel/include/gj/config.h | head -n 1)
	if [ -n "$ver" ]; then
		note_pass "GJ_IMAGE_VERSION=$ver (flash bar identity; Soft!=product; not DoD close)"
	else
		note_fail "GJ_IMAGE_VERSION define not parseable"
	fi
	# Stamp shape YYYY.MM.DD.N — greppable flash-bar form
	if printf '%s' "$ver" | grep -qE '^[0-9]{4}\.[0-9]{2}\.[0-9]{2}\.[0-9]+$'; then
		note_pass "GJ_IMAGE_VERSION stamp shape YYYY.MM.DD.N ($ver)"
	else
		note_fail "GJ_IMAGE_VERSION stamp shape invalid: ${ver:-empty}"
	fi
else
	note_fail "config.h / GJ_IMAGE_VERSION missing"
fi
if [ -f docs/ASSURANCE_LITE.md ]; then
	note_pass "docs/ASSURANCE_LITE.md present"
else
	note_fail "docs/ASSURANCE_LITE.md missing"
fi
if [ -f docs/TODO.md ] && grep -q 'ASSURANCE_LITE' docs/TODO.md; then
	note_pass "TODO live status links ASSURANCE_LITE"
else
	note_warn "TODO.md does not reference ASSURANCE_LITE"
fi

# Soft≠product / G-AC-1 greppable surface in docs
if grep -qE 'Soft≠product|Soft != product|Soft ≠ product|Soft!=product' docs/ASSURANCE_LITE.md \
	&& grep -q 'G-AC-1' docs/ASSURANCE_LITE.md docs/ABI_FIRST_PIVOT.md 2>/dev/null; then
	note_pass "Soft≠product + G-AC-1 greppable in assurance/ABI docs"
else
	note_warn "Soft≠product / G-AC-1 markers thin in docs"
fi

# --- Freestanding SKIP defaults (product policy) ---------------------------
# Freestanding class drivers are NOT product. Default SKIP both NIC and USB.
# Product drivers = userspace UDX+ABI; T0 product net = virtio until UDX owns wire.
log "assurance: lite freestanding SKIP defaults"
cfg=kernel/include/gj/config.h
if [ -f "$cfg" ]; then
	rtl_def=$(define_num "$cfg" GJ_RTL8168_PROBE || true)
	xhci_def=$(define_num "$cfg" GJ_XHCI_MSC_PROBE || true)
	if [ "$rtl_def" = "0" ]; then
		note_pass "GJ_RTL8168_PROBE default=$rtl_def (freestanding NIC SKIP)"
	else
		note_fail "GJ_RTL8168_PROBE default=${rtl_def:-missing} — want 0 (SKIP; not product)"
	fi
	if [ "$xhci_def" = "0" ]; then
		note_pass "GJ_XHCI_MSC_PROBE default=$xhci_def (freestanding USB SKIP)"
	else
		note_fail "GJ_XHCI_MSC_PROBE default=${xhci_def:-missing} — want 0 (SKIP; not product)"
	fi
	# Honesty comments must remain greppable next to the knobs
	if grep -q 'Soft!=product' "$cfg" \
		&& grep -qE 'freestanding rtl SKIP|GJ_RTL8168_PROBE' "$cfg" \
		&& grep -qE 'freestanding USB SKIP|GJ_XHCI_MSC_PROBE' "$cfg"; then
		note_pass "config.h Soft!=product + freestanding SKIP greppable"
	else
		note_warn "config.h freestanding SKIP honesty comments thin"
	fi
	# Deepen: #ifndef guards so defaults stay 0 unless deliberately overridden
	if grep -qE '#ifndef[[:space:]]+GJ_RTL8168_PROBE' "$cfg" \
		&& grep -qE '#ifndef[[:space:]]+GJ_XHCI_MSC_PROBE' "$cfg"; then
		note_pass "config.h #ifndef guards on freestanding SKIP knobs (override-safe default 0)"
	else
		note_fail "config.h missing #ifndef guards for GJ_RTL8168_PROBE / GJ_XHCI_MSC_PROBE"
	fi
else
	note_fail "config.h missing (cannot verify freestanding SKIP defaults)"
fi

# Soft module path: RUN_INIT=0 / freestanding_no_exec (G-AC-1 eng residual)
lmod=kernel/mm/linux_module.c
if [ -f "$lmod" ]; then
	run_init=$(define_num "$lmod" GJ_SOFT_MODULE_RUN_INIT || true)
	if [ "$run_init" = "0" ]; then
		note_pass "GJ_SOFT_MODULE_RUN_INIT default=$run_init (freestanding_no_exec)"
	else
		note_fail "GJ_SOFT_MODULE_RUN_INIT default=${run_init:-missing} — want 0 (no in-kernel .ko init)"
	fi
	if grep -q 'freestanding_no_exec' "$lmod" \
		&& grep -q 'G-AC-1' "$lmod"; then
		note_pass "linux_module freestanding_no_exec + G-AC-1 greppable"
	else
		note_fail "linux_module missing freestanding_no_exec / G-AC-1 honesty"
	fi
	if grep -q '_Static_assert(GJ_SOFT_MODULE_RUN_INIT == 0' "$lmod" \
		&& grep -q '_Static_assert(GJ_LMOD_NEVER_EXEC_KO == 1' "$lmod"; then
		note_pass "linux_module G-AC-1 _Static_assert RUN_INIT=0 / NEVER_EXEC_KO=1"
	else
		note_warn "linux_module G-AC-1 _Static_assert pair not found"
	fi
	# Deepen: all_ko_skip_exec / stage_only policy denser under RUN_INIT=0
	if grep -qE 'GJ_LMOD_ALL_KO_SKIP_EXEC|all_ko_skip_exec' "$lmod" \
		&& grep -qE 'GJ_LMOD_STAGE_ONLY|stage_only' "$lmod" \
		&& grep -qE 'Soft!=product|Soft ≠ product' "$lmod"; then
		note_pass "linux_module all_ko_skip_exec + stage_only + Soft!=product denser"
	else
		note_warn "linux_module all_ko_skip_exec / stage_only denser markers thin"
	fi
else
	note_fail "kernel/mm/linux_module.c missing"
fi

# Deepen: class-driver source must gate on SKIP defaults (not product wire)
log "assurance: lite freestanding SKIP source gates"
if [ -f kernel/drv/rtl8168.c ]; then
	if grep -qE '#if[[:space:]]*!GJ_RTL8168_PROBE|#if[[:space:]]*GJ_RTL8168_PROBE' kernel/drv/rtl8168.c \
		&& grep -qE 'probe SKIP|freestanding rtl SKIP|GJ_RTL8168_PROBE=0' kernel/drv/rtl8168.c \
		&& grep -qE 'Soft!=product|Soft ≠ product' kernel/drv/rtl8168.c; then
		note_pass "rtl8168.c freestanding SKIP gate + Soft!=product honesty"
	else
		note_fail "rtl8168.c missing freestanding SKIP gate / honesty"
	fi
else
	note_fail "kernel/drv/rtl8168.c missing"
fi
if [ -f kernel/drv/xhci_msc.c ]; then
	if grep -qE '#if[[:space:]]*!GJ_XHCI_MSC_PROBE|#if[[:space:]]*GJ_XHCI_MSC_PROBE' kernel/drv/xhci_msc.c \
		&& grep -qE 'init SKIP|MSC SKIP|GJ_XHCI_MSC_PROBE=0' kernel/drv/xhci_msc.c; then
		note_pass "xhci_msc.c freestanding MSC SKIP gate greppable"
	else
		note_fail "xhci_msc.c missing freestanding MSC SKIP gate"
	fi
else
	note_fail "kernel/drv/xhci_msc.c missing"
fi
if [ -f kernel/drv/net_l2.c ]; then
	if grep -qE 'freestanding rtl SKIP|GJ_RTL8168_PROBE=0' kernel/drv/net_l2.c \
		&& grep -qE '#if[[:space:]]*!GJ_RTL8168_PROBE' kernel/drv/net_l2.c; then
		note_pass "net_l2 freestanding rtl SKIP honesty under probe=0"
	else
		note_warn "net_l2 freestanding rtl SKIP markers thin"
	fi
fi
# main.c must hold freestanding SKIP once-lamps (not thrash class drivers)
if [ -f kernel/main.c ]; then
	if grep -qE 'GJ_RTL8168_PROBE' kernel/main.c \
		&& grep -qE 'GJ_XHCI_MSC_PROBE' kernel/main.c \
		&& grep -qE 'freestanding MSC SKIP|freestanding.*SKIP|class SKIP' kernel/main.c; then
		note_pass "main.c freestanding SKIP hold lamps (rtl + USB knobs)"
	else
		note_warn "main.c freestanding SKIP hold lamps thin"
	fi
fi

# --- UDX product path honesty ----------------------------------------------
# Product drivers = Linux-shaped userspace over UDX/DDI. Not freestanding
# class thrash; not in-kernel .ko wire. Soft residual ≠ product AC.
log "assurance: lite UDX product path honesty"
udx_ok=1
if [ -f user/udx/include/udx/udx.h ] && [ -f user/udx/README.md ]; then
	note_pass "user/udx tree present (udx.h + README)"
else
	note_fail "user/udx product library tree incomplete"
	udx_ok=0
fi
# Deepen: UDX surface headers (DDI/PCI/host/device/mmio/irq/dma)
udx_hdr_ok=1
for h in udx.h ddi.h pci.h host.h device.h mmio.h irq.h dma.h types.h; do
	if [ ! -f "user/udx/include/udx/$h" ]; then
		udx_hdr_ok=0
		break
	fi
done
if [ "$udx_hdr_ok" -eq 1 ]; then
	note_pass "UDX surface headers present (udx/ddi/pci/host/device/mmio/irq/dma/types)"
else
	note_fail "UDX surface header set incomplete under user/udx/include/udx/"
	udx_ok=0
fi
# Deepen: UDX runtime sources (userspace product library, not kernel class)
if [ -f user/udx/src/core.c ] && [ -f user/udx/src/host.c ] \
	&& [ -f user/udx/src/pci.c ] && [ -f user/udx/Makefile ]; then
	note_pass "UDX runtime sources present (core/host/pci + Makefile)"
else
	note_fail "UDX runtime sources incomplete (core/host/pci/Makefile)"
	udx_ok=0
fi
if [ -f user/drivers/rtl8168_udx/rtl8168_udx.c ] \
	&& [ -f user/drivers/xhci_udx/xhci_udx.c ]; then
	note_pass "UDX class hosts present (rtl8168_udx + xhci_udx)"
else
	note_fail "UDX class hosts missing (rtl8168_udx / xhci_udx)"
	udx_ok=0
fi
if [ -f user/udx/README.md ]; then
	if grep -qE 'G-AC-1' user/udx/README.md \
		&& grep -qE 'Soft!=product|Soft != product|Soft ≠ product' user/udx/README.md \
		&& grep -qE 'freestanding.*SKIP|SKIP.*freestanding|class drivers.*SKIP' user/udx/README.md \
		&& grep -qE 'Linux-shaped userspace|userspace UDX|product path:.*userspace UDX' user/udx/README.md; then
		note_pass "user/udx/README product direction honesty (G-AC-1 · Soft!=product · SKIP)"
	else
		note_fail "user/udx/README missing product-direction honesty markers"
		udx_ok=0
	fi
	# Deepen: product=UDX+ABI + virtio T0 explicit in UDX README
	if grep -qE 'product=UDX\+ABI|product path:.*UDX|userspace UDX host' user/udx/README.md \
		&& grep -qE 'virtio' user/udx/README.md; then
		note_pass "user/udx/README product=UDX+ABI · virtio T0 greppable"
	else
		note_warn "user/udx/README product=UDX+ABI / virtio T0 markers thin"
	fi
fi
if [ -f user/drivers/rtl8168_udx/README.md ]; then
	if grep -qE 'Soft!=product|Soft ≠ product|Soft != product' user/drivers/rtl8168_udx/README.md \
		&& grep -qE 'G-AC-1|virtio' user/drivers/rtl8168_udx/README.md \
		&& grep -qE 'freestanding SKIP|GJ_RTL8168_PROBE|not.*product' user/drivers/rtl8168_udx/README.md; then
		note_pass "rtl8168_udx README Soft!=product · freestanding SKIP honesty"
	else
		note_warn "rtl8168_udx README product honesty markers thin"
	fi
fi
# Deepen: xhci_udx README parity (USB product direction · freestanding MSC SKIP)
if [ -f user/drivers/xhci_udx/README.md ]; then
	if grep -qE 'Soft!=product|Soft ≠ product|Soft != product' user/drivers/xhci_udx/README.md \
		&& grep -qE 'G-AC-1' user/drivers/xhci_udx/README.md \
		&& grep -qE 'GJ_XHCI_MSC_PROBE|freestanding MSC|MSC.*SKIP|default OFF' user/drivers/xhci_udx/README.md \
		&& grep -qE 'UDX|userspace' user/drivers/xhci_udx/README.md; then
		note_pass "xhci_udx README Soft!=product · freestanding MSC SKIP honesty"
	else
		note_fail "xhci_udx README missing product-direction / freestanding MSC SKIP honesty"
		udx_ok=0
	fi
else
	note_fail "user/drivers/xhci_udx/README.md missing"
	udx_ok=0
fi
# Deepen: class-host source honesty (Soft!=product · G-AC-1 · freestanding SKIP)
if [ -f user/drivers/rtl8168_udx/rtl8168_udx.c ]; then
	if grep -qE 'Soft!=product|Soft != product' user/drivers/rtl8168_udx/rtl8168_udx.c \
		&& grep -qE 'G-AC-1' user/drivers/rtl8168_udx/rtl8168_udx.c \
		&& grep -qE 'GJ_RTL8168_PROBE|freestanding.*SKIP|product=UDX' user/drivers/rtl8168_udx/rtl8168_udx.c; then
		note_pass "rtl8168_udx.c source Soft!=product · G-AC-1 · freestanding SKIP honesty"
	else
		note_warn "rtl8168_udx.c source product honesty markers thin"
	fi
fi
if [ -f user/drivers/xhci_udx/xhci_udx.c ]; then
	if grep -qE 'Soft!=product|Soft != product' user/drivers/xhci_udx/xhci_udx.c \
		&& grep -qE 'G-AC-1' user/drivers/xhci_udx/xhci_udx.c \
		&& grep -qE 'GJ_XHCI_MSC_PROBE|freestanding_msc=SKIP|product=UDX' user/drivers/xhci_udx/xhci_udx.c; then
		note_pass "xhci_udx.c source Soft!=product · G-AC-1 · freestanding MSC SKIP honesty"
	else
		note_warn "xhci_udx.c source product honesty markers thin"
	fi
fi
if [ -f docs/ABI_FIRST_PIVOT.md ]; then
	if grep -q 'G-AC-1' docs/ABI_FIRST_PIVOT.md \
		&& grep -qE 'UDX|userspace' docs/ABI_FIRST_PIVOT.md \
		&& grep -qE 'GJ_RTL8168_PROBE|freestanding.*SKIP|Default.*SKIP' docs/ABI_FIRST_PIVOT.md \
		&& grep -qE 'virtio' docs/ABI_FIRST_PIVOT.md; then
		note_pass "ABI_FIRST_PIVOT product law (G-AC-1 · UDX · freestanding SKIP · virtio T0)"
	else
		note_fail "ABI_FIRST_PIVOT missing UDX/SKIP/virtio product law markers"
		udx_ok=0
	fi
else
	note_fail "docs/ABI_FIRST_PIVOT.md missing"
	udx_ok=0
fi
# Deepen: ASSURANCE_LITE itself names freestanding SKIP knobs + UDX product path
if [ -f docs/ASSURANCE_LITE.md ]; then
	if grep -qE 'GJ_RTL8168_PROBE' docs/ASSURANCE_LITE.md \
		&& grep -qE 'GJ_XHCI_MSC_PROBE' docs/ASSURANCE_LITE.md \
		&& grep -qE 'UDX|userspace UDX' docs/ASSURANCE_LITE.md \
		&& grep -qE 'freestanding_no_exec|RUN_INIT=0' docs/ASSURANCE_LITE.md; then
		note_pass "ASSURANCE_LITE freestanding SKIP knobs + UDX + freestanding_no_exec greppable"
	else
		note_fail "ASSURANCE_LITE missing freestanding SKIP / UDX / freestanding_no_exec markers"
		udx_ok=0
	fi
fi
# main.c once-lamp product direction (soft honesty; not DoD close)
if [ -f kernel/main.c ]; then
	if grep -q 'rtl8168_udx' kernel/main.c \
		&& grep -q 'xhci_udx' kernel/main.c \
		&& grep -qE 'Soft!=product|Soft≠product' kernel/main.c \
		&& grep -q 'freestanding_no_exec' kernel/main.c; then
		note_pass "main.c product-direction honesty lamps (UDX hosts · freestanding_no_exec)"
	else
		note_warn "main.c product-direction UDX/freestanding_no_exec lamps thin"
	fi
	# Deepen: OPEN_UDX / product=UDX+ABI once-lamp direction (not Dual DoD close)
	if grep -qE 'OPEN_UDX|product=UDX\+ABI|product path residual' kernel/main.c \
		&& grep -qE 'virtio T0|virtio' kernel/main.c; then
		note_pass "main.c OPEN_UDX / product=UDX+ABI · virtio T0 direction lamps"
	else
		note_warn "main.c OPEN_UDX / virtio T0 direction lamps thin"
	fi
fi
# Keep shell quiet about unused var under set -u in edge paths
: "${udx_ok}"

# --- Soft≠product · L1 ≠ Dual DoD close · dual license ---------------------
log "assurance: lite Soft≠product · L1 honesty · dual license"
# H5: Soft claim must never be framed as Dual DoD close in process docs
if grep -qE 'Soft≠product|Soft ≠ product|Soft!=product' docs/ASSURANCE_LITE.md \
	&& grep -qE 'assurance-check.*PASS|L1' docs/ASSURANCE_LITE.md \
	&& grep -qE 'Dual DoD|not Dual DoD close|≠ Dual DoD|!= Dual DoD|not.*DoD close' docs/ASSURANCE_LITE.md; then
	note_pass "ASSURANCE_LITE: Soft≠product + L1 PASS ≠ Dual DoD close"
else
	note_fail "ASSURANCE_LITE missing L1≠Dual-DoD-close honesty"
fi
# Hazard H5 row greppable
if grep -qE '\| H5 \|' docs/ASSURANCE_LITE.md \
	|| grep -qE 'H5.*Soft claim|Soft claim = product' docs/ASSURANCE_LITE.md; then
	note_pass "H5 Soft-claim=product hazard greppable in ASSURANCE_LITE"
else
	note_warn "H5 Soft-claim hazard row not greppable"
fi
# Three layers + L3 close rule (no Dual DoD from L1 alone)
if grep -qE 'Close Dual DoD B only with L3|L3 evidence|deliverable object code' docs/ASSURANCE_LITE.md \
	&& grep -qE 'L1' docs/ASSURANCE_LITE.md; then
	note_pass "three-layer V&V: Dual DoD close requires L3 (not L1 alone)"
else
	note_fail "ASSURANCE_LITE missing L3-required Dual DoD close rule"
fi
# Dual MIT OR Apache-2.0 tree law
if [ -f LICENSE ] && grep -qE 'MIT|Apache' LICENSE \
	&& grep -qE 'MIT OR Apache|dual' docs/ASSURANCE_LITE.md; then
	note_pass "dual MIT OR Apache-2.0 greppable (LICENSE + ASSURANCE_LITE)"
else
	note_warn "dual-license markers thin"
fi
# Script self-honesty: this file must not claim Dual DoD close on PASS
if grep -qE 'not Dual DoD close|L1 process evidence only|Soft≠product|Soft!=product' \
	scripts/gj-assurance-check.sh; then
	note_pass "gj-assurance-check self-honesty: L1 only · Soft≠product · not DoD close"
else
	note_fail "gj-assurance-check missing self Soft≠product / L1-only banner"
fi
# Image-version companion must stay Soft≠product (stamp ≠ product PASS)
if [ -f scripts/gj-image-version.sh ] \
	&& grep -qE 'Soft≠product|Soft!=product|soft≠product' scripts/gj-image-version.sh \
	&& grep -qE 'not a product PASS|not Dual DoD|test what you fly' scripts/gj-image-version.sh; then
	note_pass "gj-image-version Soft≠product · stamp ≠ product/DoD PASS"
else
	note_warn "gj-image-version honesty markers thin"
fi

# --- Stamp storm smell (heuristic WARN only) -------------------------------
log "assurance: lite H2 stamp-storm heuristic (warn only)"
# Many consecutive kprintf with wave= in one file can reintroduce H2
if [ "$has_rg" -eq 1 ]; then
	storm=$(rg -c 'kprintf\(' kernel/drv/virtio_net.c 2>/dev/null || echo 0)
else
	storm=$(grep -c 'kprintf(' kernel/drv/virtio_net.c 2>/dev/null || echo 0)
fi
# strip non-digits
storm=$(printf '%s' "$storm" | tr -cd '0-9')
[ -z "$storm" ] && storm=0
if [ "$storm" -gt 80 ]; then
	note_warn "H2 virtio_net.c has ${storm} kprintf sites — watch stamp-storm class"
else
	note_pass "H2 virtio_net.c kprintf count=${storm} (heuristic OK)"
fi

# --- Report ----------------------------------------------------------------
log ""
log "gj-assurance-check: summary pass=$pass warn=$warn fail=$fail"
log "  layer:        L1 process evidence only (docs/ASSURANCE_LITE.md)"
log "  soft≠product: this PASS ≠ product AC ≠ bar3 ≠ G-AC-1 waiver"
log "  Dual DoD:     A/B close needs L3 stamped flash + host probes — not L1 alone"
log "  freestanding: SKIP defaults (rtl/USB) · source gates · freestanding_no_exec RUN_INIT=0"
log "  product path: userspace UDX+ABI · headers/src/hosts · virtio T0 until UDX owns wire (G-AC-1)"
log "  flash bar:    GJ_IMAGE_VERSION stamp identity only — test what you fly (not DoD close)"
log "  greppable:    gj-assurance-check | assurance: lite | Soft!=product | freestanding_no_exec | freestanding SKIP | UDX | G-AC-1 | GJ_RTL8168_PROBE | GJ_XHCI_MSC_PROBE | test what you fly"
if [ "$fail" -gt 0 ]; then
	log "gj-assurance-check: FAIL"
	exit 1
fi
log "gj-assurance-check: PASS (L1 only — not Dual DoD close)"
exit 0
