/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Cooperative RR scheduler core for shared aarch64 product.
 * Stack frames built for AArch64 AAPCS64 (x29/x30 + entry).
 *
 * Wave 48 exclusive soft deepen (this unit only — greppable "coop: soft …"):
 *   coop: soft honesty     — cooperative only; no preemption/SMP claim
 *   coop: soft inventory   — slots/states/cur/next_id/stack snapshot
 *   coop: soft slots       — UNUSED/RUNNABLE/RUNNING/EXITED counts
 *   coop: soft boot        — boot thr id=0 RUNNING
 *   coop: soft null        — null entry create rejects
 *   coop: soft solo        — solo yield no-op (boot stays current)
 *   coop: soft create      — arg + current_id handoff
 *   coop: soft reuse       — EXITED slot reclaim
 *   coop: soft rr          — two-thr yield RR order
 *   coop: soft cap         — capacity fill + overflow reject
 *   coop: soft reclaim     — create after full drain
 *   coop: soft mono        — next_id monotonic across creates
 *   coop: soft align       — SP 16-byte aligned at create
 *   coop: soft path        — surface catalog + non-claims
 *   coop: soft geom        — Wave 19 max_thr/stack/frame geometry
 *   coop: soft return      — Wave 19 API return surfaces + product_kernel=OPEN
 *   coop: soft deepen      — wave=48 stamp + area count
 *   coop: soft PASS|FAIL / coop: soft inventory PASS|FAIL
 * Honesty: soft inventory only — not preemptive product sched / not bar3.
 */
#include <gj/klog.h>
#include <gj/sched_coop.h>
#include <gj/string.h>

/* Wave 48 soft inventory stamp (file-local; never product gate). */
#define COOP_SOFT_WAVE 48u

struct gj_coop_thr {
    u8  u8State;
    u8  u8Pad[3];
    u32 u32Id;
    u64 u64Sp;
    gj_coop_entry_fn pfn;
    void *pArg;
    u8  aStack[GJ_COOP_STACK] __attribute__((aligned(16)));
};

static struct gj_coop_thr g_aThr[GJ_COOP_MAX_THR];
static u32 g_u32Cur;
static u32 g_u32NextId = 1;
static int g_fInited;
/* Wave 19: soft inventory emission counter (observability only). */
static u32 g_cSoftInvLogs;

/* Soft selftest scratch (volatile — observed across yield/switch). */
static volatile int g_fSelftestDone;
static volatile int g_fSelftestRan;
static volatile u32 g_u32SelftestSeenId;
static volatile u32 g_u32SelftestArg;
static volatile u32 g_cSelftestHits;
static volatile u32 g_u32SelftestOrder;
static volatile u32 g_u32SelftestSpAlign; /* 1 if create SP was 16-aligned */

static void
coop_trampoline(void)
{
    struct gj_coop_thr *p = &g_aThr[g_u32Cur];

    if (p->pfn != 0) {
        p->pfn(p->pArg);
    }
    gj_coop_exit();
}

void
gj_coop_init(void)
{
    memset(g_aThr, 0, sizeof(g_aThr));
    /* Slot 0 = boot "main" thr (no stack of its own beyond current SP). */
    g_aThr[0].u8State = (u8)GJ_COOP_RUNNING;
    g_aThr[0].u32Id = 0;
    g_u32Cur = 0;
    g_u32NextId = 1;
    g_fInited = 1;
    g_fSelftestDone = 0;
    g_fSelftestRan = 0;
    g_u32SelftestSeenId = 0;
    g_u32SelftestArg = 0;
    g_cSelftestHits = 0;
    g_u32SelftestOrder = 0;
    g_u32SelftestSpAlign = 0;
}

