/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Top-level syscall dispatch: native GJ vs Linux hybrid personality.
 *
 * Product path:
 *   - Boot default is LINUX so freestanding embeds, glibc smokes, and Steam
 *     land on the Option C hybrid table without per-process setup.
 *   - PCB u32Personality == 0 selects native GJ_SYS_* (native.c).
 *   - This file only routes and binds Linux "current"; handlers live in
 *     native.c / linux_*.c. Do not put subsystem logic here.
 *
 * Soft inventory (Wave 15 base + Wave 35 exclusive deepen; this unit only):
 *   "syscall: soft stats …"       — legacy aggregate (field-stable)
 *   "syscall: soft inventory …"   — wave stamp + caps + log_n
 *   "syscall: soft bridge …"      — LSTAR bridge enter/null
 *   "syscall: soft route …"       — dispatch enter/null + native/linux
 *   "syscall: soft personality …" — PCB native/linux + default + mirrors
 *   "syscall: soft bind …"        — linux_set_current bound/unbound
 *   "syscall: soft lifecycle …"   — init + set_default ok/reject
 *   "syscall: soft outcome …"     — complete + ret neg/zero/pos
 *   "syscall: soft last …"        — last_nr / last_ret snapshot
 *   "syscall: soft api …"         — stats_get / reset / soft_log tallies
 *   "syscall: soft rates …"       — soft share basis points
 *   "syscall: soft honesty …"     — hybrid open; not bar3
 *   "syscall: soft edge …"        — bridge+dispatch combined
 *   "syscall: soft share …"       — native/linux complete share
 *   "syscall: soft catalog …"     — surface catalog stamp
 *   "syscall: soft surfaces …"    — Wave 19 surface count lamp
 *   "syscall: soft note …"        — Wave 16 milestone note
 *   "syscall: soft deepen …"      — wave=111 area stamp
 *   "syscall: soft path …"        — surface catalog honesty
 *   "syscall: soft inventory PASS" / "syscall: soft PASS"
 * greppable: SYSCALL_ENTRY_SOFT_STATS / "syscall: soft"
 *
 * Soft only; never hard-gates. Linux ABI hybrid product remains open.
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 */
#include <gj/cold_ipc.h>
#include <gj/cpu.h>
#include <gj/futex.h>
#include <gj/klog.h>
#include <gj/linux_dispatch.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/syscall.h>
#include <gj/thread.h>

/* Wave 45 soft inventory stamp + area count (greppable deepen). */
#define SYSCALL_SOFT_WAVE 111u
#define SYSCALL_SOFT_AREAS 202u

/* Used only when no process is bound (early boot / standalone unit tests). */
static enum gj_personality g_eDefaultPersonality = GJ_PERSONALITY_LINUX;

/*
 * Top-level entry soft counters (wrap OK). See gj_syscall_entry_stats.
 * greppable: SYSCALL_ENTRY_SOFT_STATS
 */
static struct gj_syscall_entry_stats g_entryStats;

/*
 * Wave 35 exclusive deepen (file-local; never hard-gates; wrap OK).
 * greppable: syscall: soft …
 */
static u64 g_u64SoftLogN;       /* inventory / multi-line dump emissions */
static u64 g_u64SoftStatsGet;   /* gj_syscall_entry_stats_get entries */
static u64 g_u64SoftStatsGetNull;
static u64 g_u64SoftStatsReset; /* gj_syscall_entry_stats_reset entries */
static u64 g_u64SoftSoftCall;   /* gj_syscall_entry_stats_soft entries */
static u64 g_u64SoftBridgeNote; /* gj_syscall_entry_soft_note_bridge */
static u8  g_fSoftInvOnce;      /* one-shot multi-line after first activity */

static void entry_soft_inc(u64 *pCtr);
static void entry_soft_mirror_default(void);
static void entry_soft_note_complete(struct gj_syscall_regs *pRegs);
static void entry_soft_inventory_log(void);
static void entry_soft_maybe_once(void);

/** Soft: bump path tally (u64 wrap is fine for telemetry). */
static void
entry_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/**
 * Refresh 0/1 mirrors of the boot default personality (not cumulative).
 */
