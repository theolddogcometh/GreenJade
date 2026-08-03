/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux time / delay / IRQ-request bodies.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux .ko source.
 *
 * Soft only:
 *   - linux_time_soft_init: ready lamp + linux_ksym_register of bodies
 *   - jiffies: exported volatile unsigned long; sync from timer_jiffies or bump
 *   - msleep / udelay / usleep_range / __const_udelay: capped spins + jiffies
 *   - request_irq table (32 lines): store handler; greppable bind log
 *   - memcpy / memset: register freestanding string.c addresses (no reimpl)
 *
 * Soft ≠ product: no live IRQ delivery, no unbounded sleep.
 *
 * Greppable markers (keep stable):
 *   linux_time_soft: soft init PASS
 *   linux_time_soft: soft ksym register PASS|SKIP
 *   linux_time_soft: soft irq bind irq=…
 *   linux_time_soft: soft irq free irq=…
 *   linux_time_soft: soft msleep …
 */
#include <gj/klog.h>
#include <gj/linux_time_soft.h>
#include <gj/string.h>
#include <gj/timer.h>
#include <gj/types.h>

/*
 * F2 linux_ksym may be linked later. Weak unresolved → NULL; init skips export.
 * Coordinator may also point ksym at these symbols by C name without this call.
 */
int linux_ksym_register(const char *szName, void *pFn) __attribute__((weak));

/* Soft errno-shaped returns (Linux negative errno mental model). */
#define LTS_EINVAL  (-22)
#define LTS_EBUSY   (-16)

/* ---- Soft state --------------------------------------------------------- */

volatile unsigned long jiffies;

static int  g_fReady;
static u32  g_cInitCalls;
static u32  g_cMsleep;
static u32  g_cUdelay;
static u32  g_cUsleep;
static u32  g_cConstUdelay;
static u32  g_cReqIrq;
static u32  g_cReqIrqOk;
static u32  g_cReqIrqFail;
static u32  g_cFreeIrq;
static u32  g_cEnableIrq;
static u32  g_cDisableIrq;
static u32  g_cPoll;
static u32  g_cIrqBound;

struct lts_irq_slot {
    void             *pfnHandler;
    void             *pfnThread;
    void             *pDev;
    const char       *szName;
    unsigned long     uFlags;
    u32               u32DisableDepth;
    u8                u8Used;
    u8                u8Pad[3];
};

static struct lts_irq_slot g_aIrq[LINUX_TIME_SOFT_IRQ_MAX];

/* ---- Helpers ------------------------------------------------------------ */

static void
lts_ksym_one(const char *szName, void *pFn, u32 *pu32Ok, u32 *pu32Skip)
{
    int nSt;

    if (szName == NULL || pFn == NULL) {
        if (pu32Skip != NULL && *pu32Skip < 0xffffffffu) {
            (*pu32Skip)++;
        }
        return;
    }
    if (linux_ksym_register == NULL) {
        if (pu32Skip != NULL && *pu32Skip < 0xffffffffu) {
            (*pu32Skip)++;
        }
        return;
    }
    nSt = linux_ksym_register(szName, pFn);
    if (nSt == 0) {
        if (pu32Ok != NULL && *pu32Ok < 0xffffffffu) {
            (*pu32Ok)++;
        }
    } else {
        if (pu32Skip != NULL && *pu32Skip < 0xffffffffu) {
            (*pu32Skip)++;
        }
    }
}

/**
 * Pull soft jiffies from the product mono clock when ready; else leave as-is.
 * Always returns the value written into jiffies.
 */
static unsigned long
lts_jiffies_sync(void)
{
    u64 u64Tj;

    u64Tj = timer_jiffies();
    /* timer_jiffies is live once timer_init has run (may be 0 early). */
    jiffies = (unsigned long)u64Tj;
    return jiffies;
}

/**
 * Soft bump when mono may not advance under spin (no IRQs / early boot).
 * Guarantees delay paths leave an observable jiffies change.
 */
static void
lts_jiffies_bump(unsigned long uDelta)
{
    if (uDelta == 0ul) {
        uDelta = 1ul;
    }
    jiffies += uDelta;
}

/* ---- jiffies poll ------------------------------------------------------- */