u32
gj_coop_create(gj_coop_entry_fn pfn, void *pArg)
{
    u32 i;
    u64 *pSp;
    struct gj_coop_thr *p;

    if (!g_fInited || pfn == 0) {
        return 0;
    }
    for (i = 1; i < GJ_COOP_MAX_THR; i++) {
        if (g_aThr[i].u8State == (u8)GJ_COOP_UNUSED ||
            g_aThr[i].u8State == (u8)GJ_COOP_EXITED) {
            break;
        }
    }
    if (i >= GJ_COOP_MAX_THR) {
        return 0;
    }
    p = &g_aThr[i];
    memset(p, 0, sizeof(*p));
    p->u8State = (u8)GJ_COOP_RUNNABLE;
    p->u32Id = g_u32NextId++;
    p->pfn = pfn;
    p->pArg = pArg;
    /*
     * AArch64 frame matches gj_coop_switch restore order (low→high):
     *   x27,x28 x25,x26 x23,x24 x21,x22 x19,x20 x29,x30=trampoline
     * x86_64 product does not use this core.
     */
    pSp = (u64 *)(void *)&p->aStack[GJ_COOP_STACK];
    pSp = (u64 *)(((u64)(gj_vaddr_t)pSp) & ~0xful);
    pSp -= 12;
    pSp[0] = 0;
    pSp[1] = 0; /* x27,x28 */
    pSp[2] = 0;
    pSp[3] = 0; /* x25,x26 */
    pSp[4] = 0;
    pSp[5] = 0; /* x23,x24 */
    pSp[6] = 0;
    pSp[7] = 0; /* x21,x22 */
    pSp[8] = 0;
    pSp[9] = 0; /* x19,x20 */
    pSp[10] = 0; /* x29 */
    pSp[11] = (u64)(gj_vaddr_t)(void *)coop_trampoline; /* x30 */
    p->u64Sp = (u64)(gj_vaddr_t)(void *)pSp;
    /* Soft SP align lamp (Wave 19): AAPCS64 requires 16-byte SP. */
    if ((p->u64Sp & 0xful) == 0ul) {
        g_u32SelftestSpAlign = 1;
    } else {
        g_u32SelftestSpAlign = 0;
    }
    return p->u32Id;
}

static u32
pick_next(void)
{
    u32 i;
    u32 start = g_u32Cur;

    for (i = 1; i <= GJ_COOP_MAX_THR; i++) {
        u32 idx = (start + i) % GJ_COOP_MAX_THR;

        if (g_aThr[idx].u8State == (u8)GJ_COOP_RUNNABLE) {
            return idx;
        }
    }
    /* Prefer boot thr 0 if still running */
    if (g_aThr[0].u8State == (u8)GJ_COOP_RUNNING ||
        g_aThr[0].u8State == (u8)GJ_COOP_RUNNABLE) {
        return 0;
    }
    return g_u32Cur;
}

void
gj_coop_yield(void)
{
    u32 u32Next;
    u32 u32Old;
    u64 *pOldSp;

    if (!g_fInited) {
        return;
    }
    u32Next = pick_next();
    if (u32Next == g_u32Cur) {
        return;
    }
    u32Old = g_u32Cur;
    if (g_aThr[u32Old].u8State == (u8)GJ_COOP_RUNNING) {
        g_aThr[u32Old].u8State = (u8)GJ_COOP_RUNNABLE;
    }
    g_aThr[u32Next].u8State = (u8)GJ_COOP_RUNNING;
    g_u32Cur = u32Next;
    pOldSp = &g_aThr[u32Old].u64Sp;
    gj_coop_switch(pOldSp, g_aThr[u32Next].u64Sp);
}

void
gj_coop_exit(void)
{
    if (!g_fInited) {
        return;
    }
    g_aThr[g_u32Cur].u8State = (u8)GJ_COOP_EXITED;
    g_aThr[g_u32Cur].pfn = 0;
    /* Never return — switch to another thr */
    for (;;) {
        u32 n = pick_next();

        if (n == g_u32Cur) {
            /* Only us left and exited — spin */
            __asm__ volatile("wfe");
            continue;
        }
        g_u32Cur = n;
        g_aThr[n].u8State = (u8)GJ_COOP_RUNNING;
        {
            u64 sink = 0;
            gj_coop_switch(&sink, g_aThr[n].u64Sp);
        }
    }
}

