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
 *   - linux_time_soft_irq_inject(name): soft-call recorded handler (freestanding)
 *   - _printk / _dev_err / _dev_info / _dev_warn: rate-capped kprintf soft
 *   - memcpy / memset: register freestanding string.c addresses (no reimpl)
 *
 * Soft ≠ product: no live PIC/APIC delivery; inject is explicit soft path only.
 *
 * Greppable markers (keep stable):
 *   linux_time_soft: soft init PASS
 *   linux_time_soft: soft ksym register PASS|SKIP
 *   linux_time_soft: soft irq bind irq=…
 *   linux_time_soft: soft irq free irq=…
 *   linux_time_soft: soft irq inject name=…
 *   linux_time_soft: soft msleep …
 *   linux_time_soft: soft _printk …
 *   linux_time_soft: soft _dev_err …
 */
#include <gj/klog.h>
#include <gj/linux_time_soft.h>
#include <gj/string.h>
#include <gj/timer.h>
#include <gj/types.h>
#include <stdarg.h>

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
static u32  g_cIrqInject;
static u32  g_cIrqInjectFail;
static u32  g_cPrintk;
static u32  g_cDevErr;
static u32  g_cDevInfo;
static u32  g_cDevWarn;

/** Soft log cap for _printk / _dev_* (once-ish; Soft≠product flood). */
#define LTS_LOG_CAP  32u

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