void
linux_time_soft_poll(void)
{
    unsigned long uBefore;
    unsigned long uAfter;

    if (g_cPoll < 0xffffffffu) {
        g_cPoll++;
    }
    uBefore = jiffies;
    uAfter = lts_jiffies_sync();
    if (uAfter == uBefore) {
        lts_jiffies_bump(1ul);
    }
}

/* ---- Delays ------------------------------------------------------------- */

void
msleep(unsigned int msecs)
{
    u64 u64Start;
    u64 u64Need;
    u64 u64Now;
    u32 u32Guard;
    unsigned long uBefore;

    if (g_cMsleep < 0xffffffffu) {
        g_cMsleep++;
    }

    uBefore = lts_jiffies_sync();

    /*
     * Convert ms → soft jiffy budget (GJ_TIMER_HZ ticks/sec).
     * Cap so soft never hangs product bring-up (Soft≠product sleep).
     */
    if (msecs == 0u) {
        lts_jiffies_bump(1ul);
        /* Grep: linux_time_soft: soft msleep */
        kprintf("linux_time_soft: soft msleep msecs=0 jiffies=%lu calls=%u\n",
                (unsigned long)jiffies, (unsigned)g_cMsleep);
        return;
    }

    u64Need = ((u64)msecs * (u64)GJ_TIMER_HZ + 999ull) / 1000ull;
    if (u64Need < 1ull) {
        u64Need = 1ull;
    }
    if (u64Need > 200ull) {
        u64Need = 200ull;
    }

    u64Start = timer_jiffies();
    for (u32Guard = 0u; u32Guard < 5000000u; u32Guard++) {
        u64Now = timer_jiffies();
        jiffies = (unsigned long)u64Now;
        if ((u64Now - u64Start) >= u64Need) {
            break;
        }
    }

    /* If mono did not advance, static-bump so the wait is still observable. */
    if (jiffies == uBefore || (timer_jiffies() - u64Start) < u64Need) {
        lts_jiffies_bump((unsigned long)u64Need);
    }

    /* Grep: linux_time_soft: soft msleep */
    kprintf("linux_time_soft: soft msleep msecs=%u need_j=%lu jiffies=%lu "
            "calls=%u\n",
            msecs, (unsigned long)u64Need, (unsigned long)jiffies,
            (unsigned)g_cMsleep);
}

void
__const_udelay(unsigned long xloops)
{
    volatile unsigned long uSpins;
    u32 u32Guard;

    if (g_cConstUdelay < 0xffffffffu) {
        g_cConstUdelay++;
    }

    /*
     * Linux scales xloops by loops_per_jiffy; soft burns a tiny bounded
     * count so udelay chains never hang. Always touch jiffies once.
     */
    uSpins = xloops;
    if (uSpins > 100000ul) {
        uSpins = 100000ul;
    }
    if (uSpins == 0ul) {
        uSpins = 1ul;
    }
    for (u32Guard = 0u; u32Guard < (u32)uSpins; u32Guard++) {
        /* busy soft spin */
    }
    (void)lts_jiffies_sync();
    /* Sub-jiffy soft path still leaves a breadcrumb every N calls. */
    if ((g_cConstUdelay & 0x3fu) == 1u) {
        lts_jiffies_bump(1ul);
    }
}

void
udelay(unsigned long usecs)
{
    unsigned long uLoops;

    if (g_cUdelay < 0xffffffffu) {
        g_cUdelay++;
    }

    /*
     * Soft map: a few spins per usec, hard-capped. Real LPJ is not calibrated
     * here (Soft≠product).
     */
    if (usecs == 0ul) {
        usecs = 1ul;
    }
    if (usecs > 10000ul) {
        usecs = 10000ul;
    }
    uLoops = usecs * 4ul;
    __const_udelay(uLoops);
}

void
usleep_range(unsigned long min, unsigned long max)
{
    unsigned long uUse;
    u64 u64Start;
    u64 u64Need;
    u32 u32Guard;

    (void)max;
    if (g_cUsleep < 0xffffffffu) {
        g_cUsleep++;
    }

    uUse = min;
    if (uUse == 0ul) {
        uUse = 1ul;
    }
    /* Soft: treat range as short spin; ≥1ms maps to at least one jiffy tick. */
    if (uUse >= 1000ul) {
        u64Need = (uUse / 1000ul);
        if (u64Need > 50ull) {
            u64Need = 50ull;
        }
        u64Start = timer_jiffies();
        for (u32Guard = 0u; u32Guard < 1000000u; u32Guard++) {
            jiffies = (unsigned long)timer_jiffies();
            if ((timer_jiffies() - u64Start) >= u64Need) {
                break;
            }
        }
        if ((timer_jiffies() - u64Start) < u64Need) {
            lts_jiffies_bump((unsigned long)u64Need);
        }
    } else {
        udelay(uUse);
        lts_jiffies_bump(1ul);
    }
}

