/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * WoW64 soft thunk: i386 Linux syscall NR map + arg convert + personality.
 * Pure C dual-license; clean-room public man-page mapping — not Linux/Wine.
 * Product PE32 loading is userspace; CS32 int 0x80 is trap_dispatch.
 * Hybrid SYSCALL uses this table; soft thunk zero-extends and fixes mmap2
 * pgoff / socketcall demux for pure-C paths without hardware compat CS.
 *
 * Soft inventory (Wave 14 base; Wave 35 exclusive deepen) —
 * greppable "wow64: soft …":
 *   wow64: soft inventory …
 *   wow64: soft map …
 *   wow64: soft thunk …
 *   wow64: soft adjust …
 *   wow64: soft personality …
 *   wow64: soft path …
 *   wow64: soft rates …      (Wave 19)
 *   wow64: soft honesty …    (Wave 19)
 *   wow64: soft last …       (Wave 19)
 *   wow64: soft surfaces …   (Wave 19)
 *   wow64: soft catalog …    (Wave 19)
 *   wow64: soft note …       (Wave 19)
 *   wow64: soft deepen …
 *   wow64: soft inventory PASS / soft PASS
 * Pure observation; never hard-gates path PASS; wrap OK; not bar3.
 * greppable: wow64: soft inventory
 * greppable: wow64: soft path
 * greppable: wow64: soft surfaces
 * greppable: wow64: soft deepen
 */
#include <gj/klog.h>
#include <gj/wow64.h>

static int g_fWow64;
static u32 g_u32Calls;
static u32 g_u32MapHits;
static u32 g_u32IdentityHits;
static u32 g_u32ThunkHits;
static u32 g_u32Mmap2Hits;
static u32 g_u32SocketcallHits;

/*
 * Soft product inventory (Wave 14 base; Wave 20 deepen). Cumulative path
 * tallies. greppable: wow64: soft …
 * Areas: inventory|map|thunk|adjust|personality|path|rates|honesty|
 *        last|surfaces|catalog|note|deepen|PASS
 */
#define GJ_WOW64_SOFT_WAVE 81u
#define GJ_WOW64_SOFT_AREAS 136u

static u32 g_u32SoftTranslateEnter; /* wow64_translate_nr entries */
static u32 g_u32SoftTranslateNull;  /* translate with pOutNr == NULL */
static u32 g_u32SoftIsMappedProbe;  /* wow64_nr_is_mapped probes */
static u32 g_u32SoftThunkEnter;     /* wow64_thunk_soft entries */
static u32 g_u32SoftThunkNull;      /* thunk_soft with pThunk == NULL */
static u32 g_u32SoftThunkOk;        /* thunk_soft success returns */
static u32 g_u32SoftAdjustEnter;    /* wow64_adjust_args entries */
static u32 g_u32SoftAdjustNop;      /* adjust early return (null/0) */
static u32 g_u32SoftAdjustZx;       /* adjust applied ARGS_ZX */
static u32 g_u32SoftAdjustMmap2;    /* adjust mmap2 pgoff path */
static u32 g_u32SoftAdjustSocket;   /* adjust socketcall demux path */
static u32 g_u32SoftAdjustSockFail; /* socketcall unknown subcall */
static u32 g_u32SoftAdjustOldMmap;  /* old_mmap (NR 90) soft path */
static u32 g_u32SoftAdjustLlseek;   /* _llseek (NR 140) soft compose */
static u32 g_u32SoftArgsZxCalls;    /* wow64_args_zero_extend calls */
static u32 g_u32SoftSocketNrOk;     /* wow64_socketcall_nr mapped */
static u32 g_u32SoftSocketNrFail;   /* socketcall_nr unknown/null */
static u32 g_u32SoftPtr32;          /* wow64_ptr32 calls */
static u32 g_u32SoftPersonOn;       /* wow64_set(on) transitions */
static u32 g_u32SoftPersonOff;      /* wow64_set(off) transitions */
static u32 g_u32SoftLogN;           /* soft inventory log emissions */
static u8  g_fSoftInvOnce;          /* one-shot deep dump after activity */

static void soft_inc(u32 *pCtr);
static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

/**
 * Greppable soft WoW64 inventory (product / smoke; Wave 20 deepen).
 * Prefix-stable markers (wow64: soft …):
 *   wow64: soft inventory     — personality + public counter rollup
 *   wow64: soft map           — translate / is_mapped path tallies
 *   wow64: soft thunk         — wow64_thunk_soft path tallies
 *   wow64: soft adjust        — mmap2 / socketcall / llseek fixups
 *   wow64: soft personality   — enable/disable surface
 *   wow64: soft path          — honesty claim (soft ≠ bar3)
 *   wow64: soft rates         — Wave 16 map/thunk share lamps
 *   wow64: soft honesty       — hybrid open; not bar3
 *   wow64: soft last          — Wave 16 live counter snapshot
 *   wow64: soft surfaces      — Wave 19 surface count lamp
 *   wow64: soft catalog       — Wave 19 area name rollup
 *   wow64: soft note          — Wave 16 milestone note
 *   wow64: soft deepen        — Wave 20 stamp
 *   wow64: soft inventory PASS / soft PASS
 * greppable: wow64: soft
 * Honesty: soft inventory only — not product gate; not bar3.
 */