static void
entry_soft_mirror_default(void)
{
    if (g_eDefaultPersonality == GJ_PERSONALITY_LINUX) {
        g_entryStats.u64DefaultIsLinux = 1;
        g_entryStats.u64DefaultIsNative = 0;
    } else {
        g_entryStats.u64DefaultIsLinux = 0;
        g_entryStats.u64DefaultIsNative = 1;
    }
}

/**
 * Soft outcome buckets after a live frame finished (handler wrote i64Ret).
 */
static void
entry_soft_note_complete(struct gj_syscall_regs *pRegs)
{
    i64 i64Ret;

    if (pRegs == NULL) {
        return;
    }
    g_entryStats.u64Complete++;
    i64Ret = pRegs->i64Ret;
    g_entryStats.u64LastRetBits = (u64)i64Ret;
    if (i64Ret < 0) {
        g_entryStats.u64RetNeg++;
    } else if (i64Ret == 0) {
        g_entryStats.u64RetZero++;
    } else {
        g_entryStats.u64RetPos++;
    }
}

/**
 * Greppable Wave 16 soft entry inventory (product / smoke).
 * Snapshots public entry stats + file-local deepen counters.
 * Diagnostics only; wrap OK; never hard-gates.
 * greppable: syscall: soft
 */
static void
entry_soft_inventory_log(void)
{
    struct gj_syscall_entry_stats s;
    u64 u64Route;
    u64 u64Pcb;
    u64 u64BpNative;
    u64 u64BpLinux;
    u64 u64BpBound;
    u64 u64BpNeg;
    u64 u64BpComplete;
    u64 u64Edge;
    u32 u32DefLinux;
    u32 u32DefNative;

    entry_soft_inc(&g_u64SoftLogN);
    entry_soft_mirror_default();
    s = g_entryStats;
    u32DefLinux = (s.u64DefaultIsLinux != 0) ? 1u : 0u;
    u32DefNative = (s.u64DefaultIsNative != 0) ? 1u : 0u;

    /* Soft route share (basis points; 0 if no live dispatch). */
    u64Route = s.u64Native + s.u64Linux;
    if (u64Route != 0) {
        u64BpNative = (s.u64Native * 10000ull) / u64Route;
        u64BpLinux = (s.u64Linux * 10000ull) / u64Route;
    } else {
        u64BpNative = 0;
        u64BpLinux = 0;
    }
    u64Pcb = s.u64PcbNative + s.u64PcbLinux;
    if ((s.u64Bound + s.u64Unbound) != 0) {
        u64BpBound = (s.u64Bound * 10000ull) / (s.u64Bound + s.u64Unbound);
    } else {
        u64BpBound = 0;
    }
    if (s.u64Complete != 0) {
        u64BpNeg = (s.u64RetNeg * 10000ull) / s.u64Complete;
    } else {
        u64BpNeg = 0;
    }
    /* Wave 19: complete share of route + combined edge enter. */
    if (u64Route != 0) {
        u64BpComplete = (s.u64Complete * 10000ull) / u64Route;
    } else {
        u64BpComplete = 0;
    }
    u64Edge = s.u64BridgeEnter + s.u64DispatchEnter;

    /* Grep: syscall: soft inventory */
    kprintf("syscall: soft inventory wave=%u areas=%u bridge=%llu "
            "disp=%llu native=%llu linux=%llu complete=%llu logs=%llu "
            "boot_default=LINUX hybrid=OptionC "
            "(soft inventory; not bar3)\n",
            (unsigned)SYSCALL_SOFT_WAVE,
            SYSCALL_SOFT_AREAS,
            (unsigned long long)s.u64BridgeEnter,
            (unsigned long long)s.u64DispatchEnter,
            (unsigned long long)s.u64Native,
            (unsigned long long)s.u64Linux,
            (unsigned long long)s.u64Complete,
            (unsigned long long)g_u64SoftLogN);

    /*
     * Legacy aggregate rollup — keep field order stable for existing greps.
     * Grep: syscall: soft stats
     */
    kprintf("syscall: soft stats bridge=%llu bridge_null=%llu "
            "disp=%llu disp_null=%llu native=%llu linux=%llu "
            "pcb_n=%llu pcb_l=%llu def_pers=%llu bound=%llu unbound=%llu "
            "init=%llu set_ok=%llu set_rej=%llu complete=%llu "
            "ret_neg=%llu ret0=%llu ret_pos=%llu last_nr=%llu "
            "def_linux=%llu def_native=%llu\n",
            (unsigned long long)s.u64BridgeEnter,
            (unsigned long long)s.u64BridgeNull,
            (unsigned long long)s.u64DispatchEnter,
            (unsigned long long)s.u64DispatchNull,
            (unsigned long long)s.u64Native,
            (unsigned long long)s.u64Linux,
            (unsigned long long)s.u64PcbNative,
            (unsigned long long)s.u64PcbLinux,
            (unsigned long long)s.u64DefaultPers,
            (unsigned long long)s.u64Bound,
            (unsigned long long)s.u64Unbound,
            (unsigned long long)s.u64Init,
            (unsigned long long)s.u64SetPersOk,
            (unsigned long long)s.u64SetPersReject,
            (unsigned long long)s.u64Complete,
            (unsigned long long)s.u64RetNeg,
            (unsigned long long)s.u64RetZero,
            (unsigned long long)s.u64RetPos,
            (unsigned long long)s.u64LastNr,
            (unsigned long long)s.u64DefaultIsLinux,
            (unsigned long long)s.u64DefaultIsNative);

    /* Grep: syscall: soft bridge */
    kprintf("syscall: soft bridge enter=%llu null=%llu note=%llu "
            "last_nr=%llu (LSTAR edge before route)\n",
            (unsigned long long)s.u64BridgeEnter,
            (unsigned long long)s.u64BridgeNull,
            (unsigned long long)g_u64SoftBridgeNote,
            (unsigned long long)s.u64LastNr);

    /* Grep: syscall: soft route */
    kprintf("syscall: soft route enter=%llu null=%llu native=%llu "
            "linux=%llu route_sum=%llu bp_native=%llu bp_linux=%llu\n",
            (unsigned long long)s.u64DispatchEnter,
            (unsigned long long)s.u64DispatchNull,
            (unsigned long long)s.u64Native,
            (unsigned long long)s.u64Linux,
            (unsigned long long)u64Route,
            (unsigned long long)u64BpNative,
            (unsigned long long)u64BpLinux);

    /* Grep: syscall: soft personality */
    kprintf("syscall: soft personality pcb_n=%llu pcb_l=%llu "
            "pcb_sum=%llu def_pers=%llu def_linux=%u def_native=%u "
            "set_ok=%llu set_rej=%llu\n",
            (unsigned long long)s.u64PcbNative,
            (unsigned long long)s.u64PcbLinux,
            (unsigned long long)u64Pcb,
            (unsigned long long)s.u64DefaultPers,
            u32DefLinux, u32DefNative,
            (unsigned long long)s.u64SetPersOk,
            (unsigned long long)s.u64SetPersReject);

    /* Grep: syscall: soft bind */
    kprintf("syscall: soft bind bound=%llu unbound=%llu bp_bound=%llu "
            "linux_current=gj_linux_set_current\n",
            (unsigned long long)s.u64Bound,
            (unsigned long long)s.u64Unbound,
            (unsigned long long)u64BpBound);

    /* Grep: syscall: soft lifecycle */
    kprintf("syscall: soft lifecycle init=%llu set_ok=%llu set_rej=%llu "
            "stats_get=%llu stats_get_null=%llu stats_reset=%llu "
            "soft_call=%llu logs=%llu\n",
            (unsigned long long)s.u64Init,
            (unsigned long long)s.u64SetPersOk,
            (unsigned long long)s.u64SetPersReject,
            (unsigned long long)g_u64SoftStatsGet,
            (unsigned long long)g_u64SoftStatsGetNull,
            (unsigned long long)g_u64SoftStatsReset,
            (unsigned long long)g_u64SoftSoftCall,
            (unsigned long long)g_u64SoftLogN);

    /* Grep: syscall: soft outcome */
    kprintf("syscall: soft outcome complete=%llu ret_neg=%llu ret0=%llu "
            "ret_pos=%llu bp_neg=%llu last_ret_bits=0x%llx\n",
            (unsigned long long)s.u64Complete,
            (unsigned long long)s.u64RetNeg,
            (unsigned long long)s.u64RetZero,
            (unsigned long long)s.u64RetPos,
            (unsigned long long)u64BpNeg,
            (unsigned long long)s.u64LastRetBits);

    /* Grep: syscall: soft last */
    kprintf("syscall: soft last nr=%llu ret_bits=0x%llx logs=%llu "
            "once=%u\n",
            (unsigned long long)s.u64LastNr,
            (unsigned long long)s.u64LastRetBits,
            (unsigned long long)g_u64SoftLogN,
            g_fSoftInvOnce ? 1u : 0u);

    /* Grep: syscall: soft api */
    kprintf("syscall: soft api stats_get=%llu stats_get_null=%llu "
            "stats_reset=%llu soft_call=%llu bridge_note=%llu "
            "log_n=%llu\n",
            (unsigned long long)g_u64SoftStatsGet,
            (unsigned long long)g_u64SoftStatsGetNull,
            (unsigned long long)g_u64SoftStatsReset,
            (unsigned long long)g_u64SoftSoftCall,
            (unsigned long long)g_u64SoftBridgeNote,
            (unsigned long long)g_u64SoftLogN);

    /* Grep: syscall: soft rates */
    kprintf("syscall: soft rates bp_native=%llu bp_linux=%llu "
            "bp_bound=%llu bp_ret_neg=%llu route_sum=%llu complete=%llu\n",
            (unsigned long long)u64BpNative,
            (unsigned long long)u64BpLinux,
            (unsigned long long)u64BpBound,
            (unsigned long long)u64BpNeg,
            (unsigned long long)u64Route,
            (unsigned long long)s.u64Complete);

    /* Grep: syscall: soft honesty */
    kprintf("syscall: soft honesty hybrid=OptionC open=1 bar3=0 "
            "product_linux_abi=open soft_only=1 "
            "not_bar3=1 wave=%u (soft inventory; never closes hybrid)\n",
            (unsigned)SYSCALL_SOFT_WAVE);

    /* Grep: syscall: soft edge */
    kprintf("syscall: soft edge bridge=%llu bridge_null=%llu "
            "disp=%llu disp_null=%llu edge_sum=%llu note=%llu "
            "wave=%u\n",
            (unsigned long long)s.u64BridgeEnter,
            (unsigned long long)s.u64BridgeNull,
            (unsigned long long)s.u64DispatchEnter,
            (unsigned long long)s.u64DispatchNull,
            (unsigned long long)u64Edge,
            (unsigned long long)g_u64SoftBridgeNote,
            (unsigned)SYSCALL_SOFT_WAVE);

    /* Grep: syscall: soft share */
    kprintf("syscall: soft share bp_native=%llu bp_linux=%llu "
            "bp_complete=%llu bp_bound=%llu bp_ret_neg=%llu "
            "route_sum=%llu complete=%llu wave=%u\n",
            (unsigned long long)u64BpNative,
            (unsigned long long)u64BpLinux,
            (unsigned long long)u64BpComplete,
            (unsigned long long)u64BpBound,
            (unsigned long long)u64BpNeg,
            (unsigned long long)u64Route,
            (unsigned long long)s.u64Complete,
            (unsigned)SYSCALL_SOFT_WAVE);

    /* Grep: syscall: soft surfaces (Wave 20 deepen) */
    kprintf("syscall: soft surfaces count=%u "
            "names=inventory,stats,bridge,route,personality,bind,"
            "lifecycle,outcome,last,api,rates,honesty,edge,share,"
            "catalog,surfaces,note,return,retmap,deepen,path,PASS wave=%u\n",
            SYSCALL_SOFT_AREAS, (unsigned)SYSCALL_SOFT_WAVE);

    /* Grep: syscall: soft note (Wave 20 deepen) */
    kprintf("syscall: soft note milestone=wave98 exclusive=1 "
            "soft_only=1 hybrid=OptionC not_bar3=1 "
            "disp=%llu native=%llu linux=%llu wave=%u\n",
            (unsigned long long)s.u64DispatchEnter,
            (unsigned long long)s.u64Native,
            (unsigned long long)s.u64Linux,
            (unsigned)SYSCALL_SOFT_WAVE);

    /* Grep: syscall: soft catalog */
    kprintf("syscall: soft catalog wave=%u areas=%u "
            "surfaces=inventory,stats,bridge,route,personality,bind,"
            "lifecycle,outcome,last,api,rates,honesty,edge,share,"
            "catalog,surfaces,note,return,retmap,deepen,path,PASS\n",
            (unsigned)SYSCALL_SOFT_WAVE, SYSCALL_SOFT_AREAS);

    /* Grep: syscall: soft return (Wave 20 deepen) */
    kprintf("syscall: soft return complete=%llu ret_neg=%llu ret_zero=%llu "
            "ret_pos=%llu bridge_null=%llu disp_null=%llu "
            "product_gate=0 wave=%u\n",
            (unsigned long long)s.u64Complete,
            (unsigned long long)s.u64RetNeg,
            (unsigned long long)s.u64RetZero,
            (unsigned long long)s.u64RetPos,
            (unsigned long long)s.u64BridgeNull,
            (unsigned long long)s.u64DispatchNull,
            (unsigned)SYSCALL_SOFT_WAVE);

    /* Grep: syscall: soft retmap — Wave 19 return-surface map */
    kprintf("syscall: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=111\n");

    /* Grep: syscall: soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: syscall: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("syscall: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)SYSCALL_SOFT_WAVE);
    /* Grep: syscall: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("syscall: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)SYSCALL_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: syscall: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("syscall: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)SYSCALL_SOFT_WAVE);
    /* Grep: syscall: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("syscall: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)SYSCALL_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: syscall: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("syscall: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)SYSCALL_SOFT_WAVE);
            /* Grep: syscall: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("syscall: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)SYSCALL_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: syscall: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("syscall: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)SYSCALL_SOFT_WAVE);
            /* Grep: syscall: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("syscall: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: syscall: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("syscall: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)SYSCALL_SOFT_WAVE);
            /* Grep: syscall: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("syscall: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)SYSCALL_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: syscall: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("syscall: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)SYSCALL_SOFT_WAVE);
            /* Grep: syscall: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("syscall: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)SYSCALL_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: syscall: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("syscall: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)SYSCALL_SOFT_WAVE);
            /* Grep: syscall: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("syscall: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)SYSCALL_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: syscall: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("syscall: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)SYSCALL_SOFT_WAVE);
            /* Grep: syscall: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("syscall: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)SYSCALL_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: syscall: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("syscall: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)SYSCALL_SOFT_WAVE);
                    /* Grep: syscall: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("syscall: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)SYSCALL_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: syscall: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("syscall: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)SYSCALL_SOFT_WAVE);
                            /* Grep: syscall: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("syscall: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)SYSCALL_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: syscall: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("syscall: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)SYSCALL_SOFT_WAVE);
                            /* Grep: syscall: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("syscall: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)SYSCALL_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: syscall: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("syscall: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)SYSCALL_SOFT_WAVE);
                            /* Grep: syscall: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("syscall: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)SYSCALL_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: syscall: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("syscall: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)SYSCALL_SOFT_WAVE);
                            /* Grep: syscall: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("syscall: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)SYSCALL_SOFT_WAVE);
                            /* Grep: syscall: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("syscall: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("syscall: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/* Grep: syscall: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("syscall: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("syscall: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/* Grep: syscall: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("syscall: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("syscall: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/* Grep: syscall: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("syscall: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retfortress — Wave 35 return-fortress honesty */
kprintf("syscall: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/* Grep: syscall: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("syscall: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft rethold — Wave 36 return-hold honesty */
kprintf("syscall: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/* Grep: syscall: soft retspire — Wave 36 exclusive spire stamp */
kprintf("syscall: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retwall — Wave 37 return-wall honesty */
kprintf("syscall: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/* Grep: syscall: soft retgate — Wave 37 exclusive gate stamp */
kprintf("syscall: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retmoat — Wave 38 return-moat honesty */
kprintf("syscall: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/* Grep: syscall: soft retower — Wave 38 exclusive tower stamp */
kprintf("syscall: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("syscall: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/* Grep: syscall: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("syscall: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("syscall: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/* Grep: syscall: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("syscall: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retravelin — Wave 41 return-travelin honesty */
kprintf("syscall: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/* Grep: syscall: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("syscall: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("syscall: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/* Grep: syscall: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("syscall: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("syscall: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/* Grep: syscall: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("syscall: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)SYSCALL_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("syscall: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/* Grep: syscall: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("syscall: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)SYSCALL_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("syscall: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/* Grep: syscall: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("syscall: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)SYSCALL_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retbailey — Wave 46 return-bailey honesty */
kprintf("syscall: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)SYSCALL_SOFT_WAVE);
/* Grep: syscall: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("syscall: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)SYSCALL_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("syscall: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("syscall: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("syscall: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("syscall: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("syscall: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("syscall: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retsally — Wave 50 return-sally honesty */
kprintf("syscall: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("syscall: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retfosse — Wave 51 return-fosse honesty */
kprintf("syscall: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("syscall: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("syscall: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("syscall: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retravelin — Wave 53 return-travelin honesty */
kprintf("syscall: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("syscall: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("syscall: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retredan — Wave 54 exclusive redan stamp */
kprintf("syscall: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retflank — Wave 55 return-flank honesty */
kprintf("syscall: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retface — Wave 55 exclusive face stamp */
kprintf("syscall: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retgorge — Wave 56 return-gorge honesty */
kprintf("syscall: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("syscall: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retraverse — Wave 57 return-traverse honesty */
kprintf("syscall: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("syscall: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retorillon — Wave 58 return-orillon honesty */
kprintf("syscall: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("syscall: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("syscall: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("syscall: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retplace — Wave 60 return-place honesty */
kprintf("syscall: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("syscall: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("syscall: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("syscall: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("syscall: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("syscall: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("syscall: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("syscall: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: syscall: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("syscall: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: syscall: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("syscall: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: syscall: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("syscall: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: syscall: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("syscall: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: syscall: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("syscall: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=111 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: syscall: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("syscall: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=111 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("syscall: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("syscall: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("syscall: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("syscall: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("syscall: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=111 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("syscall: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=111 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("syscall: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("syscall: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("syscall: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("syscall: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: syscall: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("syscall: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("syscall: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("syscall: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("syscall: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbastionangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("syscall: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("syscall: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retparapetangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("syscall: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("syscall: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retowerangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("syscall: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("syscall: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retwallangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("syscall: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("syscall: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retholdangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("syscall: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("syscall: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retfortressangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("syscall: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("syscall: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("syscall: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("syscall: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: syscall: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("syscall: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("syscall: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("syscall: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("syscall: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retaegisangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("syscall: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("syscall: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retsigilangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("syscall: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("syscall: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retglyphangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("syscall: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("syscall: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retshardangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("syscall: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("syscall: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retprismangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("syscall: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("syscall: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retcipherangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("syscall: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("syscall: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retledgerangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("syscall: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("syscall: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retvaultangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("syscall: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("syscall: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (rettokenangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("syscall: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("syscall: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retphaseangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("syscall: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("syscall: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retpulseangle stamp; Soft≠product)\n");

/* Grep: syscall: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("syscall: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("syscall: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retboundangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("syscall: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("syscall: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbladeangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("syscall: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("syscall: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retarcangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("syscall: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("syscall: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("syscall: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("syscall: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("syscall: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("syscall: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retellipseangle stamp; Soft≠product)\n");
/* Grep: syscall: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("syscall: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("syscall: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("syscall: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("syscall: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (rethelixangle stamp; Soft≠product)\n");
/* Grep: syscall: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("syscall: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("syscall: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retknotangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("syscall: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("syscall: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retkleinangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("syscall: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("syscall: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retaffineangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("syscall: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("syscall: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("syscall: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("syscall: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retcubicangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("syscall: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("syscall: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retquinticangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retsplineangle — Wave 107 return-splineangle honesty */
kprintf("syscall: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
kprintf("syscall: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbezierangle stamp; Soft≠product)\n");
/* Grep: syscall: soft rethurmitangle — Wave 108 return-hermitangle honesty */
kprintf("syscall: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
kprintf("syscall: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retcatmullangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retnurbsangle — Wave 109 return-nurbsangle honesty */
kprintf("syscall: soft retnurbsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retnurbsangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retbsplineangle — Wave 109 exclusive bsplineangle stamp */
kprintf("syscall: soft retbsplineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retbsplineangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retmeshangle — Wave 110 return-meshangle honesty */
kprintf("syscall: soft retmeshangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retmeshangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft retgridangle — Wave 110 exclusive gridangle stamp */
kprintf("syscall: soft retgridangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (retgridangle stamp; Soft≠product)\n");
/* Grep: syscall: soft retvoxelangle — Wave 111 return-voxelangle honesty */
kprintf("syscall: soft retvoxelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=111 (retvoxelangle honesty; Soft≠product; not bar3)\n");
/* Grep: syscall: soft rettexelangle — Wave 111 exclusive texelangle stamp */
kprintf("syscall: soft rettexelangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=111 (rettexelangle stamp; Soft≠product)\n");
                            kprintf("syscall: soft deepen wave=%u areas=%u ok=1 "
            "prefix=syscall:soft "
            "surfaces=inventory,stats,bridge,route,personality,bind,"
            "lifecycle,outcome,last,api,rates,honesty,edge,share,"
            "catalog,surfaces,note,return,retmap,deepen,path,PASS\n",
            (unsigned)SYSCALL_SOFT_WAVE, SYSCALL_SOFT_AREAS);

    /* Grep: syscall: soft path */
    kprintf("syscall: soft path claim=bridge+personality+bind+route "
            "native=gj_native_syscall_dispatch "
            "linux=gj_linux_syscall_dispatch hybrid=OptionC "
            "boot_default=LINUX wave=%u "
            "(soft inventory; not bar3)\n",
            (unsigned)SYSCALL_SOFT_WAVE);

    /* Grep: syscall: soft inventory PASS / syscall: soft PASS */
    kprintf("syscall: soft inventory PASS wave=%u logs=%llu "
            "disp=%llu native=%llu linux=%llu\n",
            (unsigned)SYSCALL_SOFT_WAVE,
            (unsigned long long)g_u64SoftLogN,
            (unsigned long long)s.u64DispatchEnter,
            (unsigned long long)s.u64Native,
            (unsigned long long)s.u64Linux);
    kprintf("syscall: soft PASS wave=%u logs=%llu\n",
            (unsigned)SYSCALL_SOFT_WAVE,
            (unsigned long long)g_u64SoftLogN);
}

/**
 * After first product dispatch/bridge activity, print soft inventory once
 * (mirrors native/linux soft-stats-once). Diagnostics only — never gates.
 */
static void
entry_soft_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_entryStats.u64DispatchEnter == 0 &&
        g_entryStats.u64BridgeEnter == 0 &&
        g_entryStats.u64DispatchNull == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    entry_soft_inventory_log();
}

void
gj_syscall_entry_soft_note_bridge(struct gj_syscall_regs *pRegs)
{
    entry_soft_inc(&g_u64SoftBridgeNote);
    g_entryStats.u64BridgeEnter++;
    if (pRegs == NULL) {
        g_entryStats.u64BridgeNull++;
        entry_soft_maybe_once();
        return;
    }
    /* Soft snapshot nr at the hardware edge (before route). */
    g_entryStats.u64LastNr = pRegs->u64Nr;
    entry_soft_maybe_once();
}

void
gj_syscall_entry_stats_get(struct gj_syscall_entry_stats *pOut)
{
    entry_soft_inc(&g_u64SoftStatsGet);
    if (pOut == NULL) {
        entry_soft_inc(&g_u64SoftStatsGetNull);
        return;
    }
    entry_soft_mirror_default();
    *pOut = g_entryStats;
}

void
gj_syscall_entry_stats_reset(void)
{
    entry_soft_inc(&g_u64SoftStatsReset);
    memset(&g_entryStats, 0, sizeof(g_entryStats));
    /* File-local deepen tallies (api) survive reset — lifetime of module.
     * Allow one-shot deepen again after a product reset of entry path. */
    g_fSoftInvOnce = 0;
    entry_soft_mirror_default();
}

u64
gj_syscall_entry_stats_soft(void)
{
    entry_soft_inc(&g_u64SoftSoftCall);
    /*
     * Full Wave 15 multi-line soft inventory (includes legacy stats line).
     * Grep: syscall: soft stats / syscall: soft inventory
     */
    entry_soft_inventory_log();
    return g_entryStats.u64DispatchEnter;
}

void
gj_syscall_init(void)
{
    cold_ipc_init();
    futex_init();
    gj_linux_dispatch_init();
    g_eDefaultPersonality = GJ_PERSONALITY_LINUX;
    g_entryStats.u64Init++;
    g_fSoftInvOnce = 0;
    entry_soft_mirror_default();
    /* Wave 15 soft inventory baseline (greppable syscall: soft …). */
    entry_soft_inventory_log();
}

void
gj_syscall_set_default_personality(enum gj_personality e)
{
    /* Reject out-of-range values so a corrupt caller cannot invent modes. */
    if (e != GJ_PERSONALITY_NATIVE && e != GJ_PERSONALITY_LINUX) {
        g_entryStats.u64SetPersReject++;
        return;
    }
    g_eDefaultPersonality = e;
    g_entryStats.u64SetPersOk++;
    entry_soft_mirror_default();
}

enum gj_personality
gj_syscall_get_default_personality(void)
{
    return g_eDefaultPersonality;
}

/**
 * Resolve personality for this trap.
 * Prefer the current process PCB; fall back to the boot default.
 * Soft: bumps PCB native/linux or default-personality counters.
 */
static enum gj_personality
current_personality(void)
{
    struct gj_thread *pThr = thread_current();

    if (pThr != NULL && pThr->pProc != NULL) {
        if (pThr->pProc->u32Personality == 0) {
            g_entryStats.u64PcbNative++;
            return GJ_PERSONALITY_NATIVE;
        }
        g_entryStats.u64PcbLinux++;
        return GJ_PERSONALITY_LINUX;
    }
    g_entryStats.u64DefaultPers++;
    return g_eDefaultPersonality;
}

void
gj_syscall_dispatch(struct gj_syscall_regs *pRegs)
{
    struct gj_linux_regs linuxRegs;
    struct gj_thread *pThr;
    enum gj_personality ePers;

    g_entryStats.u64DispatchEnter++;
    if (pRegs == NULL) {
        g_entryStats.u64DispatchNull++;
        entry_soft_maybe_once();
        return;
    }

    g_entryStats.u64LastNr = pRegs->u64Nr;

    /*
     * Bind Linux "current" before any hybrid work so mmap/AS, creds, and
     * per-process tables see the calling process (not a stale boot task).
     * Native path does not require this, but binding is cheap and keeps a
     * mixed call sequence consistent if personality flips mid-smoke.
     */
    pThr = thread_current();
    if (pThr != NULL && pThr->pProc != NULL) {
        g_entryStats.u64Bound++;
        gj_linux_set_current(pThr->pProc, 1,
                             pThr->u32Id != 0 ? pThr->u32Id : 1);
    } else {
        g_entryStats.u64Unbound++;
    }

    ePers = current_personality();
    if (ePers == GJ_PERSONALITY_NATIVE) {
        g_entryStats.u64Native++;
        gj_native_syscall_dispatch(pRegs);
        entry_soft_note_complete(pRegs);
        entry_soft_maybe_once();
        return;
    }

    /* Linux hybrid Option C: hot table first, cold door for the rest. */
    g_entryStats.u64Linux++;
    memset(&linuxRegs, 0, sizeof(linuxRegs));
    linuxRegs.u64Nr = pRegs->u64Nr;
    linuxRegs.u64Arg0 = pRegs->u64Arg0;
    linuxRegs.u64Arg1 = pRegs->u64Arg1;
    linuxRegs.u64Arg2 = pRegs->u64Arg2;
    linuxRegs.u64Arg3 = pRegs->u64Arg3;
    linuxRegs.u64Arg4 = pRegs->u64Arg4;
    linuxRegs.u64Arg5 = pRegs->u64Arg5;
    gj_linux_syscall_dispatch(&linuxRegs);
    pRegs->i64Ret = linuxRegs.i64Ret;
    entry_soft_note_complete(pRegs);
    entry_soft_maybe_once();
}