/* ---- IRQ soft table ----------------------------------------------------- */

static int
lts_irq_bind(unsigned int irq, void *handler, void *thread_fn,
             unsigned long flags, const char *name, void *dev)
{
    struct lts_irq_slot *pSlot;

    if (g_cReqIrq < 0xffffffffu) {
        g_cReqIrq++;
    }

    if (irq >= LINUX_TIME_SOFT_IRQ_MAX) {
        if (g_cReqIrqFail < 0xffffffffu) {
            g_cReqIrqFail++;
        }
        return LTS_EINVAL;
    }
    if (handler == NULL && thread_fn == NULL) {
        if (g_cReqIrqFail < 0xffffffffu) {
            g_cReqIrqFail++;
        }
        return LTS_EINVAL;
    }

    pSlot = &g_aIrq[irq];
    if (pSlot->u8Used) {
        /* Soft: same dev re-bind OK; different dev → busy (no share table). */
        if (pSlot->pDev != dev) {
            if (g_cReqIrqFail < 0xffffffffu) {
                g_cReqIrqFail++;
            }
            return LTS_EBUSY;
        }
    } else {
        if (g_cIrqBound < 0xffffffffu) {
            g_cIrqBound++;
        }
    }

    pSlot->pfnHandler = (handler != NULL) ? handler : thread_fn;
    pSlot->pfnThread = thread_fn;
    pSlot->pDev = dev;
    pSlot->szName = name;
    pSlot->uFlags = flags;
    pSlot->u32DisableDepth = 0u;
    pSlot->u8Used = 1u;

    if (g_cReqIrqOk < 0xffffffffu) {
        g_cReqIrqOk++;
    }

    /* Grep: linux_time_soft: soft irq bind irq= */
    kprintf("linux_time_soft: soft irq bind irq=%u name=%s flags=0x%lx "
            "handler=%p thread=%p dev=%p bound=%u\n",
            irq,
            (name != NULL) ? name : "?",
            flags,
            pSlot->pfnHandler,
            pSlot->pfnThread,
            pSlot->pDev,
            (unsigned)g_cIrqBound);

    (void)lts_jiffies_sync();
    return 0;
}

int
request_irq(unsigned int irq, void *handler, unsigned long flags,
            const char *name, void *dev)
{
    return lts_irq_bind(irq, handler, NULL, flags, name, dev);
}

int
request_threaded_irq(unsigned int irq, void *handler, void *thread_fn,
                     unsigned long flags, const char *name, void *dev)
{
    return lts_irq_bind(irq, handler, thread_fn, flags, name, dev);
}

void
free_irq(unsigned int irq, void *dev)
{
    struct lts_irq_slot *pSlot;

    if (g_cFreeIrq < 0xffffffffu) {
        g_cFreeIrq++;
    }
    if (irq >= LINUX_TIME_SOFT_IRQ_MAX) {
        return;
    }
    pSlot = &g_aIrq[irq];
    if (!pSlot->u8Used) {
        return;
    }
    /* Linux free_irq keys on dev_id; soft: NULL dev matches any occupant. */
    if (dev != NULL && pSlot->pDev != dev) {
        return;
    }

    /* Grep: linux_time_soft: soft irq free irq= */
    kprintf("linux_time_soft: soft irq free irq=%u name=%s dev=%p\n",
            irq,
            (pSlot->szName != NULL) ? pSlot->szName : "?",
            pSlot->pDev);

    pSlot->pfnHandler = NULL;
    pSlot->pfnThread = NULL;
    pSlot->pDev = NULL;
    pSlot->szName = NULL;
    pSlot->uFlags = 0ul;
    pSlot->u32DisableDepth = 0u;
    pSlot->u8Used = 0u;
    if (g_cIrqBound > 0u) {
        g_cIrqBound--;
    }
}