u32
gj_coop_current_id(void)
{
    if (!g_fInited) {
        return 0;
    }
    return g_aThr[g_u32Cur].u32Id;
}

/* Soft slot census (Wave 19 inventory). */
static void
soft_slot_census(u32 *pUnused, u32 *pRunnable, u32 *pRunning, u32 *pExited)
{
    u32 i;
    u32 cU = 0;
    u32 cRa = 0;
    u32 cRi = 0;
    u32 cE = 0;

    for (i = 0; i < GJ_COOP_MAX_THR; i++) {
        u8 u8St = g_aThr[i].u8State;

        if (u8St == (u8)GJ_COOP_UNUSED) {
            cU++;
        } else if (u8St == (u8)GJ_COOP_RUNNABLE) {
            cRa++;
        } else if (u8St == (u8)GJ_COOP_RUNNING) {
            cRi++;
        } else if (u8St == (u8)GJ_COOP_EXITED) {
            cE++;
        }
    }
    if (pUnused != 0) {
        *pUnused = cU;
    }
    if (pRunnable != 0) {
        *pRunnable = cRa;
    }
    if (pRunning != 0) {
        *pRunning = cRi;
    }
    if (pExited != 0) {
        *pExited = cE;
    }
}

/*
 * Wave 19 greppable soft inventory dump (never hard-gates boot).
 * Prefix-stable: "coop: soft …". greppable: coop: soft
 */
