/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Cooperative RR scheduler core for shared aarch64 product.
 * Stack frames built for AArch64 AAPCS64 (x29/x30 + entry).
 */
#include <gj/sched_coop.h>
#include <gj/string.h>

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
static volatile int g_fSelftestDone;
static volatile int g_fSelftestRan;

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

static void
selftest_entry(void *pArg)
{
    (void)pArg;
    g_fSelftestRan = 1;
    g_fSelftestDone = 1;
    gj_coop_exit();
}

int
gj_coop_selftest(void)
{
    u32 id;
    unsigned spins;

    if (!g_fInited) {
        gj_coop_init();
    }
    g_fSelftestDone = 0;
    g_fSelftestRan = 0;
    id = gj_coop_create(selftest_entry, 0);
    if (id == 0) {
        return 0;
    }
    for (spins = 0; spins < 64u && !g_fSelftestDone; spins++) {
        gj_coop_yield();
    }
    return (g_fSelftestRan && g_fSelftestDone) ? 1 : 0;
}