void
usleep_range_state(unsigned long min, unsigned long max, unsigned state)
{
    /*
     * Soft: ignore TASK_* state (no real schedule). Same budget as
     * usleep_range. Post-probe r8169 delay path. Soft≠product.
     */
    (void)state;
    usleep_range(min, max);
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

/**
 * Soft IRQ inject by request_irq name.
 * Looks up the soft table for a matching name, and if a handler was recorded
 * and disable-depth is 0, calls handler(irq, dev). Optional threaded wake
 * (soft ret == 2) runs thread_fn. Soft≠product: no APIC; freestanding later.
 * Grep: linux_time_soft: soft irq inject name=
 *
 * Returns 0 on call, LTS_EINVAL if not found / no handler, LTS_EBUSY if
 * disabled.
 */
int
linux_time_soft_irq_inject(const char *szName)
{
    u32 i;
    struct lts_irq_slot *pSlot;
    int (*pfnHandler)(int nIrq, void *pDev);
    int (*pfnThread)(int nIrq, void *pDev);
    int nRet;
    /* Soft IRQ_WAKE_THREAD mental model (Linux value is 2). */
    const int nWakeThread = 2;

    if (!g_fReady) {
        linux_time_soft_init();
    }
    if (szName == NULL || szName[0] == '\0') {
        if (g_cIrqInjectFail < 0xffffffffu) {
            g_cIrqInjectFail++;
        }
        return LTS_EINVAL;
    }

    for (i = 0u; i < LINUX_TIME_SOFT_IRQ_MAX; i++) {
        pSlot = &g_aIrq[i];
        if (pSlot->u8Used == 0u || pSlot->szName == NULL) {
            continue;
        }
        if (strcmp(pSlot->szName, szName) != 0) {
            continue;
        }

        if (pSlot->u32DisableDepth > 0u) {
            if (g_cIrqInjectFail < 0xffffffffu) {
                g_cIrqInjectFail++;
            }
            /* Grep: linux_time_soft: soft irq inject name= */
            kprintf("linux_time_soft: soft irq inject name=%s irq=%u "
                    "SKIP disabled depth=%u (Soft≠product)\n",
                    szName, (unsigned)i, (unsigned)pSlot->u32DisableDepth);
            return LTS_EBUSY;
        }

        pfnHandler = (int (*)(int, void *))(uintptr_t)pSlot->pfnHandler;
        if (pfnHandler == NULL) {
            if (g_cIrqInjectFail < 0xffffffffu) {
                g_cIrqInjectFail++;
            }
            return LTS_EINVAL;
        }

        nRet = pfnHandler((int)i, pSlot->pDev);

        /* Soft threaded path: primary returned wake-thread and thread_fn set. */
        if (nRet == nWakeThread && pSlot->pfnThread != NULL &&
            pSlot->pfnThread != pSlot->pfnHandler) {
            pfnThread = (int (*)(int, void *))(uintptr_t)pSlot->pfnThread;
            nRet = pfnThread((int)i, pSlot->pDev);
        }

        if (g_cIrqInject < 0xffffffffu) {
            g_cIrqInject++;
        }
        /* Grep: linux_time_soft: soft irq inject name= */
        kprintf("linux_time_soft: soft irq inject name=%s irq=%u "
                "handler=%p dev=%p ret=%d calls=%u (Soft≠product)\n",
                szName, (unsigned)i, pSlot->pfnHandler, pSlot->pDev, nRet,
                (unsigned)g_cIrqInject);
        (void)lts_jiffies_sync();
        return 0;
    }

    if (g_cIrqInjectFail < 0xffffffffu) {
        g_cIrqInjectFail++;
    }
    /* Grep: linux_time_soft: soft irq inject name= */
    kprintf("linux_time_soft: soft irq inject name=%s SKIP unbound "
            "(Soft≠product)\n",
            szName);
    return LTS_EINVAL;
}

u32
linux_time_soft_irq_inject_count(void)
{
    return g_cIrqInject;
}

/* ---- Soft _printk / _dev_* (re-register over empty ksym stubs) ---------- */

/**
 * Soft printk path: print format string only (varargs ignored — Soft≠
 * full vprintk). Rate-capped. Returns 0 (Linux int _printk).
 */
int
_printk(const char *szFmt, ...)
{
    va_list vaArgs;

    va_start(vaArgs, szFmt);
    va_end(vaArgs);

    if (g_cPrintk < 0xffffffffu) {
        g_cPrintk++;
    }
    if (g_cPrintk > LTS_LOG_CAP) {
        return 0;
    }
    /* Grep: linux_time_soft: soft _printk */
    kprintf("linux_time_soft: soft _printk calls=%u %s\n",
            (unsigned)g_cPrintk,
            (szFmt != NULL && szFmt[0] != '\0') ? szFmt : "");
    return 0;
}

static int
lts_dev_log(u32 *pu32Ctr, const char *szLevel, const void *pDev,
            const char *szFmt)
{
    if (pu32Ctr == NULL) {
        return 0;
    }
    if (*pu32Ctr < 0xffffffffu) {
        (*pu32Ctr)++;
    }
    if (*pu32Ctr > LTS_LOG_CAP) {
        return 0;
    }
    /* Grep: linux_time_soft: soft _dev_err (and info/warn share prefix) */
    kprintf("linux_time_soft: soft _dev_%s dev=%p calls=%u %s\n",
            (szLevel != NULL) ? szLevel : "err",
            pDev,
            (unsigned)*pu32Ctr,
            (szFmt != NULL && szFmt[0] != '\0') ? szFmt : "");
    return 0;
}

int
_dev_err(const void *pDev, const char *szFmt, ...)
{
    va_list vaArgs;

    va_start(vaArgs, szFmt);
    va_end(vaArgs);
    return lts_dev_log(&g_cDevErr, "err", pDev, szFmt);
}

int
_dev_info(const void *pDev, const char *szFmt, ...)
{
    va_list vaArgs;

    va_start(vaArgs, szFmt);
    va_end(vaArgs);
    return lts_dev_log(&g_cDevInfo, "info", pDev, szFmt);
}

int
_dev_warn(const void *pDev, const char *szFmt, ...)
{
    va_list vaArgs;

    va_start(vaArgs, szFmt);
    va_end(vaArgs);
    return lts_dev_log(&g_cDevWarn, "warn", pDev, szFmt);
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
    g_cIrqInject = 0u;
    g_cIrqInjectFail = 0u;
    g_cPrintk = 0u;
    g_cDevErr = 0u;
    g_cDevInfo = 0u;
    g_cDevWarn = 0u;
    memset(g_aIrq, 0, sizeof(g_aIrq));
    (void)lts_jiffies_sync();
    g_fReady = 1;

    /*
     * Export soft bodies for F2 ksym / module resolve.
     * Weak linux_ksym_register: no-op SKIP when F2 not linked yet.
     * memcpy/memset: real freestanding string helpers (do not reimplement).
     * _printk / _dev_*: replace empty ksym stubs for r8169 early probe logs.
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
    lts_ksym_one("usleep_range_state", (void *)usleep_range_state, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("request_irq", (void *)request_irq, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("request_threaded_irq", (void *)request_threaded_irq,
                 &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("free_irq", (void *)free_irq, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("enable_irq", (void *)enable_irq, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("disable_irq", (void *)disable_irq, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("_printk", (void *)_printk, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("printk", (void *)_printk, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("_dev_err", (void *)_dev_err, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("_dev_info", (void *)_dev_info, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("_dev_warn", (void *)_dev_warn, &u32KsymOk, &u32KsymSkip);
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