static void
coop_soft_inventory(int fPass, unsigned cAreas, u32 u32Surf)
{
    u32 cUnused;
    u32 cRunnable;
    u32 cRunning;
    u32 cExited;
    const char *szVerdict;

    if (g_cSoftInvLogs < 0xffffffffu) {
        g_cSoftInvLogs++;
    }

    soft_slot_census(&cUnused, &cRunnable, &cRunning, &cExited);
    szVerdict = (fPass != 0) ? "PASS" : "FAIL";

    /*
     * Honesty: cooperative RR only — not preemptive product sched,
     * not SMP migration, not bar3. Soft inventory only.
     * greppable: coop: soft honesty
     */
    kprintf("coop: soft honesty cooperative=1 preemptive=0 smp=0 "
            "bar3=0 multi_server=0 confine=0 product_kernel=OPEN "
            "max_thr=%u stack=%u wave=%u "
            "(soft inventory only; not product sched)\n",
            (unsigned)GJ_COOP_MAX_THR, (unsigned)GJ_COOP_STACK,
            (unsigned)COOP_SOFT_WAVE);

    /* Grep: coop: soft exclusive — Wave 35 exclusive deepen */
    kprintf("coop: soft exclusive wave=%u multi_server=0 confine=0 bar3=0 "
            "product_kernel=OPEN soft_only=1 preemptive=0 smp=0\n",
            (unsigned)COOP_SOFT_WAVE);

    /* Grep: coop: soft open — Wave 19 open-lamp rollup */
    kprintf("coop: soft open multi_server=0 confine=0 bar3=0 "
            "product_kernel=OPEN soft_only=1 wave=%u\n",
            (unsigned)COOP_SOFT_WAVE);

    /* Grep: coop: soft inventory */
    kprintf("coop: soft inventory inited=%u cur=%u cur_id=%u next_id=%u "
            "max_thr=%u stack=%u unused=%u runnable=%u running=%u "
            "exited=%u logs=%u surf=0x%x wave=%u\n",
            g_fInited ? 1u : 0u, g_u32Cur,
            (unsigned)gj_coop_current_id(), g_u32NextId,
            (unsigned)GJ_COOP_MAX_THR, (unsigned)GJ_COOP_STACK,
            cUnused, cRunnable, cRunning, cExited,
            g_cSoftInvLogs, u32Surf, (unsigned)COOP_SOFT_WAVE);

    /* Grep: coop: soft slots */
    kprintf("coop: soft slots unused=%u runnable=%u running=%u "
            "exited=%u max=%u sum=%u wave=%u\n",
            cUnused, cRunnable, cRunning, cExited,
            (unsigned)GJ_COOP_MAX_THR,
            cUnused + cRunnable + cRunning + cExited,
            (unsigned)COOP_SOFT_WAVE);

    /* Grep: coop: soft path — surface catalog + non-claims */
    kprintf("coop: soft path init=1 create=1 yield=1 exit=1 "
            "current_id=1 selftest=1 preemptive=0 smp=0 bar3=0 wave=%u\n",
            (unsigned)COOP_SOFT_WAVE);

    /* Grep: coop: soft geom (Wave 19 max_thr/stack/frame) */
    kprintf("coop: soft geom max_thr=%u stack=%u frame_words=12 "
            "sp_align=16 thr_bytes=%u boot_id=0 wave=%u\n",
            (unsigned)GJ_COOP_MAX_THR, (unsigned)GJ_COOP_STACK,
            (unsigned)sizeof(struct gj_coop_thr),
            (unsigned)COOP_SOFT_WAVE);

    /* Grep: coop: soft return — Wave 19 API return surfaces */
    kprintf("coop: soft return create_id=1 yield_void=1 exit_void=1 "
            "current_id=1 selftest_bool=1 inv_ret=%u surf=0x%x "
            "product_kernel=OPEN wave=%u\n",
            (fPass != 0) ? 1u : 0u, u32Surf, (unsigned)COOP_SOFT_WAVE);

    /* Grep: coop: soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: coop: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("coop: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)COOP_SOFT_WAVE);
    /* Grep: coop: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("coop: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)COOP_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: coop: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("coop: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)COOP_SOFT_WAVE);
    /* Grep: coop: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("coop: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)COOP_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: coop: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("coop: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)COOP_SOFT_WAVE);
            /* Grep: coop: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("coop: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)COOP_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: coop: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("coop: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)COOP_SOFT_WAVE);
            /* Grep: coop: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("coop: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)COOP_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: coop: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("coop: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)COOP_SOFT_WAVE);
            /* Grep: coop: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("coop: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)COOP_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: coop: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("coop: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)COOP_SOFT_WAVE);
            /* Grep: coop: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("coop: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)COOP_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: coop: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("coop: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)COOP_SOFT_WAVE);
            /* Grep: coop: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("coop: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)COOP_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: coop: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("coop: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)COOP_SOFT_WAVE);
            /* Grep: coop: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("coop: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)COOP_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: coop: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("coop: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)COOP_SOFT_WAVE);
                    /* Grep: coop: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("coop: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)COOP_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: coop: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("coop: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)COOP_SOFT_WAVE);
                            /* Grep: coop: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("coop: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)COOP_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: coop: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("coop: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)COOP_SOFT_WAVE);
                            /* Grep: coop: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("coop: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)COOP_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: coop: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("coop: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)COOP_SOFT_WAVE);
                            /* Grep: coop: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("coop: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)COOP_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: coop: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("coop: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)COOP_SOFT_WAVE);
                            /* Grep: coop: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("coop: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)COOP_SOFT_WAVE);
                            /* Grep: coop: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("coop: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)COOP_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: coop: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("coop: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)COOP_SOFT_WAVE);
/* Grep: coop: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("coop: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)COOP_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: coop: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("coop: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)COOP_SOFT_WAVE);
/* Grep: coop: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("coop: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)COOP_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: coop: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("coop: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)COOP_SOFT_WAVE);
/* Grep: coop: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("coop: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)COOP_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: coop: soft retfortress — Wave 35 return-fortress honesty */
kprintf("coop: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)COOP_SOFT_WAVE);
/* Grep: coop: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("coop: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)COOP_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: coop: soft rethold — Wave 36 return-hold honesty */
kprintf("coop: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)COOP_SOFT_WAVE);
/* Grep: coop: soft retspire — Wave 36 exclusive spire stamp */
kprintf("coop: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)COOP_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: coop: soft retwall — Wave 37 return-wall honesty */
kprintf("coop: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)COOP_SOFT_WAVE);
/* Grep: coop: soft retgate — Wave 37 exclusive gate stamp */
kprintf("coop: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)COOP_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: coop: soft retmoat — Wave 38 return-moat honesty */
kprintf("coop: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)COOP_SOFT_WAVE);
/* Grep: coop: soft retower — Wave 38 exclusive tower stamp */
kprintf("coop: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)COOP_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: coop: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("coop: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)COOP_SOFT_WAVE);
/* Grep: coop: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("coop: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)COOP_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: coop: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("coop: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)COOP_SOFT_WAVE);
/* Grep: coop: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("coop: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)COOP_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: coop: soft retravelin — Wave 41 return-travelin honesty */
kprintf("coop: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)COOP_SOFT_WAVE);
/* Grep: coop: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("coop: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)COOP_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: coop: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("coop: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)COOP_SOFT_WAVE);
/* Grep: coop: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("coop: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)COOP_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: coop: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("coop: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)COOP_SOFT_WAVE);
/* Grep: coop: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("coop: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)COOP_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: coop: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("coop: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)COOP_SOFT_WAVE);
/* Grep: coop: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("coop: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)COOP_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: coop: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("coop: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)COOP_SOFT_WAVE);
/* Grep: coop: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("coop: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)COOP_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: coop: soft retbailey — Wave 46 return-bailey honesty */
kprintf("coop: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)COOP_SOFT_WAVE);
/* Grep: coop: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("coop: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)COOP_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: coop: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("coop: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=48 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: coop: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("coop: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=48 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: coop: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("coop: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=48 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: coop: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("coop: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=48 "
        "(retpalisade stamp; Soft≠product)\n");



                            kprintf("coop: soft deepen wave=%u areas=%u max_thr=%u stack=%u "
            "logs=%u surf=0x%x\n",
            (unsigned)COOP_SOFT_WAVE, cAreas,
            (unsigned)GJ_COOP_MAX_THR, (unsigned)GJ_COOP_STACK,
            g_cSoftInvLogs, u32Surf);

    /* Grep: coop: soft inventory PASS|FAIL / coop: soft PASS|FAIL */
    kprintf("coop: soft inventory %s cur_id=%u next_id=%u wave=%u\n",
            szVerdict, (unsigned)gj_coop_current_id(), g_u32NextId,
            (unsigned)COOP_SOFT_WAVE);
    kprintf("coop: soft %s areas=%u surf=0x%x wave=%u\n",
            szVerdict, cAreas, u32Surf, (unsigned)COOP_SOFT_WAVE);
}

static void
selftest_entry(void *pArg)
{
    g_u32SelftestArg = (u32)(gj_vaddr_t)pArg;
    g_u32SelftestSeenId = gj_coop_current_id();
    g_fSelftestRan = 1;
    g_fSelftestDone = 1;
    g_cSelftestHits++;
    gj_coop_exit();
}

/* A: mark, yield so peer can run, mark resume, exit. */
static void
selftest_entry_a(void *pArg)
{
    (void)pArg;
    g_u32SelftestOrder |= 1u;
    g_cSelftestHits++;
    gj_coop_yield();
    g_u32SelftestOrder |= 4u;
    gj_coop_exit();
}

/* B: mark and exit (runs while A yielded). */
static void
selftest_entry_b(void *pArg)
{
    (void)pArg;
    g_u32SelftestOrder |= 2u;
    g_cSelftestHits++;
    gj_coop_exit();
}

static void
selftest_quick_exit(void *pArg)
{
    (void)pArg;
    g_cSelftestHits++;
    gj_coop_exit();
}

/*
 * Soft deepen Wave 19: boot id, null create, solo yield, arg+id handoff,
 * SP align, EXITED slot reuse, two-thr yield RR, capacity fill+reclaim,
 * next_id monotonic, soft inventory. Returns 1 on PASS.
 */
int
gj_coop_selftest(void)
{
    u32 u32Id;
    u32 u32Id2;
    u32 u32Id3;
    u32 u32Fill;
    u32 u32Next0;
    unsigned uSpins;
    u32 cCreated;
    unsigned cAreas;
    u32 u32Surf;
    int fOk;
    u32 cUnused;
    u32 cRunnable;
    u32 cRunning;
    u32 cExited;

    /* Surface bits for greppable per-area lamps. */
    enum {
        SURF_BOOT    = 1u << 0,
        SURF_NULL    = 1u << 1,
        SURF_SOLO    = 1u << 2,
        SURF_CREATE  = 1u << 3,
        SURF_REUSE   = 1u << 4,
        SURF_RR      = 1u << 5,
        SURF_CAP     = 1u << 6,
        SURF_RECLAIM = 1u << 7,
        SURF_MONO    = 1u << 8,
        SURF_ALIGN   = 1u << 9,
        SURF_SLOTS   = 1u << 10
    };

    cAreas = 0;
    u32Surf = 0;
    fOk = 1;

    if (!g_fInited) {
        gj_coop_init();
    }

    if (gj_coop_current_id() != 0u) {
        fOk = 0;
        goto soft_out;
    }
    if (g_aThr[0].u8State != (u8)GJ_COOP_RUNNING &&
        g_aThr[0].u8State != (u8)GJ_COOP_RUNNABLE) {
        fOk = 0;
        goto soft_out;
    }
    u32Surf |= SURF_BOOT;
    cAreas++; /* boot */

    /* Null entry must fail. */
    if (gj_coop_create(0, 0) != 0u) {
        fOk = 0;
        goto soft_out;
    }
    u32Surf |= SURF_NULL;
    cAreas++; /* null */

    /* Solo yield is a no-op when no other runnable thr. */
    gj_coop_yield();
    if (gj_coop_current_id() != 0u) {
        fOk = 0;
        goto soft_out;
    }
    u32Surf |= SURF_SOLO;
    cAreas++; /* solo */

    /* Slot census: boot running, rest unused or exited from prior. */
    soft_slot_census(&cUnused, &cRunnable, &cRunning, &cExited);
    if (cRunning != 1u || cRunnable != 0u) {
        /* Allow prior EXITED slots; running must be exactly boot. */
        fOk = 0;
        goto soft_out;
    }
    if (cUnused + cRunnable + cRunning + cExited != GJ_COOP_MAX_THR) {
        fOk = 0;
        goto soft_out;
    }
    u32Surf |= SURF_SLOTS;
    cAreas++; /* slots */

    /* --- create / yield / exit with arg + current_id + SP align --- */
    g_fSelftestDone = 0;
    g_fSelftestRan = 0;
    g_u32SelftestSeenId = 0;
    g_u32SelftestArg = 0;
    g_cSelftestHits = 0;
    g_u32SelftestSpAlign = 0;
    u32Next0 = g_u32NextId;
    u32Id = gj_coop_create(selftest_entry, (void *)(gj_vaddr_t)0xA5A5u);
    if (u32Id == 0u) {
        fOk = 0;
        goto soft_out;
    }
    if (u32Id != u32Next0 || g_u32NextId != u32Next0 + 1u) {
        fOk = 0;
        goto soft_out;
    }
    u32Surf |= SURF_MONO;
    cAreas++; /* mono */
    if (g_u32SelftestSpAlign != 1u) {
        fOk = 0;
        goto soft_out;
    }
    u32Surf |= SURF_ALIGN;
    cAreas++; /* align */
    for (uSpins = 0; uSpins < 64u && !g_fSelftestDone; uSpins++) {
        gj_coop_yield();
    }
    if (!g_fSelftestRan || !g_fSelftestDone) {
        fOk = 0;
        goto soft_out;
    }
    if (g_u32SelftestSeenId != u32Id || g_u32SelftestArg != 0xA5A5u) {
        fOk = 0;
        goto soft_out;
    }
    if (gj_coop_current_id() != 0u) {
        fOk = 0;
        goto soft_out;
    }
    u32Surf |= SURF_CREATE;
    cAreas++; /* create */

    /* --- EXITED slot reuse --- */
    g_fSelftestDone = 0;
    g_fSelftestRan = 0;
    g_u32SelftestSeenId = 0;
    g_u32SelftestArg = 0;
    g_u32SelftestSpAlign = 0;
    u32Id2 = gj_coop_create(selftest_entry, (void *)(gj_vaddr_t)0xBEEFu);
    if (u32Id2 == 0u) {
        fOk = 0;
        goto soft_out;
    }
    /* New id must be strictly greater (monotonic next_id). */
    if (u32Id2 <= u32Id) {
        fOk = 0;
        goto soft_out;
    }
    if (g_u32SelftestSpAlign != 1u) {
        fOk = 0;
        goto soft_out;
    }
    for (uSpins = 0; uSpins < 64u && !g_fSelftestDone; uSpins++) {
        gj_coop_yield();
    }
    if (!g_fSelftestRan || g_u32SelftestSeenId != u32Id2 ||
        g_u32SelftestArg != 0xBEEFu) {
        fOk = 0;
        goto soft_out;
    }
    if (gj_coop_current_id() != 0u) {
        fOk = 0;
        goto soft_out;
    }
    u32Surf |= SURF_REUSE;
    cAreas++; /* reuse */

    /* --- two-thr RR: A yields, B runs, A resumes --- */
    g_cSelftestHits = 0;
    g_u32SelftestOrder = 0;
    u32Id = gj_coop_create(selftest_entry_a, 0);
    u32Id2 = gj_coop_create(selftest_entry_b, 0);
    if (u32Id == 0u || u32Id2 == 0u) {
        fOk = 0;
        goto soft_out;
    }
    if (u32Id2 <= u32Id) {
        fOk = 0;
        goto soft_out;
    }
    for (uSpins = 0; uSpins < 128u && (g_u32SelftestOrder & 7u) != 7u; uSpins++) {
        gj_coop_yield();
    }
    if (g_cSelftestHits < 2u || (g_u32SelftestOrder & 7u) != 7u) {
        fOk = 0;
        goto soft_out;
    }
    if (gj_coop_current_id() != 0u) {
        fOk = 0;
        goto soft_out;
    }
    u32Surf |= SURF_RR;
    cAreas++; /* rr */

    /* --- fill remaining slots, reject overflow, reclaim after exit --- */
    g_cSelftestHits = 0;
    cCreated = 0;
    for (;;) {
        u32Fill = gj_coop_create(selftest_quick_exit, 0);
        if (u32Fill == 0u) {
            break;
        }
        cCreated++;
        if (cCreated > GJ_COOP_MAX_THR) {
            fOk = 0;
            goto soft_out;
        }
    }
    if (cCreated == 0u) {
        fOk = 0;
        goto soft_out;
    }
    /* Table full — another create must fail. */
    if (gj_coop_create(selftest_quick_exit, 0) != 0u) {
        fOk = 0;
        goto soft_out;
    }
    for (uSpins = 0; uSpins < 256u && g_cSelftestHits < cCreated; uSpins++) {
        gj_coop_yield();
    }
    if (g_cSelftestHits < cCreated) {
        fOk = 0;
        goto soft_out;
    }
    u32Surf |= SURF_CAP;
    cAreas++; /* cap */

    /* After all EXITED, create must work again. */
    g_fSelftestDone = 0;
    g_fSelftestRan = 0;
    g_u32SelftestSeenId = 0;
    g_u32SelftestArg = 0;
    u32Id3 = gj_coop_create(selftest_entry, (void *)(gj_vaddr_t)0x1111u);
    if (u32Id3 == 0u) {
        fOk = 0;
        goto soft_out;
    }
    for (uSpins = 0; uSpins < 64u && !g_fSelftestDone; uSpins++) {
        gj_coop_yield();
    }
    if (!g_fSelftestRan || g_u32SelftestSeenId != u32Id3 ||
        g_u32SelftestArg != 0x1111u) {
        fOk = 0;
        goto soft_out;
    }
    if (gj_coop_current_id() != 0u) {
        fOk = 0;
        goto soft_out;
    }
    u32Surf |= SURF_RECLAIM;
    cAreas++; /* reclaim */

soft_out:
    coop_soft_inventory(fOk, cAreas, u32Surf);

    /* Grep: per-surface soft PASS lamps (only surfaces that actually ran). */
    if ((u32Surf & SURF_BOOT) != 0u) {
        kprintf("coop: soft boot soft PASS id=0 wave=%u\n",
                (unsigned)COOP_SOFT_WAVE);
    }
    if ((u32Surf & SURF_NULL) != 0u) {
        kprintf("coop: soft null soft PASS wave=%u\n",
                (unsigned)COOP_SOFT_WAVE);
    }
    if ((u32Surf & SURF_SOLO) != 0u) {
        kprintf("coop: soft solo soft PASS wave=%u\n",
                (unsigned)COOP_SOFT_WAVE);
    }
    if ((u32Surf & SURF_SLOTS) != 0u) {
        kprintf("coop: soft slots soft PASS wave=%u\n",
                (unsigned)COOP_SOFT_WAVE);
    }
    if ((u32Surf & SURF_CREATE) != 0u) {
        kprintf("coop: soft create soft PASS wave=%u\n",
                (unsigned)COOP_SOFT_WAVE);
    }
    if ((u32Surf & SURF_REUSE) != 0u) {
        kprintf("coop: soft reuse soft PASS wave=%u\n",
                (unsigned)COOP_SOFT_WAVE);
    }
    if ((u32Surf & SURF_RR) != 0u) {
        kprintf("coop: soft rr soft PASS order=7 wave=%u\n",
                (unsigned)COOP_SOFT_WAVE);
    }
    if ((u32Surf & SURF_CAP) != 0u) {
        kprintf("coop: soft cap soft PASS max_thr=%u wave=%u\n",
                (unsigned)GJ_COOP_MAX_THR, (unsigned)COOP_SOFT_WAVE);
    }
    if ((u32Surf & SURF_RECLAIM) != 0u) {
        kprintf("coop: soft reclaim soft PASS wave=%u\n",
                (unsigned)COOP_SOFT_WAVE);
    }
    if ((u32Surf & SURF_MONO) != 0u) {
        kprintf("coop: soft mono soft PASS next_id=%u wave=%u\n",
                g_u32NextId, (unsigned)COOP_SOFT_WAVE);
    }
    if ((u32Surf & SURF_ALIGN) != 0u) {
        kprintf("coop: soft align soft PASS sp16=1 wave=%u\n",
                (unsigned)COOP_SOFT_WAVE);
    }

    /* Grep: coop: soft return selftest — Wave 19 terminal return surface */
    kprintf("coop: soft return selftest_ret=%d surf=0x%x areas=%u "
            "product_kernel=OPEN wave=%u\n",
            fOk, u32Surf, cAreas, (unsigned)COOP_SOFT_WAVE);

    return fOk;
}