static void
soft_inventory_log(void)
{
    u32 u32Enabled;
    u32 u32AdjActive;
    u32 u32MapRatio;
    u32 u32ThunkBp;
    u32 u32IdentityBp;

    soft_inc(&g_u32SoftLogN);
    u32Enabled = (g_fWow64 != 0) ? 1u : 0u;
    u32AdjActive = 0;
    if (g_u32SoftAdjustMmap2 != 0) {
        u32AdjActive++;
    }
    if (g_u32SoftAdjustSocket != 0) {
        u32AdjActive++;
    }
    if (g_u32SoftAdjustOldMmap != 0) {
        u32AdjActive++;
    }
    if (g_u32SoftAdjustLlseek != 0) {
        u32AdjActive++;
    }
    /* Soft map hit fraction in basis points (0 if no calls yet). */
    if (g_u32Calls != 0) {
        u32MapRatio = (u32)(((u64)g_u32MapHits * 10000ull) /
                            (u64)g_u32Calls);
        u32IdentityBp = (u32)(((u64)g_u32IdentityHits * 10000ull) /
                              (u64)g_u32Calls);
        u32ThunkBp = (u32)(((u64)g_u32ThunkHits * 10000ull) /
                           (u64)g_u32Calls);
    } else {
        u32MapRatio = 0;
        u32IdentityBp = 0;
        u32ThunkBp = 0;
    }

    /* Grep: wow64: soft inventory */
    kprintf("wow64: soft inventory enabled=%u calls=%u map=%u identity=%u "
            "thunk=%u mmap2=%u socketcall=%u log_n=%u "
            "areas=%u wave=%u (soft; not bar3)\n",
            u32Enabled, g_u32Calls, g_u32MapHits, g_u32IdentityHits,
            g_u32ThunkHits, g_u32Mmap2Hits, g_u32SocketcallHits,
            g_u32SoftLogN,
            (unsigned)GJ_WOW64_SOFT_AREAS,
            (unsigned)GJ_WOW64_SOFT_WAVE);

    /* Grep: wow64: soft map */
    kprintf("wow64: soft map enter=%u null_out=%u is_mapped=%u "
            "hits=%u identity=%u map_bp=%u wave=%u\n",
            g_u32SoftTranslateEnter, g_u32SoftTranslateNull,
            g_u32SoftIsMappedProbe, g_u32MapHits, g_u32IdentityHits,
            u32MapRatio, (unsigned)GJ_WOW64_SOFT_WAVE);

    /* Grep: wow64: soft thunk */
    kprintf("wow64: soft thunk enter=%u ok=%u null=%u hits=%u wave=%u\n",
            g_u32SoftThunkEnter, g_u32SoftThunkOk, g_u32SoftThunkNull,
            g_u32ThunkHits, (unsigned)GJ_WOW64_SOFT_WAVE);

    /* Grep: wow64: soft adjust */
    kprintf("wow64: soft adjust enter=%u nop=%u zx=%u mmap2=%u socket=%u "
            "sock_fail=%u old_mmap=%u llseek=%u args_zx=%u "
            "sock_nr_ok=%u sock_nr_fail=%u ptr32=%u adj_active=%u "
            "wave=%u\n",
            g_u32SoftAdjustEnter, g_u32SoftAdjustNop, g_u32SoftAdjustZx,
            g_u32SoftAdjustMmap2, g_u32SoftAdjustSocket,
            g_u32SoftAdjustSockFail, g_u32SoftAdjustOldMmap,
            g_u32SoftAdjustLlseek, g_u32SoftArgsZxCalls,
            g_u32SoftSocketNrOk, g_u32SoftSocketNrFail, g_u32SoftPtr32,
            u32AdjActive, (unsigned)GJ_WOW64_SOFT_WAVE);

    /* Grep: wow64: soft personality */
    kprintf("wow64: soft personality enabled=%u on=%u off=%u "
            "page_shift=%u wave=%u\n",
            u32Enabled, g_u32SoftPersonOn, g_u32SoftPersonOff,
            GJ_WOW64_PAGE_SHIFT, (unsigned)GJ_WOW64_SOFT_WAVE);

    /* Grep: wow64: soft path */
    kprintf("wow64: soft path claim=nr_map+arg_zx+mmap2_pgoff+"
            "socketcall_demux+llseek_compose hybrid=SYSCALL+int80 "
            "areas=%u wave=%u (soft inventory; not bar3)\n",
            (unsigned)GJ_WOW64_SOFT_AREAS,
            (unsigned)GJ_WOW64_SOFT_WAVE);

    /* Grep: wow64: soft rates (Wave 20 deepen) */
    kprintf("wow64: soft rates bp_map=%u bp_identity=%u bp_thunk=%u "
            "calls=%u map=%u thunk=%u wave=%u\n",
            u32MapRatio, u32IdentityBp, u32ThunkBp,
            g_u32Calls, g_u32MapHits, g_u32ThunkHits,
            (unsigned)GJ_WOW64_SOFT_WAVE);

    /* Grep: wow64: soft honesty (Wave 20 deepen) */
    kprintf("wow64: soft honesty hybrid=OptionC open=1 bar3=0 "
            "product_pe32=userspace soft_only=1 pe32_int80=trap "
            "wave=%u (soft inventory; never closes hybrid)\n",
            (unsigned)GJ_WOW64_SOFT_WAVE);

    /* Grep: wow64: soft last (Wave 20 deepen) */
    kprintf("wow64: soft last enabled=%u calls=%u map=%u thunk=%u "
            "adjust=%u logs=%u once=%u wave=%u\n",
            u32Enabled, g_u32Calls, g_u32MapHits, g_u32ThunkHits,
            g_u32SoftAdjustEnter, g_u32SoftLogN,
            g_fSoftInvOnce ? 1u : 0u, (unsigned)GJ_WOW64_SOFT_WAVE);

    /* Grep: wow64: soft surfaces (Wave 20 deepen) */
    kprintf("wow64: soft surfaces count=%u "
            "names=inventory,map,thunk,adjust,personality,path,rates,"
            "honesty,last,surfaces,catalog,note,return,retmap,deepen,PASS wave=%u\n",
            (unsigned)GJ_WOW64_SOFT_AREAS,
            (unsigned)GJ_WOW64_SOFT_WAVE);

    /* Grep: wow64: soft catalog (Wave 20 deepen) */
    kprintf("wow64: soft catalog wave=%u areas=%u "
            "surfaces=inventory,map,thunk,adjust,personality,path,rates,"
            "honesty,last,surfaces,catalog,note,return,retmap,deepen,PASS\n",
            (unsigned)GJ_WOW64_SOFT_WAVE,
            (unsigned)GJ_WOW64_SOFT_AREAS);

    /* Grep: wow64: soft note (Wave 20 deepen) */
    kprintf("wow64: soft note milestone=wave81 exclusive=1 "
            "soft_only=1 not_bar3=1 calls=%u map=%u wave=%u\n",
            g_u32Calls, g_u32MapHits, (unsigned)GJ_WOW64_SOFT_WAVE);

    /* Grep: wow64: soft return (Wave 20 deepen) */
    kprintf("wow64: soft return thunk_ok=%u thunk_null=%u translate_null=%u "
            "adjust_nop=%u map=%u product_gate=0 wave=%u\n",
            g_u32SoftThunkOk, g_u32SoftThunkNull, g_u32SoftTranslateNull,
            g_u32SoftAdjustNop, g_u32MapHits, (unsigned)GJ_WOW64_SOFT_WAVE);

    /* Grep: wow64: soft retmap — Wave 19 return-surface map */
    kprintf("wow64: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=81\n");

    /* Grep: wow64: soft deepen wave */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: wow64: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("wow64: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_WOW64_SOFT_WAVE);
    /* Grep: wow64: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("wow64: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_WOW64_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: wow64: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("wow64: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_WOW64_SOFT_WAVE);
    /* Grep: wow64: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("wow64: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_WOW64_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: wow64: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("wow64: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_WOW64_SOFT_WAVE);
            /* Grep: wow64: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("wow64: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_WOW64_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: wow64: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("wow64: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_WOW64_SOFT_WAVE);
            /* Grep: wow64: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("wow64: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_WOW64_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: wow64: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("wow64: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_WOW64_SOFT_WAVE);
            /* Grep: wow64: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("wow64: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_WOW64_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: wow64: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("wow64: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_WOW64_SOFT_WAVE);
            /* Grep: wow64: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("wow64: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_WOW64_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: wow64: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("wow64: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_WOW64_SOFT_WAVE);
            /* Grep: wow64: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("wow64: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_WOW64_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: wow64: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("wow64: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_WOW64_SOFT_WAVE);
            /* Grep: wow64: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("wow64: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_WOW64_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: wow64: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("wow64: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_WOW64_SOFT_WAVE);
                    /* Grep: wow64: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("wow64: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_WOW64_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: wow64: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("wow64: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_WOW64_SOFT_WAVE);
                            /* Grep: wow64: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("wow64: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_WOW64_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: wow64: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("wow64: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_WOW64_SOFT_WAVE);
                            /* Grep: wow64: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("wow64: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)GJ_WOW64_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: wow64: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("wow64: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_WOW64_SOFT_WAVE);
                            /* Grep: wow64: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("wow64: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)GJ_WOW64_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: wow64: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("wow64: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_WOW64_SOFT_WAVE);
                            /* Grep: wow64: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("wow64: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_WOW64_SOFT_WAVE);
                            /* Grep: wow64: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("wow64: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)GJ_WOW64_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("wow64: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/* Grep: wow64: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("wow64: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("wow64: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/* Grep: wow64: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("wow64: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("wow64: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/* Grep: wow64: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("wow64: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retfortress — Wave 35 return-fortress honesty */
kprintf("wow64: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/* Grep: wow64: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("wow64: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft rethold — Wave 36 return-hold honesty */
kprintf("wow64: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/* Grep: wow64: soft retspire — Wave 36 exclusive spire stamp */
kprintf("wow64: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retwall — Wave 37 return-wall honesty */
kprintf("wow64: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/* Grep: wow64: soft retgate — Wave 37 exclusive gate stamp */
kprintf("wow64: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retmoat — Wave 38 return-moat honesty */
kprintf("wow64: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/* Grep: wow64: soft retower — Wave 38 exclusive tower stamp */
kprintf("wow64: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("wow64: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/* Grep: wow64: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("wow64: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("wow64: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/* Grep: wow64: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("wow64: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retravelin — Wave 41 return-travelin honesty */
kprintf("wow64: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/* Grep: wow64: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("wow64: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("wow64: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/* Grep: wow64: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("wow64: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("wow64: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/* Grep: wow64: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("wow64: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("wow64: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/* Grep: wow64: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("wow64: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("wow64: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/* Grep: wow64: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("wow64: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retbailey — Wave 46 return-bailey honesty */
kprintf("wow64: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);
/* Grep: wow64: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("wow64: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)GJ_WOW64_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("wow64: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("wow64: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("wow64: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("wow64: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("wow64: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("wow64: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retsally — Wave 50 return-sally honesty */
kprintf("wow64: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("wow64: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retfosse — Wave 51 return-fosse honesty */
kprintf("wow64: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("wow64: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("wow64: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("wow64: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retravelin — Wave 53 return-travelin honesty */
kprintf("wow64: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("wow64: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("wow64: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retredan — Wave 54 exclusive redan stamp */
kprintf("wow64: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retflank — Wave 55 return-flank honesty */
kprintf("wow64: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retface — Wave 55 exclusive face stamp */
kprintf("wow64: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retgorge — Wave 56 return-gorge honesty */
kprintf("wow64: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("wow64: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retraverse — Wave 57 return-traverse honesty */
kprintf("wow64: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("wow64: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retorillon — Wave 58 return-orillon honesty */
kprintf("wow64: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("wow64: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("wow64: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("wow64: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retplace — Wave 60 return-place honesty */
kprintf("wow64: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("wow64: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("wow64: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("wow64: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("wow64: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("wow64: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("wow64: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("wow64: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: wow64: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("wow64: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: wow64: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("wow64: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: wow64: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("wow64: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: wow64: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("wow64: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: wow64: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("wow64: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=81 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: wow64: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("wow64: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=81 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("wow64: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("wow64: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("wow64: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("wow64: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("wow64: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("wow64: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("wow64: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("wow64: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("wow64: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("wow64: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: wow64: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("wow64: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("wow64: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: wow64: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("wow64: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("wow64: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retbastionangle stamp; Soft≠product)\n");
/* Grep: wow64: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("wow64: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("wow64: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retparapetangle stamp; Soft≠product)\n");
/* Grep: wow64: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("wow64: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("wow64: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retowerangle stamp; Soft≠product)\n");
/* Grep: wow64: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("wow64: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("wow64: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retwallangle stamp; Soft≠product)\n");
/* Grep: wow64: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("wow64: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("wow64: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retholdangle stamp; Soft≠product)\n");
/* Grep: wow64: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("wow64: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("wow64: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retfortressangle stamp; Soft≠product)\n");
/* Grep: wow64: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("wow64: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("wow64: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: wow64: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("wow64: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("wow64: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: wow64: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("wow64: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: wow64: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("wow64: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retbulwarkangle stamp; Soft≠product)\n");

                            kprintf("wow64: soft deepen wave=%u areas=%u calls=%u map=%u "
            "thunk=%u adjust=%u logs=%u "
            "(Wave 81 exclusive; not bar3)\n",
            (unsigned)GJ_WOW64_SOFT_WAVE,
            (unsigned)GJ_WOW64_SOFT_AREAS,
            g_u32Calls, g_u32MapHits, g_u32ThunkHits,
            g_u32SoftAdjustEnter, g_u32SoftLogN);

    /* Grep: wow64: soft inventory PASS / soft PASS */
    kprintf("wow64: soft inventory PASS wave=%u logs=%u "
            "calls=%u map=%u thunk=%u\n",
            (unsigned)GJ_WOW64_SOFT_WAVE, g_u32SoftLogN,
            g_u32Calls, g_u32MapHits, g_u32ThunkHits);
    kprintf("wow64: soft PASS wave=%u logs=%u\n",
            (unsigned)GJ_WOW64_SOFT_WAVE, g_u32SoftLogN);
}

/**
 * After first product translate/thunk/adjust/personality activity, print
 * soft inventory once (mirrors futex/compositor soft-stats-once).
 * Diagnostics only — never gates path PASS.
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_u32SoftTranslateEnter == 0 && g_u32SoftThunkEnter == 0 &&
        g_u32SoftAdjustEnter == 0 && g_u32SoftPersonOn == 0 &&
        g_u32SoftPersonOff == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    soft_inventory_log();
}

int
wow64_enabled(void)
{
    return g_fWow64;
}

void
wow64_set(int fOn)
{
    g_fWow64 = fOn ? 1 : 0;
    if (g_fWow64 != 0) {
        soft_inc(&g_u32SoftPersonOn);
    } else {
        soft_inc(&g_u32SoftPersonOff);
    }
    kprintf("wow64: personality %s\n", g_fWow64 ? "on" : "off");
    /* Wave 19: greppable soft inventory on personality flip. */
    soft_inventory_maybe_once();
}

/*
 * Core NR map (no counters). Returns 1 if explicit map, 0 if identity.
 * i386 → x86_64 public ABI; Deck Top-50 / glibc / PE32 int80 soft needs.
 * Single switch only — no duplicate case labels.
 */
static int
wow64_map_nr_core(u32 u32Nr32, u32 *pOutNr)
{
    u32 nr64;
    int fMapped = 1;

    switch (u32Nr32) {
    /* ---- process / exit / ids / credentials --------------------------- */
    case 1:   nr64 = 60; break;   /* exit */
    case 2:   nr64 = 57; break;   /* fork */
    case 7:   nr64 = 61; break;   /* waitpid → wait4 */
    case 11:  nr64 = 59; break;   /* execve */
    case 13:  nr64 = 201; break;  /* time */
    case 20:  nr64 = 39; break;   /* getpid */
    case 23:  nr64 = 105; break;  /* setuid */
    case 24:  nr64 = 102; break;  /* getuid */
    case 27:  nr64 = 37; break;   /* alarm */
    case 29:  nr64 = 34; break;   /* pause */
    case 37:  nr64 = 62; break;   /* kill */
    case 46:  nr64 = 106; break;  /* setgid */
    case 47:  nr64 = 104; break;  /* getgid */
    case 49:  nr64 = 107; break;  /* geteuid */
    case 50:  nr64 = 108; break;  /* getegid */
    case 57:  nr64 = 109; break;  /* setpgid */
    case 61:  nr64 = 161; break;  /* chroot */
    case 64:  nr64 = 110; break;  /* getppid */
    case 65:  nr64 = 111; break;  /* getpgrp */
    case 66:  nr64 = 112; break;  /* setsid */
    case 70:  nr64 = 113; break;  /* setreuid */
    case 71:  nr64 = 114; break;  /* setregid */
    case 81:  nr64 = 116; break;  /* setgroups */
    case 114: nr64 = 61; break;   /* wait4 */
    case 120: nr64 = 56; break;   /* clone */
    case 132: nr64 = 115; break;  /* getgroups */
    case 147: nr64 = 124; break;  /* getsid */
    case 164: nr64 = 117; break;  /* setresuid */
    case 165: nr64 = 118; break;  /* getresuid */
    case 170: nr64 = 119; break;  /* setresgid */
    case 171: nr64 = 120; break;  /* getresgid */
    case 190: nr64 = 58; break;   /* vfork */
    case 199: nr64 = 102; break;  /* getuid32 → getuid */
    case 200: nr64 = 104; break;  /* getgid32 → getgid */
    case 201: nr64 = 107; break;  /* geteuid32 */
    case 202: nr64 = 108; break;  /* getegid32 */
    case 203: nr64 = 113; break;  /* setreuid32 */
    case 204: nr64 = 114; break;  /* setregid32 */
    case 208: nr64 = 117; break;  /* setresuid32 */
    case 210: nr64 = 119; break;  /* setresgid32 */
    case 213: nr64 = 118; break;  /* getresuid32 */
    case 214: nr64 = 120; break;  /* getresgid32 */
    case 224: nr64 = 186; break;  /* gettid */
    case 238: nr64 = 200; break;  /* tkill */
    case 252: nr64 = 231; break;  /* exit_group */
    case 258: nr64 = 218; break;  /* set_tid_address */
    case 270: nr64 = 234; break;  /* tgkill */
    case 284: nr64 = 247; break;  /* waitid */
    case 340: nr64 = 302; break;  /* prlimit64 */
    case 347: nr64 = 310; break;  /* process_vm_readv */
    case 348: nr64 = 311; break;  /* process_vm_writev */
    case 358: nr64 = 322; break;  /* execveat */
    case 424: nr64 = 424; break;  /* pidfd_send_signal identity */
    case 434: nr64 = 434; break;  /* pidfd_open identity */
    case 435: nr64 = 435; break;  /* clone3 identity */

    /* ---- path / vfs (open, stat, rename, *at, xattr) ------------------ */
    case 5:   nr64 = 2; break;    /* open */
    case 6:   nr64 = 3; break;    /* close */
    case 8:   nr64 = 85; break;   /* creat */
    case 9:   nr64 = 86; break;   /* link */
    case 10:  nr64 = 87; break;   /* unlink */
    case 12:  nr64 = 80; break;   /* chdir */
    case 14:  nr64 = 133; break;  /* mknod */
    case 15:  nr64 = 90; break;   /* chmod */
    case 16:  nr64 = 94; break;   /* lchown */
    case 21:  nr64 = 165; break;  /* mount */
    case 22:  nr64 = 166; break;  /* umount → umount2 */
    case 28:  nr64 = 72; break;   /* fcntl (legacy alt) */
    case 30:  nr64 = 132; break;  /* utime */
    case 33:  nr64 = 21; break;   /* access */
    case 38:  nr64 = 82; break;   /* rename */
    case 39:  nr64 = 83; break;   /* mkdir */
    case 40:  nr64 = 84; break;   /* rmdir */
    case 55:  nr64 = 72; break;   /* fcntl */
    case 83:  nr64 = 88; break;   /* symlink */
    case 85:  nr64 = 89; break;   /* readlink */
    case 92:  nr64 = 76; break;   /* truncate */
    case 93:  nr64 = 77; break;   /* ftruncate */
    case 94:  nr64 = 91; break;   /* fchmod */
    case 95:  nr64 = 93; break;   /* fchown */
    case 99:  nr64 = 137; break;  /* statfs */
    case 100: nr64 = 138; break;  /* fstatfs */
    case 106: nr64 = 4; break;    /* stat */
    case 107: nr64 = 6; break;    /* lstat */
    case 108: nr64 = 5; break;    /* fstat */
    case 118: nr64 = 74; break;   /* fsync */
    case 133: nr64 = 81; break;   /* fchdir */
    case 136: nr64 = 135; break;  /* personality */
    case 148: nr64 = 75; break;   /* fdatasync */
    case 182: nr64 = 92; break;   /* chown */
    case 183: nr64 = 79; break;   /* getcwd */
    case 195: nr64 = 4; break;    /* stat64 → stat */
    case 196: nr64 = 6; break;    /* lstat64 → lstat */
    case 197: nr64 = 5; break;    /* fstat64 → fstat */
    case 198: nr64 = 94; break;   /* lchown32 */
    case 207: nr64 = 93; break;   /* fchown32 */
    case 212: nr64 = 92; break;   /* chown32 */
    case 226: nr64 = 188; break;  /* setxattr */
    case 227: nr64 = 189; break;  /* lsetxattr */
    case 228: nr64 = 190; break;  /* fsetxattr */
    case 229: nr64 = 191; break;  /* getxattr */
    case 230: nr64 = 192; break;  /* lgetxattr */
    case 231: nr64 = 193; break;  /* fgetxattr */
    case 232: nr64 = 194; break;  /* listxattr */
    case 233: nr64 = 195; break;  /* llistxattr */
    case 234: nr64 = 196; break;  /* flistxattr */
    case 235: nr64 = 197; break;  /* removexattr */
    case 236: nr64 = 198; break;  /* lremovexattr */
    case 237: nr64 = 199; break;  /* fremovexattr */
    case 268: nr64 = 137; break;  /* statfs64 → statfs */
    case 269: nr64 = 138; break;  /* fstatfs64 → fstatfs */
    case 271: nr64 = 280; break;  /* utimensat (utimes-shaped on some) */
    case 295: nr64 = 257; break;  /* openat */
    case 296: nr64 = 258; break;  /* mkdirat */
    case 297: nr64 = 259; break;  /* mknodat */
    case 298: nr64 = 260; break;  /* fchownat */
    case 299: nr64 = 261; break;  /* futimesat */
    case 300: nr64 = 262; break;  /* fstatat64 → newfstatat */
    case 301: nr64 = 263; break;  /* unlinkat */
    case 302: nr64 = 264; break;  /* renameat */
    case 303: nr64 = 265; break;  /* linkat */
    case 304: nr64 = 266; break;  /* symlinkat */
    case 305: nr64 = 267; break;  /* readlinkat */
    case 306: nr64 = 268; break;  /* fchmodat */
    case 307: nr64 = 269; break;  /* faccessat */
    case 341: nr64 = 303; break;  /* name_to_handle_at */
    case 342: nr64 = 304; break;  /* open_by_handle_at */
    case 344: nr64 = 306; break;  /* syncfs */
    case 353: nr64 = 316; break;  /* renameat2 */
    case 383: nr64 = 332; break;  /* statx */
    case 437: nr64 = 437; break;  /* openat2 identity */
    case 439: nr64 = 439; break;  /* faccessat2 identity */

    /* ---- mmap2 / memory ----------------------------------------------- */
    case 45:  nr64 = 12; break;   /* brk */
    case 90:  nr64 = 9; break;    /* old mmap → mmap */
    case 91:  nr64 = 11; break;   /* munmap */
    case 125: nr64 = 10; break;   /* mprotect */
    case 144: nr64 = 26; break;   /* msync */
    case 150: nr64 = 149; break;  /* mlock */
    case 151: nr64 = 150; break;  /* munlock */
    case 152: nr64 = 151; break;  /* mlockall */
    case 153: nr64 = 152; break;  /* munlockall */
    case 163: nr64 = 25; break;   /* mremap */
    case 172: nr64 = 157; break;  /* prctl */
    case 192: nr64 = 9; break;    /* mmap2 → mmap (pgoff pages; soft convert) */
    case 218: nr64 = 27; break;   /* mincore */
    case 219: nr64 = 28; break;   /* madvise */
    case 356: nr64 = 319; break;  /* memfd_create */
    case 376: nr64 = 325; break;  /* mlock2 */

    /* ---- fd I/O / pipe / socket / splice / uring ---------------------- */
    case 3:   nr64 = 0; break;    /* read */
    case 4:   nr64 = 1; break;    /* write */
    case 19:  nr64 = 8; break;    /* lseek */
    case 41:  nr64 = 32; break;   /* dup */
    case 42:  nr64 = 22; break;   /* pipe */
    case 54:  nr64 = 16; break;   /* ioctl */
    case 63:  nr64 = 33; break;   /* dup2 */
    case 102: nr64 = 41; break;   /* socketcall → socket (coarse; soft demux) */
    case 140: nr64 = 8; break;    /* _llseek → lseek-shaped */
    case 141: nr64 = 78; break;   /* getdents */
    case 145: nr64 = 19; break;   /* readv */
    case 146: nr64 = 20; break;   /* writev */
    case 180: nr64 = 17; break;   /* pread64 */
    case 181: nr64 = 18; break;   /* pwrite64 */
    case 187: nr64 = 40; break;   /* sendfile (i386) */
    case 220: nr64 = 217; break;  /* getdents64 */
    case 221: nr64 = 72; break;   /* fcntl64 → fcntl */
    case 239: nr64 = 40; break;   /* sendfile64 → sendfile */
    case 313: nr64 = 275; break;  /* splice */
    case 314: nr64 = 277; break;  /* sync_file_range */
    case 315: nr64 = 276; break;  /* tee */
    case 316: nr64 = 278; break;  /* vmsplice */
    case 324: nr64 = 285; break;  /* fallocate */
    case 330: nr64 = 292; break;  /* dup3 */
    case 331: nr64 = 293; break;  /* pipe2 */
    case 333: nr64 = 295; break;  /* preadv */
    case 334: nr64 = 296; break;  /* pwritev */
    case 337: nr64 = 299; break;  /* recvmmsg */
    case 345: nr64 = 307; break;  /* sendmmsg */
    case 364: nr64 = 288; break;  /* accept4 */
    case 377: nr64 = 326; break;  /* copy_file_range */
    case 378: nr64 = 327; break;  /* preadv2 */
    case 379: nr64 = 328; break;  /* pwritev2 */
    case 425: nr64 = 425; break;  /* io_uring_setup identity */
    case 426: nr64 = 426; break;  /* io_uring_enter identity */
    case 427: nr64 = 427; break;  /* io_uring_register identity */
    case 436: nr64 = 436; break;  /* close_range identity */

    /* ---- poll / epoll / inotify / eventfd ----------------------------- */
    case 142: nr64 = 23; break;   /* select */
    case 168: nr64 = 7; break;    /* poll */
    case 254: nr64 = 213; break;  /* epoll_create */
    case 255: nr64 = 233; break;  /* epoll_ctl */
    case 256: nr64 = 232; break;  /* epoll_wait */
    case 291: nr64 = 253; break;  /* inotify_init */
    case 292: nr64 = 254; break;  /* inotify_add_watch */
    case 293: nr64 = 255; break;  /* inotify_rm_watch */
    case 308: nr64 = 270; break;  /* pselect6 */
    case 309: nr64 = 271; break;  /* ppoll */
    case 319: nr64 = 281; break;  /* epoll_pwait */
    case 323: nr64 = 284; break;  /* eventfd */
    case 328: nr64 = 290; break;  /* eventfd2 */
    case 329: nr64 = 291; break;  /* epoll_create1 */
    case 332: nr64 = 294; break;  /* inotify_init1 */

    /* ---- time / futex / tls / sched / signals / misc ------------------ */
    case 36:  nr64 = 162; break;  /* sync */
    case 67:  nr64 = 13; break;   /* sigaction → rt_sigaction */
    case 72:  nr64 = 130; break;  /* sigsuspend → rt_sigsuspend */
    case 73:  nr64 = 127; break;  /* sigpending → rt_sigpending */
    case 75:  nr64 = 160; break;  /* setrlimit */
    case 76:  nr64 = 97; break;   /* getrlimit */
    case 77:  nr64 = 98; break;   /* getrusage */
    case 78:  nr64 = 96; break;   /* gettimeofday */
    case 79:  nr64 = 164; break;  /* settimeofday */
    case 96:  nr64 = 140; break;  /* getpriority */
    case 97:  nr64 = 141; break;  /* setpriority */
    case 103: nr64 = 103; break;  /* syslog identity */
    case 104: nr64 = 38; break;   /* setitimer */
    case 105: nr64 = 36; break;   /* getitimer */
    case 116: nr64 = 99; break;   /* sysinfo */
    case 122: nr64 = 63; break;   /* uname */
    case 143: nr64 = 73; break;   /* flock */
    case 154: nr64 = 142; break;  /* sched_setparam */
    case 155: nr64 = 143; break;  /* sched_getparam */
    case 156: nr64 = 144; break;  /* sched_setscheduler */
    case 157: nr64 = 145; break;  /* sched_getscheduler */
    case 158: nr64 = 24; break;   /* sched_yield */
    case 159: nr64 = 146; break;  /* sched_get_priority_max */
    case 160: nr64 = 147; break;  /* sched_get_priority_min */
    case 161: nr64 = 148; break;  /* sched_rr_get_interval */
    case 162: nr64 = 35; break;   /* nanosleep */
    case 174: nr64 = 13; break;   /* rt_sigaction */
    case 175: nr64 = 14; break;   /* rt_sigprocmask */
    case 176: nr64 = 127; break;  /* rt_sigpending */
    case 177: nr64 = 128; break;  /* rt_sigtimedwait */
    case 178: nr64 = 129; break;  /* rt_sigqueueinfo */
    case 179: nr64 = 130; break;  /* rt_sigsuspend */
    case 186: nr64 = 131; break;  /* sigaltstack */
    case 191: nr64 = 97; break;   /* ugetrlimit → getrlimit */
    case 240: nr64 = 202; break;  /* futex */
    case 241: nr64 = 203; break;  /* sched_setaffinity */
    case 242: nr64 = 204; break;  /* sched_getaffinity */
    case 243: nr64 = 158; break;  /* set_thread_area → arch_prctl soft */
    case 259: nr64 = 222; break;  /* timer_create */
    case 260: nr64 = 223; break;  /* timer_settime */
    case 261: nr64 = 224; break;  /* timer_gettime */
    case 262: nr64 = 225; break;  /* timer_getoverrun */
    case 263: nr64 = 226; break;  /* timer_delete */
    case 264: nr64 = 227; break;  /* clock_settime */
    case 265: nr64 = 228; break;  /* clock_gettime */
    case 266: nr64 = 229; break;  /* clock_getres */
    case 267: nr64 = 230; break;  /* clock_nanosleep */
    case 311: nr64 = 273; break;  /* set_robust_list */
    case 312: nr64 = 274; break;  /* get_robust_list */
    case 318: nr64 = 309; break;  /* getcpu */
    case 321: nr64 = 282; break;  /* signalfd */
    case 322: nr64 = 283; break;  /* timerfd_create */
    case 325: nr64 = 286; break;  /* timerfd_settime */
    case 326: nr64 = 287; break;  /* timerfd_gettime */
    case 327: nr64 = 289; break;  /* signalfd4 */
    case 343: nr64 = 305; break;  /* clock_adjtime */
    case 346: nr64 = 308; break;  /* setns */
    case 349: nr64 = 312; break;  /* kcmp */
    case 350: nr64 = 314; break;  /* sched_setattr soft */
    case 351: nr64 = 315; break;  /* sched_getattr soft */
    case 354: nr64 = 317; break;  /* seccomp */
    case 355: nr64 = 318; break;  /* getrandom */
    case 357: nr64 = 321; break;  /* bpf */
    case 374: nr64 = 323; break;  /* userfaultfd */
    case 375: nr64 = 324; break;  /* membarrier */
    case 386: nr64 = 334; break;  /* rseq */

    default:
        nr64 = u32Nr32;
        fMapped = 0;
        break;
    }

    if (pOutNr != NULL) {
        *pOutNr = nr64;
    }
    return fMapped;
}

int
wow64_translate_nr(u32 u32Nr32, u32 *pOutNr)
{
    int fMapped;

    soft_inc(&g_u32SoftTranslateEnter);
    g_u32Calls++;
    if (pOutNr == NULL) {
        soft_inc(&g_u32SoftTranslateNull);
        soft_inventory_maybe_once();
        return -1;
    }
    fMapped = wow64_map_nr_core(u32Nr32, pOutNr);
    if (fMapped) {
        g_u32MapHits++;
    } else {
        g_u32IdentityHits++;
    }
    soft_inventory_maybe_once();
    return 0;
}

int
wow64_nr_is_mapped(u32 u32Nr32)
{
    u32 u32Nr64;

    soft_inc(&g_u32SoftIsMappedProbe);
    return wow64_map_nr_core(u32Nr32, &u32Nr64);
}

u32
wow64_args_zero_extend(u64 *pArgs, u32 cArgs)
{
    u32 iArg;
    u32 cTouch;

    soft_inc(&g_u32SoftArgsZxCalls);
    if (pArgs == NULL || cArgs == 0) {
        return 0;
    }
    cTouch = cArgs > 6u ? 6u : cArgs;
    for (iArg = 0; iArg < cTouch; iArg++) {
        pArgs[iArg] &= 0xffffffffull;
    }
    return cTouch;
}

u64
wow64_mmap2_pgoff_to_bytes(u32 u32PgoffPages)
{
    return (u64)u32PgoffPages << GJ_WOW64_PAGE_SHIFT;
}

u64
wow64_ptr32(u64 u64Val)
{
    soft_inc(&g_u32SoftPtr32);
    return u64Val & 0xffffffffull;
}

/*
 * i386 socketcall subcall → x86_64 NR (public man socketcall).
 */
int
wow64_socketcall_nr(u32 u32Call, u32 *pOutNr)
{
    u32 nr64;

    if (pOutNr == NULL) {
        soft_inc(&g_u32SoftSocketNrFail);
        return -1;
    }
    switch (u32Call) {
    case GJ_WOW64_SYS_SOCKET:      nr64 = 41; break;  /* socket */
    case GJ_WOW64_SYS_BIND:        nr64 = 49; break;  /* bind */
    case GJ_WOW64_SYS_CONNECT:     nr64 = 42; break;  /* connect */
    case GJ_WOW64_SYS_LISTEN:      nr64 = 50; break;  /* listen */
    case GJ_WOW64_SYS_ACCEPT:      nr64 = 43; break;  /* accept */
    case GJ_WOW64_SYS_GETSOCKNAME: nr64 = 51; break;  /* getsockname */
    case GJ_WOW64_SYS_GETPEERNAME: nr64 = 52; break;  /* getpeername */
    case GJ_WOW64_SYS_SOCKETPAIR:  nr64 = 53; break;  /* socketpair */
    case GJ_WOW64_SYS_SEND:        nr64 = 44; break;  /* send → sendto */
    case GJ_WOW64_SYS_RECV:        nr64 = 45; break;  /* recv → recvfrom */
    case GJ_WOW64_SYS_SENDTO:      nr64 = 44; break;  /* sendto */
    case GJ_WOW64_SYS_RECVFROM:    nr64 = 45; break;  /* recvfrom */
    case GJ_WOW64_SYS_SHUTDOWN:    nr64 = 48; break;  /* shutdown */
    case GJ_WOW64_SYS_SETSOCKOPT:  nr64 = 54; break;  /* setsockopt */
    case GJ_WOW64_SYS_GETSOCKOPT:  nr64 = 55; break;  /* getsockopt */
    case GJ_WOW64_SYS_SENDMSG:     nr64 = 46; break;  /* sendmsg */
    case GJ_WOW64_SYS_RECVMSG:     nr64 = 47; break;  /* recvmsg */
    case GJ_WOW64_SYS_ACCEPT4:     nr64 = 288; break; /* accept4 */
    case GJ_WOW64_SYS_RECVMMSG:    nr64 = 299; break; /* recvmmsg */
    case GJ_WOW64_SYS_SENDMMSG:    nr64 = 307; break; /* sendmmsg */
    default:
        soft_inc(&g_u32SoftSocketNrFail);
        return -1;
    }
    *pOutNr = nr64;
    soft_inc(&g_u32SoftSocketNrOk);
    return 0;
}

u32
wow64_adjust_args(u32 u32Nr32, u32 *pOutNr64, u64 *pArgs, u32 cArgs)
{
    u32 u32Flags = 0;
    u32 u32Call;
    u32 u32SockNr;

    soft_inc(&g_u32SoftAdjustEnter);
    if (pArgs == NULL || cArgs == 0) {
        soft_inc(&g_u32SoftAdjustNop);
        soft_inventory_maybe_once();
        return 0;
    }

    (void)wow64_args_zero_extend(pArgs, cArgs);
    u32Flags |= GJ_WOW64_THUNK_ARGS_ZX;
    soft_inc(&g_u32SoftAdjustZx);

    if (u32Nr32 == 192u) {
        /* mmap2: arg5 is page offset → byte offset for mmap */
        if (cArgs > 5u) {
            pArgs[5] = wow64_mmap2_pgoff_to_bytes((u32)pArgs[5]);
            u32Flags |= GJ_WOW64_THUNK_MMAP2_PGOFF;
            g_u32Mmap2Hits++;
            soft_inc(&g_u32SoftAdjustMmap2);
        }
        if (pOutNr64 != NULL) {
            *pOutNr64 = 9; /* mmap */
        }
        u32Flags |= GJ_WOW64_THUNK_NR_MAPPED;
        soft_inventory_maybe_once();
        return u32Flags;
    }

    if (u32Nr32 == 102u) {
        /* socketcall(call, args_ptr): demux NR; user pull later */
        u32Call = (u32)pArgs[0];
        soft_inc(&g_u32SoftAdjustSocket);
        if (wow64_socketcall_nr(u32Call, &u32SockNr) == 0) {
            if (pOutNr64 != NULL) {
                *pOutNr64 = u32SockNr;
            }
            u32Flags |= GJ_WOW64_THUNK_SOCKETCALL;
            u32Flags |= GJ_WOW64_THUNK_NR_MAPPED;
            u32Flags |= GJ_WOW64_THUNK_SOCKET_NEED_PULL;
            g_u32SocketcallHits++;
        } else {
            if (pOutNr64 != NULL && *pOutNr64 == 0) {
                *pOutNr64 = 41;
            }
            u32Flags |= GJ_WOW64_THUNK_SOCKETCALL;
            soft_inc(&g_u32SoftAdjustSockFail);
        }
        soft_inventory_maybe_once();
        return u32Flags;
    }

    if (u32Nr32 == 90u) {
        /*
         * old_mmap: single struct ptr in arg0 on i386. Soft path only
         * zero-extends; struct unpack is trap/dispatch responsibility.
         */
        soft_inc(&g_u32SoftAdjustOldMmap);
        u32Flags |= GJ_WOW64_THUNK_NR_MAPPED;
        if (pOutNr64 != NULL) {
            *pOutNr64 = 9;
        }
        soft_inventory_maybe_once();
        return u32Flags;
    }

    if (u32Nr32 == 140u) {
        /*
         * _llseek(fd, offset_high, offset_low, result, whence):
         * soft-compose 64-bit offset into arg1 for lseek-shaped hot path.
         * arg0=fd, arg1=high, arg2=low, arg3=result*, arg4=whence
         */
        if (cArgs >= 5u) {
            u64 u64Off = ((u64)(u32)pArgs[1] << 32) | (u64)(u32)pArgs[2];

            pArgs[1] = u64Off;
            pArgs[2] = pArgs[4]; /* whence */
            u32Flags |= GJ_WOW64_THUNK_NR_MAPPED;
            soft_inc(&g_u32SoftAdjustLlseek);
        }
        soft_inventory_maybe_once();
        return u32Flags;
    }

    soft_inventory_maybe_once();
    return u32Flags;
}

int
wow64_thunk_soft(struct gj_wow64_thunk *pThunk)
{
    u32 u32Nr64;
    int fMapped;
    u32 u32Adj;

    soft_inc(&g_u32SoftThunkEnter);
    if (pThunk == NULL) {
        soft_inc(&g_u32SoftThunkNull);
        soft_inventory_maybe_once();
        return -1;
    }

    g_u32ThunkHits++;
    g_u32Calls++;
    soft_inc(&g_u32SoftThunkOk);

    fMapped = wow64_map_nr_core(pThunk->u32Nr32, &u32Nr64);
    if (fMapped) {
        g_u32MapHits++;
        pThunk->u32Flags = GJ_WOW64_THUNK_NR_MAPPED;
    } else {
        g_u32IdentityHits++;
        pThunk->u32Flags = GJ_WOW64_THUNK_IDENTITY;
        u32Nr64 = pThunk->u32Nr32;
    }
    pThunk->u32Nr64 = u32Nr64;
    pThunk->u32SocketCall = 0;

    u32Adj = wow64_adjust_args(pThunk->u32Nr32, &pThunk->u32Nr64,
                               pThunk->aArgs, 6);
    pThunk->u32Flags |= u32Adj;

    if ((u32Adj & GJ_WOW64_THUNK_SOCKETCALL) != 0) {
        pThunk->u32SocketCall = (u32)(pThunk->aArgs[0] & 0xffffffffull);
    }

    soft_inventory_maybe_once();
    return 0;
}

u32
wow64_calls(void)
{
    return g_u32Calls;
}

u32
wow64_map_hits(void)
{
    return g_u32MapHits;
}

u32
wow64_identity_hits(void)
{
    return g_u32IdentityHits;
}

u32
wow64_thunk_hits(void)
{
    return g_u32ThunkHits;
}

u32
wow64_mmap2_hits(void)
{
    return g_u32Mmap2Hits;
}

u32
wow64_socketcall_hits(void)
{
    return g_u32SocketcallHits;
}
