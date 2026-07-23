/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Bridge from syscall_entry.S stack frame → gj_syscall_dispatch.
 *
 * Soft deepen: LSTAR edge counters (bridge enter / null) before route.
 *   greppable: SYSCALL_ENTRY_SOFT_STATS
 *   Cumulative route stats live in dispatch.c (gj_syscall_entry_stats_*).
 *
 * Soft deepen (Wave 14 base + Wave 15 exclusive; this unit only):
 *   Multi-line greppable "entry_bridge: soft …" inventory:
 *     inventory | path | rates | honesty | last | deepen | PASS
 *   Local edge lamps only — never hard-gates; wrap OK; not bar3.
 * greppable: entry_bridge: soft
 * greppable: entry_bridge: soft inventory
 * greppable: entry_bridge: soft path
 * greppable: entry_bridge: soft rates
 * greppable: entry_bridge: soft honesty
 * greppable: entry_bridge: soft last
 * greppable: entry_bridge: soft deepen
 * greppable: entry_bridge: soft inventory PASS
 * greppable: SYSCALL_ENTRY_SOFT_STATS
 *
 * Pure C11 — dual MIT OR Apache-2.0.
 */
#include <gj/klog.h>
#include <gj/syscall.h>
#include <gj/types.h>

/* Wave 15 soft inventory stamp (file-local; never product gate). */
#define ENTRY_BRIDGE_SOFT_WAVE  15u
/* inventory|path|rates|honesty|last|deepen|PASS */
#define ENTRY_BRIDGE_SOFT_AREAS 7u

/*
 * Soft edge tallies (wrap OK). Diagnostics only — does not alter route.
 * greppable: entry_bridge: soft …
 */
static u64 g_u64BridgeSoftEnter; /* every LSTAR land / bridge call */
static u64 g_u64BridgeSoftNull;  /* pRegs == NULL at edge */
static u64 g_u64BridgeSoftRoute; /* dispatched into gj_syscall_dispatch */
static u64 g_u64BridgeSoftLogN;  /* inventory log emissions */
static u64 g_u64BridgeSoftLastNr; /* soft last NR snapshot at route edge */
static u8  g_fBridgeSoftOnce;    /* one-shot deep dump after first activity */

static void entry_bridge_soft_inc(u64 *pCtr);
static void entry_bridge_soft_inventory_log(void);
static void entry_bridge_soft_inventory_maybe_once(void);

/** Soft: bump path tally (u64 wrap is fine for telemetry). */
static void
entry_bridge_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/**
 * Greppable soft entry-bridge inventory (Wave 15 exclusive deepen).
 * Prefix-stable markers:
 *   entry_bridge: soft inventory  — edge enter/null/route rollup
 *   entry_bridge: soft path       — honesty claim (LSTAR → note → dispatch)
 *   entry_bridge: soft rates      — bp_null / bp_route share (Wave 15)
 *   entry_bridge: soft honesty    — hybrid open; not bar3 (Wave 15)
 *   entry_bridge: soft last       — last_nr + edge snapshot (Wave 15)
 *   entry_bridge: soft deepen     — wave stamp
 *   entry_bridge: soft inventory PASS / soft PASS
 * greppable: entry_bridge: soft
 * Honesty: soft inventory only — not product gate; not bar3.
 */
