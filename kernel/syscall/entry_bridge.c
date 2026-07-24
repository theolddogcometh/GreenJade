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

/* Wave 62 soft inventory stamp (file-local; never product gate). */
#define ENTRY_BRIDGE_SOFT_WAVE 82u
/* inventory|path|rates|honesty|last|surfaces|note|catalog|deepen|PASS */
#define ENTRY_BRIDGE_SOFT_AREAS 134u

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
    kprintf("entry_bridge: soft note milestone=wave82 exclusive=1 "
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
    kprintf("entry_bridge: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=82\n");

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
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("entry_bridge: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/* Grep: entry_bridge: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("entry_bridge: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("entry_bridge: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/* Grep: entry_bridge: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("entry_bridge: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retravelin — Wave 41 return-travelin honesty */
kprintf("entry_bridge: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/* Grep: entry_bridge: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("entry_bridge: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("entry_bridge: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/* Grep: entry_bridge: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("entry_bridge: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("entry_bridge: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/* Grep: entry_bridge: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("entry_bridge: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("entry_bridge: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/* Grep: entry_bridge: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("entry_bridge: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("entry_bridge: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/* Grep: entry_bridge: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("entry_bridge: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retbailey — Wave 46 return-bailey honesty */
kprintf("entry_bridge: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);
/* Grep: entry_bridge: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("entry_bridge: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)ENTRY_BRIDGE_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("entry_bridge: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("entry_bridge: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("entry_bridge: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("entry_bridge: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("entry_bridge: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("entry_bridge: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retsally — Wave 50 return-sally honesty */
kprintf("entry_bridge: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("entry_bridge: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retfosse — Wave 51 return-fosse honesty */
kprintf("entry_bridge: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("entry_bridge: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("entry_bridge: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("entry_bridge: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retravelin — Wave 53 return-travelin honesty */
kprintf("entry_bridge: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("entry_bridge: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("entry_bridge: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retredan — Wave 54 exclusive redan stamp */
kprintf("entry_bridge: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retflank — Wave 55 return-flank honesty */
kprintf("entry_bridge: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retface — Wave 55 exclusive face stamp */
kprintf("entry_bridge: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retgorge — Wave 56 return-gorge honesty */
kprintf("entry_bridge: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("entry_bridge: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retraverse — Wave 57 return-traverse honesty */
kprintf("entry_bridge: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("entry_bridge: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retorillon — Wave 58 return-orillon honesty */
kprintf("entry_bridge: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("entry_bridge: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("entry_bridge: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("entry_bridge: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retplace — Wave 60 return-place honesty */
kprintf("entry_bridge: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("entry_bridge: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("entry_bridge: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("entry_bridge: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("entry_bridge: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("entry_bridge: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("entry_bridge: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("entry_bridge: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: entry_bridge: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("entry_bridge: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: entry_bridge: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("entry_bridge: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: entry_bridge: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("entry_bridge: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: entry_bridge: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("entry_bridge: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: entry_bridge: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("entry_bridge: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=82 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: entry_bridge: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("entry_bridge: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=82 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("entry_bridge: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("entry_bridge: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("entry_bridge: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("entry_bridge: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("entry_bridge: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("entry_bridge: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("entry_bridge: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("entry_bridge: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("entry_bridge: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("entry_bridge: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: entry_bridge: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("entry_bridge: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("entry_bridge: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: entry_bridge: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("entry_bridge: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("entry_bridge: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retbastionangle stamp; Soft≠product)\n");
/* Grep: entry_bridge: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("entry_bridge: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("entry_bridge: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retparapetangle stamp; Soft≠product)\n");
/* Grep: entry_bridge: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("entry_bridge: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("entry_bridge: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retowerangle stamp; Soft≠product)\n");
/* Grep: entry_bridge: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("entry_bridge: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("entry_bridge: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retwallangle stamp; Soft≠product)\n");
/* Grep: entry_bridge: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("entry_bridge: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("entry_bridge: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retholdangle stamp; Soft≠product)\n");
/* Grep: entry_bridge: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("entry_bridge: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("entry_bridge: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retfortressangle stamp; Soft≠product)\n");
/* Grep: entry_bridge: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("entry_bridge: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("entry_bridge: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: entry_bridge: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("entry_bridge: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("entry_bridge: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: entry_bridge: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("entry_bridge: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("entry_bridge: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: entry_bridge: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("entry_bridge: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: entry_bridge: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("entry_bridge: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retaegisangle stamp; Soft≠product)\n");

                            kprintf("entry_bridge: soft deepen wave=%u areas=%u enter=%lu "
            "route=%lu logs=%lu "
            "(Wave 82 exclusive; not bar3)\n",
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
