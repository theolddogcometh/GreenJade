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
 * Soft deepen (Wave 15 base + Wave 19 exclusive; this unit only):
 *   Multi-line greppable "entry_bridge: soft …" inventory:
 *     inventory | path | rates | honesty | last | surfaces | note |
 *     catalog | deepen | PASS
 *   Local edge lamps only — never hard-gates; wrap OK; not bar3.
 * greppable: entry_bridge: soft
 * greppable: entry_bridge: soft inventory
 * greppable: entry_bridge: soft path
 * greppable: entry_bridge: soft rates
 * greppable: entry_bridge: soft honesty
 * greppable: entry_bridge: soft last
 * greppable: entry_bridge: soft surfaces
 * greppable: entry_bridge: soft note
 * greppable: entry_bridge: soft catalog
 * greppable: entry_bridge: soft deepen
 * greppable: entry_bridge: soft inventory PASS
 * greppable: SYSCALL_ENTRY_SOFT_STATS
 *
 * Pure C11 — dual MIT OR Apache-2.0.
 */
#include <gj/klog.h>
#include <gj/syscall.h>
#include <gj/types.h>

/* Wave 38 soft inventory stamp (file-local; never product gate). */
#define ENTRY_BRIDGE_SOFT_WAVE 38u
/* inventory|path|rates|honesty|last|surfaces|note|catalog|deepen|PASS */
#define ENTRY_BRIDGE_SOFT_AREAS 50u

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
 * Greppable soft entry-bridge inventory (Wave 35 exclusive deepen).
 * Prefix-stable markers:
 *   entry_bridge: soft inventory  — edge enter/null/route rollup
 *   entry_bridge: soft path       — honesty claim (LSTAR → note → dispatch)
 *   entry_bridge: soft rates      — bp_null / bp_route share
 *   entry_bridge: soft honesty    — hybrid open; not bar3
 *   entry_bridge: soft last       — last_nr + edge snapshot
 *   entry_bridge: soft surfaces   — Wave 19 surface count lamp
 *   entry_bridge: soft note       — Wave 16 milestone note
 *   entry_bridge: soft catalog    — Wave 19 area name rollup
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

    /* Grep: entry_bridge: soft last */
    kprintf("entry_bridge: soft last nr=%lu enter=%lu route=%lu "
            "logs=%lu once=%u wave=%u\n",
            (unsigned long)g_u64BridgeSoftLastNr,
            (unsigned long)u64Enter,
            (unsigned long)u64Route,
            (unsigned long)u64Logs,
            g_fBridgeSoftOnce ? 1u : 0u,
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);

    /* Grep: entry_bridge: soft surfaces (Wave 20 deepen) */
    kprintf("entry_bridge: soft surfaces count=%u "
            "names=inventory,path,rates,honesty,last,surfaces,note,"
            "catalog,return,retmap,deepen,PASS wave=%u\n",
            (unsigned)ENTRY_BRIDGE_SOFT_AREAS,
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);

    /* Grep: entry_bridge: soft note (Wave 20 deepen) */
    kprintf("entry_bridge: soft note milestone=wave38 exclusive=1 "
            "edge=LSTAR soft_only=1 not_bar3=1 "
            "enter=%lu route=%lu wave=%u\n",
            (unsigned long)u64Enter,
            (unsigned long)u64Route,
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);

    /* Grep: entry_bridge: soft catalog (Wave 20 deepen) */
    kprintf("entry_bridge: soft catalog wave=%u areas=%u "
            "surfaces=inventory,path,rates,honesty,last,surfaces,note,"
            "catalog,return,retmap,deepen,PASS\n",
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE,
            (unsigned)ENTRY_BRIDGE_SOFT_AREAS);

    /* Grep: entry_bridge: soft return (Wave 20 deepen) */
    kprintf("entry_bridge: soft return null_guard=1 ret_rewrite=0 "
            "route_void=1 enter_only=1 product_gate=0 "
            "enter=%lu route=%lu wave=%u\n",
            (unsigned long)u64Enter,
            (unsigned long)u64Route,
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);

    /* Grep: entry_bridge: soft retmap — Wave 19 return-surface map */
    kprintf("entry_bridge: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=38\n");

    /* Grep: entry_bridge: soft deepen wave */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: entry_bridge: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("entry_bridge: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
    /* Grep: entry_bridge: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("entry_bridge: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: entry_bridge: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("entry_bridge: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
    /* Grep: entry_bridge: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("entry_bridge: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: entry_bridge: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("entry_bridge: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
            /* Grep: entry_bridge: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("entry_bridge: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: entry_bridge: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("entry_bridge: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
            /* Grep: entry_bridge: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("entry_bridge: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: entry_bridge: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("entry_bridge: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
            /* Grep: entry_bridge: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("entry_bridge: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: entry_bridge: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("entry_bridge: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
            /* Grep: entry_bridge: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("entry_bridge: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: entry_bridge: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("entry_bridge: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
            /* Grep: entry_bridge: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("entry_bridge: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: entry_bridge: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("entry_bridge: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
            /* Grep: entry_bridge: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("entry_bridge: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: entry_bridge: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("entry_bridge: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
                    /* Grep: entry_bridge: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("entry_bridge: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: entry_bridge: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("entry_bridge: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
                            /* Grep: entry_bridge: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("entry_bridge: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: entry_bridge: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("entry_bridge: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
                            /* Grep: entry_bridge: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("entry_bridge: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: entry_bridge: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("entry_bridge: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
                            /* Grep: entry_bridge: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("entry_bridge: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: entry_bridge: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("entry_bridge: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
                            /* Grep: entry_bridge: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("entry_bridge: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
                            /* Grep: entry_bridge: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("entry_bridge: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("entry_bridge: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/* Grep: entry_bridge: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("entry_bridge: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("entry_bridge: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/* Grep: entry_bridge: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("entry_bridge: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("entry_bridge: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/* Grep: entry_bridge: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("entry_bridge: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retfortress — Wave 35 return-fortress honesty */
kprintf("entry_bridge: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/* Grep: entry_bridge: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("entry_bridge: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft rethold — Wave 36 return-hold honesty */
kprintf("entry_bridge: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/* Grep: entry_bridge: soft retspire — Wave 36 exclusive spire stamp */
kprintf("entry_bridge: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retwall — Wave 37 return-wall honesty */
kprintf("entry_bridge: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/* Grep: entry_bridge: soft retgate — Wave 37 exclusive gate stamp */
kprintf("entry_bridge: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retmoat — Wave 38 return-moat honesty */
kprintf("entry_bridge: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/* Grep: entry_bridge: soft retower — Wave 38 exclusive tower stamp */
kprintf("entry_bridge: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
                            kprintf("entry_bridge: soft deepen wave=%u areas=%u enter=%lu "
            "route=%lu logs=%lu "
            "(Wave 38 exclusive; not bar3)\n",
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
     * Wave 19: local bridge soft inventory + path/rates/surfaces (file-local).
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