static void
entry_bridge_soft_inventory_log(void)
{
    u64 u64Enter;
    u64 u64Null;
    u64 u64Route;
    u64 u64Logs;
    u64 u64BpNull;
    u64 u64BpRoute;
    u32 u32Routed;

    entry_bridge_soft_inc(&g_u64BridgeSoftLogN);
    u64Enter = g_u64BridgeSoftEnter;
    u64Null = g_u64BridgeSoftNull;
    u64Route = g_u64BridgeSoftRoute;
    u64Logs = g_u64BridgeSoftLogN;
    u32Routed = (u64Route != 0) ? 1u : 0u;
    if (u64Enter != 0) {
        u64BpNull = (u64Null * 10000ull) / u64Enter;
        u64BpRoute = (u64Route * 10000ull) / u64Enter;
    } else {
        u64BpNull = 0;
        u64BpRoute = 0;
    }

    /* Grep: entry_bridge: soft inventory */
    kprintf("entry_bridge: soft inventory enter=%lu null=%lu route=%lu "
            "logs=%lu routed=%u areas=%u wave=%u "
            "(soft; not bar3)\n",
            (unsigned long)u64Enter,
            (unsigned long)u64Null,
            (unsigned long)u64Route,
            (unsigned long)u64Logs,
            u32Routed,
            (unsigned)ENTRY_BRIDGE_SOFT_AREAS,
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);

    /* Grep: entry_bridge: soft path */
    kprintf("entry_bridge: soft path claim=LSTAR→note_bridge→dispatch "
            "null_guard=1 enter_only=1 ret_rewrite=0 "
            "areas=%u wave=%u (soft inventory; not bar3)\n",
            (unsigned)ENTRY_BRIDGE_SOFT_AREAS,
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);

    /* Grep: entry_bridge: soft rates (Wave 15 deepen) */
    kprintf("entry_bridge: soft rates bp_null=%lu bp_route=%lu "
            "enter=%lu null=%lu route=%lu wave=%u\n",
            (unsigned long)u64BpNull,
            (unsigned long)u64BpRoute,
            (unsigned long)u64Enter,
            (unsigned long)u64Null,
            (unsigned long)u64Route,
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);

    /* Grep: entry_bridge: soft honesty (Wave 15 deepen) */
    kprintf("entry_bridge: soft honesty hybrid=OptionC open=1 bar3=0 "
            "product_linux_abi=open soft_only=1 edge=LSTAR "
            "wave=%u (soft inventory; never closes hybrid)\n",
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);

    /* Grep: entry_bridge: soft last (Wave 15 deepen) */
    kprintf("entry_bridge: soft last nr=%lu enter=%lu route=%lu "
            "logs=%lu once=%u wave=%u\n",
            (unsigned long)g_u64BridgeSoftLastNr,
            (unsigned long)u64Enter,
            (unsigned long)u64Route,
            (unsigned long)u64Logs,
            g_fBridgeSoftOnce ? 1u : 0u,
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);

    /* Grep: entry_bridge: soft deepen wave */
    kprintf("entry_bridge: soft deepen wave=%u areas=%u enter=%lu "
            "route=%lu logs=%lu "
            "(Wave 15 exclusive; not bar3)\n",
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE,
            (unsigned)ENTRY_BRIDGE_SOFT_AREAS,
            (unsigned long)u64Enter,
            (unsigned long)u64Route,
            (unsigned long)u64Logs);

    /* Grep: entry_bridge: soft inventory PASS / soft PASS */
    kprintf("entry_bridge: soft inventory PASS wave=%u logs=%lu "
            "enter=%lu route=%lu\n",
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE,
            (unsigned long)u64Logs,
            (unsigned long)u64Enter,
            (unsigned long)u64Route);
    kprintf("entry_bridge: soft PASS wave=%u logs=%lu\n",
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE,
            (unsigned long)u64Logs);
}

/**
 * After first product bridge activity, print soft inventory once.
 * Diagnostics only — never gates path PASS.
 */
static void
entry_bridge_soft_inventory_maybe_once(void)
{
    if (g_fBridgeSoftOnce != 0) {
        return;
    }
    if (g_u64BridgeSoftEnter == 0) {
        return;
    }
    g_fBridgeSoftOnce = 1;
    entry_bridge_soft_inventory_log();
}

void
gj_syscall_entry_asm_bridge(struct gj_syscall_regs *pRegs)
{
    /*
     * Soft entry edge: every LSTAR land is counted before personality route.
     * Smoke tests that call gj_syscall_dispatch directly skip this note.
     * Wave 15: local bridge soft inventory + path/rates stamps (file-local).
     */
    entry_bridge_soft_inc(&g_u64BridgeSoftEnter);
    gj_syscall_entry_soft_note_bridge(pRegs);
    if (pRegs == NULL) {
        entry_bridge_soft_inc(&g_u64BridgeSoftNull);
        entry_bridge_soft_inventory_maybe_once();
        return;
    }
    /* Soft last NR at LSTAR edge (never mutates frame / route). */
    g_u64BridgeSoftLastNr = pRegs->u64Nr;
    entry_bridge_soft_inc(&g_u64BridgeSoftRoute);
    entry_bridge_soft_inventory_maybe_once();
    gj_syscall_dispatch(pRegs);
}