void
enable_irq(unsigned int irq)
{
    struct lts_irq_slot *pSlot;

    if (g_cEnableIrq < 0xffffffffu) {
        g_cEnableIrq++;
    }
    if (irq >= LINUX_TIME_SOFT_IRQ_MAX) {
        return;
    }
    pSlot = &g_aIrq[irq];
    if (!pSlot->u8Used) {
        return;
    }
    if (pSlot->u32DisableDepth > 0u) {
        pSlot->u32DisableDepth--;
    }
}

void
disable_irq(unsigned int irq)
{
    struct lts_irq_slot *pSlot;

    if (g_cDisableIrq < 0xffffffffu) {
        g_cDisableIrq++;
    }
    if (irq >= LINUX_TIME_SOFT_IRQ_MAX) {
        return;
    }
    pSlot = &g_aIrq[irq];
    if (!pSlot->u8Used) {
        return;
    }
    if (pSlot->u32DisableDepth < 0xffffffffu) {
        pSlot->u32DisableDepth++;
    }
}

/* ---- Diagnostics -------------------------------------------------------- */

u32
linux_time_soft_irq_bound_count(void)
{
    return g_cIrqBound;
}

u32
linux_time_soft_msleep_calls(void)
{
    return g_cMsleep;
}

int
linux_time_soft_ready(void)
{
    return g_fReady;
}

/* ---- Init / ksym export ------------------------------------------------- */

void
linux_time_soft_init(void)
{
    u32 u32KsymOk;
    u32 u32KsymSkip;

    g_cInitCalls++;
    if (g_fReady) {
        /* Grep: linux_time_soft: soft init PASS */
        kprintf("linux_time_soft: soft init PASS (idempotent call=%u) "
                "jiffies=%lu\n",
                (unsigned)g_cInitCalls, (unsigned long)jiffies);
        return;
    }

    jiffies = 0ul;
    g_cMsleep = 0u;
    g_cUdelay = 0u;
    g_cUsleep = 0u;
    g_cConstUdelay = 0u;
    g_cReqIrq = 0u;
    g_cReqIrqOk = 0u;
    g_cReqIrqFail = 0u;
    g_cFreeIrq = 0u;
    g_cEnableIrq = 0u;
    g_cDisableIrq = 0u;
    g_cPoll = 0u;
    g_cIrqBound = 0u;
    memset(g_aIrq, 0, sizeof(g_aIrq));
    (void)lts_jiffies_sync();
    g_fReady = 1;

    /*
     * Export soft bodies for F2 ksym / module resolve.
     * Weak linux_ksym_register: no-op SKIP when F2 not linked yet.
     * memcpy/memset: real freestanding string helpers (do not reimplement).
     */
    u32KsymOk = 0u;
    u32KsymSkip = 0u;
    lts_ksym_one("jiffies", (void *)&jiffies, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("msleep", (void *)msleep, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("__const_udelay", (void *)__const_udelay, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("udelay", (void *)udelay, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("usleep_range", (void *)usleep_range, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("request_irq", (void *)request_irq, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("request_threaded_irq", (void *)request_threaded_irq,
                 &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("free_irq", (void *)free_irq, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("enable_irq", (void *)enable_irq, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("disable_irq", (void *)disable_irq, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("memcpy", (void *)memcpy, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("memset", (void *)memset, &u32KsymOk, &u32KsymSkip);

    /* Grep: linux_time_soft: soft init PASS */
    kprintf("linux_time_soft: soft init PASS irq_max=%u jiffies=%lu "
            "timer_j=%lu soft_ne_product=1 product=OPEN\n",
            (unsigned)LINUX_TIME_SOFT_IRQ_MAX,
            (unsigned long)jiffies,
            (unsigned long)timer_jiffies());

    if (u32KsymOk > 0u) {
        /* Grep: linux_time_soft: soft ksym register PASS */
        kprintf("linux_time_soft: soft ksym register PASS n=%u skip=%u\n",
                (unsigned)u32KsymOk, (unsigned)u32KsymSkip);
    } else {
        /* Grep: linux_time_soft: soft ksym register SKIP */
        kprintf("linux_time_soft: soft ksym register SKIP (no linux_ksym or "
                "all failed) skip=%u\n",
                (unsigned)u32KsymSkip);
    }
}
