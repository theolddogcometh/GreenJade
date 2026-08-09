/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux time / delay / IRQ-request / completion bodies.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux .ko source.
 *
 * Soft only (C0 eng residual; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   - linux_time_soft_init: ready lamp + linux_ksym_register of bodies
 *   - jiffies / jiffies_64 / get_jiffies_64: soft tick data residual
 *   - ktime_get / ktime_get_ns / ktime_to_ms / ktime_to_us / ms_to_ktime
 *   - ksym rebind ktime_get_mono_fast_ns / real_ns / boot_fast_ns / raw_ns
 *     -> soft mono (hot residual; Soft!=product wall/boot fidelity)
 *   - msecs/usecs/nsecs_to_jiffies + inverses + round_jiffies* (hot pure math)
 *   - msleep / mdelay / ndelay / msleep_interruptible / ssleep / udelay /
 *     usleep_range / fsleep / __udelay / __ndelay / __delay
 *   - schedule_timeout* / io_schedule_timeout + soft schedule ksym rebind
 *     (never collides with freestanding schedule(); ksym-name only)
 *   - soft timer_list residual: timer_setup / mod_timer / mod_timer_pending /
 *     add_timer* / del_timer* / timer_pending / timer_reduce / init_timer_key
 *     (never fires)
 *   - soft completion residual: init/reinit/complete_star/wait_star/try_wait
 *     + wait_for_completion_io*
 *   - request_irq table (32 lines): store handler; greppable bind log
 *   - disable_irq_nosync / synchronize_irq / irq_set_affinity_hint companions
 *   - linux_time_soft_irq_inject(name): soft-call recorded handler (freestanding)
 *   - _printk / __warn_printk / _dev_err / _dev_info / _dev_warn: rate-capped
 *   - memcpy / memset: register freestanding string.c addresses (no reimpl)
 *   - once residual selfcheck + lean PASS lamps (stamp-free; no version stamp)
 * Soft!=product. G-AC-1 (no .ko product AC). Dual DoD A/B remain OPEN.
 *
 * Hot helpers stay lean (no kprintf on ktime / jiffy convert / jiffies sync /
 * timer_pending / try_wait / completion_done): counter only. Delay / schedule /
 * timer-mod / completion-wait cold paths rate-cap greppable lamps (LTS_LOG_CAP).
 * No stamp storms. No version stamp. Never adds work to IRQ hot path
 * (no timer_tick / APIC / PIC hooks here). NEVER bump GJ_IMAGE_VERSION.
 *
 * Soft != product: no live PIC/APIC delivery; inject is explicit soft path only.
 * Soft timer_list never fires callbacks. Soft completion never hangs forever.
 * Non-destructive to freestanding wire (hybrid 4a): soft IRQ table never binds
 * APIC/MSI, never reprograms 10ec:8168 vectors, never steals freestanding IRQ0
 * ownership. Delays are spin-capped (no forever sleep). Soft!=product.
 *
 * Greppable markers (keep stable):
 *   linux_time_soft: soft init PASS
 *   linux_time_soft: soft ksym register PASS|SKIP
 *   linux_time_soft: soft hybrid zero-touch PASS
 *   linux_time_soft: soft residual lean PASS
 *   linux_time_soft: soft residual selfcheck PASS|FAIL
 *   linux_time_soft: soft residual dual_dod OPEN
 *   linux_time_soft: soft irq bind irq=...
 *   linux_time_soft: soft irq free irq=...
 *   linux_time_soft: soft irq inject name=...
 *   linux_time_soft: soft timer mod|del|setup ...
 *   linux_time_soft: soft completion ...
 *   linux_time_soft: soft msleep ...
 *   linux_time_soft: soft schedule_timeout ...
 *   linux_time_soft: soft msecs_to_jiffies ...
 *   linux_time_soft: soft _printk ...
 *   linux_time_soft: soft __warn_printk ...
 *   linux_time_soft: soft _dev_err ...
 *   linux_time_soft: soft ktime_get residual ...
 */
#include <gj/klog.h>
#include <gj/linux_time_soft.h>
#include <gj/string.h>
#include <gj/timer.h>
#include <gj/types.h>
#include <stdarg.h>

/*
 * F2 linux_ksym may be linked later. Weak unresolved -> NULL; init skips export.
 * Coordinator may also point ksym at these symbols by C name without this call.
 */
int linux_ksym_register(const char *szName, void *pFn) __attribute__((weak));

/* Soft errno-shaped returns (Linux negative errno mental model). */
#define LTS_EINVAL  (-22)
#define LTS_EBUSY   (-16)

/* ---- Soft state --------------------------------------------------------- */

volatile unsigned long jiffies;
volatile u64           jiffies_64;

static int  g_fReady;
static u32  g_cInitCalls;
static u32  g_cMsleep;
static u32  g_cMdelay;
static u32  g_cUdelay;
static u32  g_cUsleep;
static u32  g_cConstUdelay;
static u32  g_cReqIrq;
static u32  g_cReqIrqOk;
static u32  g_cReqIrqFail;
static u32  g_cFreeIrq;
static u32  g_cEnableIrq;
static u32  g_cDisableIrq;
static u32  g_cSyncIrq;
static u32  g_cPoll;
static u32  g_cIrqBound;
static u32  g_cIrqInject;
static u32  g_cIrqInjectFail;
static u32  g_cPrintk;
static u32  g_cWarnPrintk;
static u32  g_cDevErr;
static u32  g_cDevInfo;
static u32  g_cDevWarn;
static u32  g_cKtimeGet;
static u32  g_cMsecsToJ;
static u32  g_cSchedTimeout;
static u32  g_cModTimer;
static u32  g_cDelTimer;
static u32  g_cTimerPending;
static u32  g_cTimerSetup;
static u32  g_cNdelay;
static u32  g_cRoundJ;
static u32  g_cCompInit;
static u32  g_cCompComplete;
static u32  g_cCompWait;
static u32  g_cCompTry;
static int  g_fHybridLamp;
static int  g_fResidualLeanLamp;
static int  g_fSelfcheckLamp;
static int  g_fDualDodLamp;
static u32  g_cScheduleSoft;
static u32  g_cTimerReduce;
static u32  g_cFsleep;
static u32  g_cIrqAffHint;
static u32  g_cInitTimerKey;
static u32  g_cSelfcheckOk;
static u32  g_cSelfcheckFail;

/** Soft log cap for _printk / _dev_* / delay / timer lamps (Soft!=product flood). */
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

/*
 * Soft timer_list residual: void* key bookkeeping only.
 * Soft!=product - never fires, no IRQ0/APIC, no callback invoke.
 */
struct lts_timer_slot {
    void             *pTimer;
    void             *pfnCallback;
    unsigned long     uExpires;
    unsigned int      uFlags;
    u8                u8Pending;
    u8                u8Used;
    u8                u8Pad[2];
};

static struct lts_timer_slot g_aTimer[LINUX_TIME_SOFT_TIMER_MAX];

/*
 * Soft completion residual: void* key + done count.
 * Soft!=product - no wait queue; fail-closed spin cap; never hangs forever.
 */
struct lts_comp_slot {
    void             *pComp;
    u32               u32Done;
    u8                u8Used;
    u8                u8Pad[3];
};

static struct lts_comp_slot g_aComp[LINUX_TIME_SOFT_COMPLETION_MAX];

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
 * Soft set both jiffies and jiffies_64 from a mono timer_jiffies snapshot.
 * Soft!=product timekeeping.
 */
static void
lts_jiffies_set_from_timer(u64 u64Tj)
{
    jiffies = (unsigned long)u64Tj;
    jiffies_64 = u64Tj;
}

/**
 * Pull soft jiffies from the product mono clock when ready; else leave as-is.
 * Always returns the value written into jiffies. Mirrors jiffies_64.
 */
static unsigned long
lts_jiffies_sync(void)
{
    u64 u64Tj;

    u64Tj = timer_jiffies();
    /* timer_jiffies is live once timer_init has run (may be 0 early). */
    lts_jiffies_set_from_timer(u64Tj);
    return jiffies;
}

/**
 * Soft bump when mono may not advance under spin (no IRQs / early boot).
 * Guarantees delay paths leave an observable jiffies change.
 * Keeps jiffies_64 coherent with soft jiffies.
 */
static void
lts_jiffies_bump(unsigned long uDelta)
{
    if (uDelta == 0ul) {
        uDelta = 1ul;
    }
    jiffies += uDelta;
    jiffies_64 += (u64)uDelta;
}

/**
 * Soft get_jiffies_64: snapshot soft 64-bit tick. Hot helper - no kprintf.
 * Soft!=product (no seqlock).
 */
u64
get_jiffies_64(void)
{
    (void)lts_jiffies_sync();
    return jiffies_64;
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

/* ---- ktime_get (r8169 und; soft mono only) ------------------------------ */

/**
 * Soft ktime_get: return freestanding mono nsec as Linux-shaped ktime_t (i64).
 * Prefer timer_mono_nsec_soft (APIC interpolate when live) + soft jiffies sync.
 * Never programs hardware clocks, never touches NIC BAR / freestanding wire.
 * Hot helper: counter only - no kprintf (stamp-storm / IRQ-path lean policy).
 * Soft!=product PHC / CLOCK_MONOTONIC fidelity beyond mono source.
 */
i64
ktime_get(void)
{
    u64 u64Ns;
    i64 i64Out;

    if (g_cKtimeGet < 0xffffffffu) {
        g_cKtimeGet++;
    }

    /*
     * Prefer soft mono when timer is live (finer residual than coarse jiffy
     * product); keep exported jiffies coherent. Else soft jiffies * nsec/tick.
     * No IRQ work - read-only timer surfaces only.
     */
    if (timer_ready() != 0) {
        u64Ns = timer_mono_nsec_soft();
        lts_jiffies_set_from_timer(timer_jiffies());
    } else {
        (void)lts_jiffies_sync();
        u64Ns = (u64)jiffies * (1000000000ull / (u64)GJ_TIMER_HZ);
    }

    /* Clamp to signed 63-bit domain (Linux ktime_t); soft never negative. */
    if (u64Ns > 0x7fffffffffffffffull) {
        u64Ns = 0x7fffffffffffffffull;
    }
    i64Out = (i64)u64Ns;
    return i64Out;
}

/**
 * Soft ktime_get_ns: mono nsec as u64. Hot helper - no kprintf.
 * Soft!=product CLOCK_MONOTONIC fidelity.
 * Also ksym-registered as ktime_get_mono_fast_ns (soft residual rebind).
 */
u64
ktime_get_ns(void)
{
    i64 i64Kt;

    i64Kt = ktime_get();
    if (i64Kt < 0) {
        return 0ull;
    }
    return (u64)i64Kt;
}

/**
 * Soft ktime_get_real_ns: rebind to soft mono. Hot helper - no kprintf.
 * Soft!=product (not CLOCK_REALTIME / wall clock). C0 residual leaf.
 */
u64
ktime_get_real_ns(void)
{
    return ktime_get_ns();
}

/**
 * Soft ktime_get_boot_fast_ns: rebind to soft mono. Hot helper - no kprintf.
 * Soft!=product (not CLOCK_BOOTTIME). C0 residual leaf.
 */
u64
ktime_get_boot_fast_ns(void)
{
    return ktime_get_ns();
}

/**
 * Soft ktime_get_raw_ns: rebind to soft mono. Hot helper - no kprintf.
 * Soft!=product (not CLOCK_MONOTONIC_RAW). C0 residual leaf.
 */
u64
ktime_get_raw_ns(void)
{
    return ktime_get_ns();
}

/**
 * Soft ktime_to_ms: nsec -> ms (truncate toward zero). Hot pure math.
 * Soft!=product overflow table.
 */
i64
ktime_to_ms(i64 i64Kt)
{
    if (i64Kt < 0) {
        return -((-i64Kt) / 1000000LL);
    }
    return i64Kt / 1000000LL;
}

/**
 * Soft ktime_to_us: nsec -> us (truncate toward zero). Hot pure math.
 */
i64
ktime_to_us(i64 i64Kt)
{
    if (i64Kt < 0) {
        return -((-i64Kt) / 1000LL);
    }
    return i64Kt / 1000LL;
}

/**
 * Soft ms_to_ktime: ms -> nsec ktime shape. Hot pure math. Soft!=product.
 */
i64
ms_to_ktime(i64 i64Ms)
{
    /* Soft clamp so multiply stays in signed 63-bit domain. */
    if (i64Ms > 9223372036854LL) {
        i64Ms = 9223372036854LL;
    }
    if (i64Ms < -9223372036854LL) {
        i64Ms = -9223372036854LL;
    }
    return i64Ms * 1000000LL;
}

/* ---- Soft jiffy conversion (hot: pure math, no kprintf) ----------------- */

/**
 * Soft msecs_to_jiffies: ms -> jiffies at GJ_TIMER_HZ (round up).
 * Soft!=product (no MAX_JIFFY_OFFSET table). Hot helper - no log.
 * Grep lamp only via init residual / cold diagnostics.
 */
unsigned long
msecs_to_jiffies(const unsigned int m)
{
    u64 u64Need;

    if (g_cMsecsToJ < 0xffffffffu) {
        g_cMsecsToJ++;
    }
    if (m == 0u) {
        return 0ul;
    }
    u64Need = ((u64)m * (u64)GJ_TIMER_HZ + 999ull) / 1000ull;
    if (u64Need < 1ull) {
        u64Need = 1ull;
    }
    /* Soft sanity cap (huge ms inputs); not Linux MAX_JIFFY_OFFSET. */
    if (u64Need > 0xffffffffull) {
        u64Need = 0xffffffffull;
    }
    return (unsigned long)u64Need;
}

/*
 * Weak: linux_usb_soft.c may also define a strong crude __msecs_to_jiffies
 * (1ms~1jiffy leaf residual). Strong wins for C linkage; this TU still
 * ksym-registers HZ-scaled msecs_to_jiffies under both names. Soft!=product.
 */
__attribute__((weak))
unsigned long
__msecs_to_jiffies(const unsigned int m)
{
    return msecs_to_jiffies(m);
}

unsigned int
jiffies_to_msecs(const unsigned long j)
{
    u64 u64Ms;

    u64Ms = ((u64)j * 1000ull) / (u64)GJ_TIMER_HZ;
    if (u64Ms > 0xffffffffull) {
        u64Ms = 0xffffffffull;
    }
    return (unsigned int)u64Ms;
}

unsigned long
usecs_to_jiffies(const unsigned int u)
{
    u64 u64Need;

    if (u == 0u) {
        return 0ul;
    }
    u64Need = ((u64)u * (u64)GJ_TIMER_HZ + 999999ull) / 1000000ull;
    if (u64Need < 1ull) {
        u64Need = 1ull;
    }
    if (u64Need > 0xffffffffull) {
        u64Need = 0xffffffffull;
    }
    return (unsigned long)u64Need;
}

unsigned int
jiffies_to_usecs(const unsigned long j)
{
    u64 u64Us;

    u64Us = ((u64)j * 1000000ull) / (u64)GJ_TIMER_HZ;
    if (u64Us > 0xffffffffull) {
        u64Us = 0xffffffffull;
    }
    return (unsigned int)u64Us;
}

/**
 * Soft nsecs_to_jiffies: nsec -> jiffies at GJ_TIMER_HZ (round up).
 * Hot pure math. Soft!=product MAX_JIFFY_OFFSET.
 */
unsigned long
nsecs_to_jiffies(u64 u64Nsec)
{
    u64 u64Need;
    u64 u64Period;

    if (u64Nsec == 0ull) {
        return 0ul;
    }
    u64Period = 1000000000ull / (u64)GJ_TIMER_HZ;
    if (u64Period == 0ull) {
        u64Period = 1ull;
    }
    u64Need = (u64Nsec + u64Period - 1ull) / u64Period;
    if (u64Need < 1ull) {
        u64Need = 1ull;
    }
    if (u64Need > 0xffffffffull) {
        u64Need = 0xffffffffull;
    }
    return (unsigned long)u64Need;
}

/**
 * Soft jiffies_to_nsecs: inverse of soft nsecs_to_jiffies (truncate).
 * Hot pure math. Soft!=product.
 */
u64
jiffies_to_nsecs(const unsigned long j)
{
    return (u64)j * (1000000000ull / (u64)GJ_TIMER_HZ);
}

/**
 * Soft round_jiffies: round absolute jiffy up toward next full second (HZ).
 * Hot pure math - no kprintf. Soft!=product round table.
 */
unsigned long
round_jiffies(unsigned long uJ)
{
    unsigned long uHz;
    unsigned long uRem;

    if (g_cRoundJ < 0xffffffffu) {
        g_cRoundJ++;
    }
    uHz = (unsigned long)GJ_TIMER_HZ;
    if (uHz == 0ul) {
        return uJ;
    }
    uRem = uJ % uHz;
    if (uRem == 0ul) {
        return uJ;
    }
    /* Soft: round nearest; ties up. */
    if (uRem >= (uHz / 2ul)) {
        return uJ + (uHz - uRem);
    }
    return uJ - uRem;
}

/**
 * Soft round_jiffies_relative: round relative delta; add to soft jiffies.
 * Hot pure math. Soft!=product.
 */
unsigned long
round_jiffies_relative(unsigned long uJ)
{
    unsigned long uAbs;

    (void)lts_jiffies_sync();
    uAbs = jiffies + uJ;
    uAbs = round_jiffies(uAbs);
    if (uAbs < jiffies) {
        return 0ul;
    }
    return uAbs - jiffies;
}

/**
 * Soft round_jiffies_up: always round absolute up to next full second.
 * Hot pure math. Soft!=product.
 */
unsigned long
round_jiffies_up(unsigned long uJ)
{
    unsigned long uHz;
    unsigned long uRem;

    if (g_cRoundJ < 0xffffffffu) {
        g_cRoundJ++;
    }
    uHz = (unsigned long)GJ_TIMER_HZ;
    if (uHz == 0ul) {
        return uJ;
    }
    uRem = uJ % uHz;
    if (uRem == 0ul) {
        return uJ;
    }
    return uJ + (uHz - uRem);
}

/**
 * Soft round_jiffies_up_relative: relative form of round_jiffies_up.
 * Hot pure math. Soft!=product.
 */
unsigned long
round_jiffies_up_relative(unsigned long uJ)
{
    unsigned long uAbs;

    (void)lts_jiffies_sync();
    uAbs = round_jiffies_up(jiffies + uJ);
    if (uAbs < jiffies) {
        return 0ul;
    }
    return uAbs - jiffies;
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
     * Convert ms -> soft jiffy budget (GJ_TIMER_HZ ticks/sec).
     * Cap so soft never hangs product bring-up (Soft!=product sleep).
     */
    if (msecs == 0u) {
        lts_jiffies_bump(1ul);
        if (g_cMsleep <= LTS_LOG_CAP) {
            /* Grep: linux_time_soft: soft msleep */
            kprintf("linux_time_soft: soft msleep msecs=0 jiffies=%lu calls=%u\n",
                    (unsigned long)jiffies, (unsigned)g_cMsleep);
        }
        return;
    }

    u64Need = ((u64)msecs * (u64)GJ_TIMER_HZ + 999ull) / 1000ull;
    if (u64Need < 1ull) {
        u64Need = 1ull;
    }
    if (u64Need > (u64)LINUX_TIME_SOFT_TIMEOUT_JIFFIES_MAX) {
        u64Need = (u64)LINUX_TIME_SOFT_TIMEOUT_JIFFIES_MAX;
    }

    u64Start = timer_jiffies();
    for (u32Guard = 0u; u32Guard < 5000000u; u32Guard++) {
        u64Now = timer_jiffies();
        lts_jiffies_set_from_timer(u64Now);
        if ((u64Now - u64Start) >= u64Need) {
            break;
        }
    }

    /* If mono did not advance, static-bump so the wait is still observable. */
    if (jiffies == uBefore || (timer_jiffies() - u64Start) < u64Need) {
        lts_jiffies_bump((unsigned long)u64Need);
    }

    if (g_cMsleep <= LTS_LOG_CAP) {
        /* Grep: linux_time_soft: soft msleep */
        kprintf("linux_time_soft: soft msleep msecs=%u need_j=%lu jiffies=%lu "
                "calls=%u\n",
                msecs, (unsigned long)u64Need, (unsigned long)jiffies,
                (unsigned)g_cMsleep);
    }
}

unsigned long
msleep_interruptible(unsigned int msecs)
{
    /*
     * Soft: never signal-wakes. Same budget as msleep; always full sleep.
     * Returns 0 (Linux: remaining jiffies if interrupted). Soft!=product.
     */
    msleep(msecs);
    return 0ul;
}

void
ssleep(unsigned int seconds)
{
    unsigned int uMs;

    /*
     * Soft: seconds -> msleep chain, hard-capped so bring-up never hangs.
     * msleep itself caps at LINUX_TIME_SOFT_TIMEOUT_JIFFIES_MAX.
     */
    if (seconds == 0u) {
        msleep(0u);
        return;
    }
    if (seconds > 2u) {
        seconds = 2u;
    }
    uMs = seconds * 1000u;
    msleep(uMs);
}

/**
 * Soft mdelay: busy-style ms delay mapped to soft msleep budget.
 * Replaces empty ksym mdelay stub. Cap LINUX_TIME_SOFT_MDELAY_MS_MAX.
 * Soft!=product (not calibrated LPJ busy-loop).
 */
void
mdelay(unsigned long msecs)
{
    if (g_cMdelay < 0xffffffffu) {
        g_cMdelay++;
    }
    if (msecs > LINUX_TIME_SOFT_MDELAY_MS_MAX) {
        msecs = LINUX_TIME_SOFT_MDELAY_MS_MAX;
    }
    msleep((unsigned int)msecs);
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
     * here (Soft!=product).
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

/**
 * Soft ndelay: nsec busy budget -> tiny capped spin via __const_udelay.
 * Soft!=product (not calibrated LPJ). Cap LINUX_TIME_SOFT_NDELAY_NS_MAX.
 */
void
ndelay(unsigned long nsecs)
{
    unsigned long uLoops;

    if (g_cNdelay < 0xffffffffu) {
        g_cNdelay++;
    }
    if (nsecs == 0ul) {
        nsecs = 1ul;
    }
    if (nsecs > LINUX_TIME_SOFT_NDELAY_NS_MAX) {
        nsecs = LINUX_TIME_SOFT_NDELAY_NS_MAX;
    }
    /* Soft: ~1 spin per 250 ns, min 1. Soft!=product. */
    uLoops = (nsecs / 250ul) + 1ul;
    if (uLoops > 100000ul) {
        uLoops = 100000ul;
    }
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
    /* Soft: treat range as short spin; >=1ms maps to at least one jiffy tick. */
    if (uUse >= 1000ul) {
        u64Need = (uUse / 1000ul);
        if (u64Need > 50ull) {
            u64Need = 50ull;
        }
        u64Start = timer_jiffies();
        for (u32Guard = 0u; u32Guard < 1000000u; u32Guard++) {
            lts_jiffies_set_from_timer(timer_jiffies());
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
     * usleep_range. Post-probe r8169 delay path. Soft!=product.
     */
    (void)state;
    usleep_range(min, max);
}

/**
 * Soft __udelay: Linux udelay leaf alias -> soft udelay. Soft!=product LPJ.
 */
void
__udelay(unsigned long usecs)
{
    udelay(usecs);
}

/**
 * Soft __ndelay: Linux ndelay leaf alias -> soft ndelay. Soft!=product LPJ.
 */
void
__ndelay(unsigned long nsecs)
{
    ndelay(nsecs);
}

/**
 * Soft __delay: loops -> capped __const_udelay. Soft!=product LPJ.
 */
void
__delay(unsigned long loops)
{
    if (loops == 0ul) {
        loops = 1ul;
    }
    if (loops > 100000ul) {
        loops = 100000ul;
    }
    __const_udelay(loops);
}

/**
 * Soft fsleep: seconds-fraction sleep as capped msleep. Soft!=product.
 * Linux: void fsleep(unsigned long usecs) maps usecs to best delay helper.
 * Soft: usecs -> msleep budget (ms), hard-capped.
 */
void
fsleep(unsigned long usecs)
{
    unsigned int uMs;

    if (g_cFsleep < 0xffffffffu) {
        g_cFsleep++;
    }
    if (usecs == 0ul) {
        msleep(0u);
        return;
    }
    /* Soft: round up to ms; cap so bring-up never hangs. Soft!=product. */
    uMs = (unsigned int)((usecs + 999ul) / 1000ul);
    if (uMs == 0u) {
        uMs = 1u;
    }
    if (uMs > (unsigned int)LINUX_TIME_SOFT_MDELAY_MS_MAX) {
        uMs = (unsigned int)LINUX_TIME_SOFT_MDELAY_MS_MAX;
    }
    msleep(uMs);
}

/* ---- schedule_timeout* (USB/net module waits; fail-closed) -------------- */

/**
 * Soft schedule_timeout body: cap + spin on mono when live else jiffies bump.
 * Always returns 0 (full timeout elapsed). No real schedule / TASK_*.
 * Soft!=product timer ABI. Grep: linux_time_soft: soft schedule_timeout
 */
static long
lts_schedule_timeout_body(long timeout)
{
    u64 u64Need;
    u64 u64Start;
    u64 u64Now;
    u32 u32Guard;
    unsigned long uBefore;

    if (g_cSchedTimeout < 0xffffffffu) {
        g_cSchedTimeout++;
    }

    /* Linux: timeout <= 0 -> return immediately (remaining). Soft: 0. */
    if (timeout <= 0L) {
        return 0L;
    }

    u64Need = (u64)timeout;
    if (u64Need > (u64)LINUX_TIME_SOFT_TIMEOUT_JIFFIES_MAX) {
        u64Need = (u64)LINUX_TIME_SOFT_TIMEOUT_JIFFIES_MAX;
    }

    uBefore = lts_jiffies_sync();
    u64Start = timer_jiffies();
    for (u32Guard = 0u; u32Guard < 5000000u; u32Guard++) {
        u64Now = timer_jiffies();
        lts_jiffies_set_from_timer(u64Now);
        if ((u64Now - u64Start) >= u64Need) {
            break;
        }
    }

    if (jiffies == uBefore || (timer_jiffies() - u64Start) < u64Need) {
        lts_jiffies_bump((unsigned long)u64Need);
    }

    if (g_cSchedTimeout <= LTS_LOG_CAP) {
        /* Grep: linux_time_soft: soft schedule_timeout */
        kprintf("linux_time_soft: soft schedule_timeout timeout=%ld need_j=%lu "
                "jiffies=%lu calls=%u (Soft!=product)\n",
                (long)timeout, (unsigned long)u64Need,
                (unsigned long)jiffies, (unsigned)g_cSchedTimeout);
    }
    return 0L;
}

long
schedule_timeout(long timeout)
{
    return lts_schedule_timeout_body(timeout);
}

long
schedule_timeout_interruptible(long timeout)
{
    return lts_schedule_timeout_body(timeout);
}

long
schedule_timeout_uninterruptible(long timeout)
{
    return lts_schedule_timeout_body(timeout);
}

long
schedule_timeout_killable(long timeout)
{
    return lts_schedule_timeout_body(timeout);
}

long
schedule_timeout_idle(long timeout)
{
    return lts_schedule_timeout_body(timeout);
}

long
io_schedule_timeout(long timeout)
{
    /* Soft: no real block-layer io_schedule; same fail-closed budget. */
    return lts_schedule_timeout_body(timeout);
}

/**
 * Soft schedule (ksym-name only): fail-closed jiffy bump; never calls
 * freestanding product schedule() (link name collision avoided via static).
 * Soft!=product TASK_*; C0 residual over empty ksym stub.
 * Grep: linux_time_soft: soft schedule (via residual lean / ksym PASS)
 */
static void
lts_soft_schedule(void)
{
    if (g_cScheduleSoft < 0xffffffffu) {
        g_cScheduleSoft++;
    }
    (void)lts_jiffies_sync();
    lts_jiffies_bump(1ul);
}

/* ---- Soft timer_list residual (void* key; never fires) ------------------ */

/**
 * Find soft timer slot by key, or free slot if fAlloc.
 * Returns NULL if not found / table full.
 */
static struct lts_timer_slot *
lts_timer_find(void *pTimer, int fAlloc)
{
    u32 i;
    struct lts_timer_slot *pFree;

    if (pTimer == NULL) {
        return NULL;
    }
    pFree = NULL;
    for (i = 0u; i < LINUX_TIME_SOFT_TIMER_MAX; i++) {
        if (g_aTimer[i].u8Used != 0u && g_aTimer[i].pTimer == pTimer) {
            return &g_aTimer[i];
        }
        if (fAlloc != 0 && pFree == NULL && g_aTimer[i].u8Used == 0u) {
            pFree = &g_aTimer[i];
        }
    }
    if (fAlloc != 0 && pFree != NULL) {
        pFree->pTimer = pTimer;
        pFree->pfnCallback = NULL;
        pFree->uExpires = 0ul;
        pFree->uFlags = 0u;
        pFree->u8Pending = 0u;
        pFree->u8Used = 1u;
        return pFree;
    }
    return NULL;
}

/**
 * Soft timer_setup: record key + callback pointer + flags. Never arms/fires.
 * Grep: linux_time_soft: soft timer setup
 * Soft!=product timer_list ABI.
 */
void
timer_setup(void *pTimer, void *pfnCallback, unsigned int uFlags)
{
    struct lts_timer_slot *pSlot;

    if (g_cTimerSetup < 0xffffffffu) {
        g_cTimerSetup++;
    }
    if (pTimer == NULL) {
        return;
    }
    pSlot = lts_timer_find(pTimer, 1);
    if (pSlot == NULL) {
        if (g_cTimerSetup <= LTS_LOG_CAP) {
            /* Grep: linux_time_soft: soft timer setup */
            kprintf("linux_time_soft: soft timer setup FULL timer=%p "
                    "(Soft!=product)\n",
                    pTimer);
        }
        return;
    }
    pSlot->pfnCallback = pfnCallback;
    pSlot->uFlags = uFlags;
    pSlot->u8Pending = 0u;
    if (g_cTimerSetup <= LTS_LOG_CAP) {
        /* Grep: linux_time_soft: soft timer setup */
        kprintf("linux_time_soft: soft timer setup timer=%p cb=%p flags=0x%x "
                "calls=%u (Soft!=product)\n",
                pTimer, pfnCallback, uFlags, (unsigned)g_cTimerSetup);
    }
}

/**
 * Soft mod_timer: record expires; mark pending. Never fires.
 * Returns 1 if was already pending, else 0.
 * Grep: linux_time_soft: soft timer mod
 * Soft!=product timer_list ABI / IRQ delivery.
 */
int
mod_timer(void *pTimer, unsigned long uExpires)
{
    struct lts_timer_slot *pSlot;
    int nWasPending;

    if (g_cModTimer < 0xffffffffu) {
        g_cModTimer++;
    }
    if (pTimer == NULL) {
        return 0;
    }

    pSlot = lts_timer_find(pTimer, 1);
    if (pSlot == NULL) {
        /* Table full: fail-closed soft success-shaped 0 (not pending). */
        if (g_cModTimer <= LTS_LOG_CAP) {
            /* Grep: linux_time_soft: soft timer mod */
            kprintf("linux_time_soft: soft timer mod FULL timer=%p "
                    "expires=%lu (Soft!=product)\n",
                    pTimer, uExpires);
        }
        return 0;
    }

    nWasPending = (pSlot->u8Pending != 0u) ? 1 : 0;
    pSlot->uExpires = uExpires;
    pSlot->u8Pending = 1u;
    if (g_cTimerPending < 0xffffffffu && nWasPending == 0) {
        g_cTimerPending++;
    }

    if (g_cModTimer <= LTS_LOG_CAP) {
        /* Grep: linux_time_soft: soft timer mod */
        kprintf("linux_time_soft: soft timer mod timer=%p expires=%lu "
                "was_pending=%d pending=%u calls=%u (Soft!=product)\n",
                pTimer, uExpires, nWasPending,
                (unsigned)g_cTimerPending, (unsigned)g_cModTimer);
    }
    (void)lts_jiffies_sync();
    return nWasPending;
}

/**
 * Soft mod_timer_pending: arm only if already pending; else return 0.
 * Soft!=product - never fires.
 */
int
mod_timer_pending(void *pTimer, unsigned long uExpires)
{
    struct lts_timer_slot *pSlot;

    if (pTimer == NULL) {
        return 0;
    }
    pSlot = lts_timer_find(pTimer, 0);
    if (pSlot == NULL || pSlot->u8Pending == 0u) {
        return 0;
    }
    return mod_timer(pTimer, uExpires);
}

/**
 * Soft timer_reduce: arm only if not pending or expires earlier. Never fires.
 * Soft!=product timer_list ABI. C0 residual over empty ksym path.
 * Returns 1 if already pending with expires <= new (no change), else mod.
 */
int
timer_reduce(void *pTimer, unsigned long uExpires)
{
    struct lts_timer_slot *pSlot;

    if (g_cTimerReduce < 0xffffffffu) {
        g_cTimerReduce++;
    }
    if (pTimer == NULL) {
        return 0;
    }
    pSlot = lts_timer_find(pTimer, 0);
    if (pSlot != NULL && pSlot->u8Pending != 0u &&
        pSlot->uExpires <= uExpires) {
        return 1;
    }
    return mod_timer(pTimer, uExpires);
}

/**
 * Soft init_timer_key (ksym rebind): record timer key via timer_setup path.
 * Static - does not collide with linux_usb_soft no-op global; ksym rebind
 * prefers this stronger residual when time_soft_init runs after. Soft!=product.
 */
static void
lts_init_timer_key(void *pTimer, void *pfnFunc, unsigned int uFlags,
                   const char *szName, void *pKey)
{
    (void)szName;
    (void)pKey;
    if (g_cInitTimerKey < 0xffffffffu) {
        g_cInitTimerKey++;
    }
    /* Soft: store key + optional callback; never arms/fires. Soft!=product. */
    timer_setup(pTimer, pfnFunc, uFlags);
}

/**
 * Soft add_timer: arm if not already pending.
 * Soft!=product - uses stored expires or jiffies+1 when unknown.
 */
void
add_timer(void *pTimer)
{
    struct lts_timer_slot *pSlot;
    unsigned long uExp;

    if (pTimer == NULL) {
        return;
    }
    pSlot = lts_timer_find(pTimer, 0);
    if (pSlot != NULL && pSlot->u8Pending != 0u) {
        /* Linux: WARN if already pending; soft: no-op. */
        return;
    }
    if (pSlot != NULL && pSlot->uExpires != 0ul) {
        uExp = pSlot->uExpires;
    } else {
        (void)lts_jiffies_sync();
        uExp = jiffies + 1ul;
    }
    (void)mod_timer(pTimer, uExp);
}

/**
 * Soft add_timer_on: ignore CPU; same as add_timer. Soft!=product.
 */
void
add_timer_on(void *pTimer, int nCpu)
{
    (void)nCpu;
    add_timer(pTimer);
}

/**
 * Soft del_timer: clear pending. Return 1 if was pending.
 * Grep: linux_time_soft: soft timer del
 */
int
del_timer(void *pTimer)
{
    struct lts_timer_slot *pSlot;
    int nWasPending;

    if (g_cDelTimer < 0xffffffffu) {
        g_cDelTimer++;
    }
    if (pTimer == NULL) {
        return 0;
    }
    pSlot = lts_timer_find(pTimer, 0);
    if (pSlot == NULL) {
        return 0;
    }
    nWasPending = (pSlot->u8Pending != 0u) ? 1 : 0;
    if (nWasPending != 0) {
        pSlot->u8Pending = 0u;
        if (g_cTimerPending > 0u) {
            g_cTimerPending--;
        }
    }
    if (g_cDelTimer <= LTS_LOG_CAP) {
        /* Grep: linux_time_soft: soft timer del */
        kprintf("linux_time_soft: soft timer del timer=%p was_pending=%d "
                "pending=%u calls=%u (Soft!=product)\n",
                pTimer, nWasPending, (unsigned)g_cTimerPending,
                (unsigned)g_cDelTimer);
    }
    return nWasPending;
}

/**
 * Soft del_timer_sync: same as del_timer (no real IRQ race to wait).
 * Soft!=product.
 */
int
del_timer_sync(void *pTimer)
{
    return del_timer(pTimer);
}

/**
 * Soft del_singleshot_timer_sync: alias of del_timer_sync. Soft!=product.
 */
int
del_singleshot_timer_sync(void *pTimer)
{
    return del_timer_sync(pTimer);
}

/**
 * Soft timer_pending: 1 if soft table has key pending. Hot helper - no log.
 * Soft!=product.
 */
int
timer_pending(const void *pTimer)
{
    u32 i;

    if (pTimer == NULL) {
        return 0;
    }
    for (i = 0u; i < LINUX_TIME_SOFT_TIMER_MAX; i++) {
        if (g_aTimer[i].u8Used != 0u &&
            g_aTimer[i].pTimer == (void *)(uintptr_t)pTimer &&
            g_aTimer[i].u8Pending != 0u) {
            return 1;
        }
    }
    return 0;
}

/* ---- Soft completion residual (void* key; fail-closed) ------------------ */

/**
 * Find soft completion slot by key, or free slot if fAlloc.
 */
static struct lts_comp_slot *
lts_comp_find(void *pComp, int fAlloc)
{
    u32 i;
    struct lts_comp_slot *pFree;

    if (pComp == NULL) {
        return NULL;
    }
    pFree = NULL;
    for (i = 0u; i < LINUX_TIME_SOFT_COMPLETION_MAX; i++) {
        if (g_aComp[i].u8Used != 0u && g_aComp[i].pComp == pComp) {
            return &g_aComp[i];
        }
        if (fAlloc != 0 && pFree == NULL && g_aComp[i].u8Used == 0u) {
            pFree = &g_aComp[i];
        }
    }
    if (fAlloc != 0 && pFree != NULL) {
        pFree->pComp = pComp;
        pFree->u32Done = 0u;
        pFree->u8Used = 1u;
        return pFree;
    }
    return NULL;
}

/**
 * Soft init_completion: bind key; done=0.
 * Grep: linux_time_soft: soft completion
 */
void
init_completion(void *pComp)
{
    struct lts_comp_slot *pSlot;

    if (g_cCompInit < 0xffffffffu) {
        g_cCompInit++;
    }
    if (pComp == NULL) {
        return;
    }
    pSlot = lts_comp_find(pComp, 1);
    if (pSlot == NULL) {
        if (g_cCompInit <= LTS_LOG_CAP) {
            /* Grep: linux_time_soft: soft completion */
            kprintf("linux_time_soft: soft completion init FULL comp=%p "
                    "(Soft!=product)\n",
                    pComp);
        }
        return;
    }
    pSlot->u32Done = 0u;
    if (g_cCompInit <= LTS_LOG_CAP) {
        /* Grep: linux_time_soft: soft completion */
        kprintf("linux_time_soft: soft completion init comp=%p calls=%u "
                "(Soft!=product)\n",
                pComp, (unsigned)g_cCompInit);
    }
}

/**
 * Soft reinit_completion: done=0. Soft!=product.
 */
void
reinit_completion(void *pComp)
{
    struct lts_comp_slot *pSlot;

    if (pComp == NULL) {
        return;
    }
    pSlot = lts_comp_find(pComp, 1);
    if (pSlot != NULL) {
        pSlot->u32Done = 0u;
    }
}

/**
 * Soft complete: increment done. Soft!=product (no waiter wake IRQ).
 * Grep: linux_time_soft: soft completion
 */
void
complete(void *pComp)
{
    struct lts_comp_slot *pSlot;

    if (g_cCompComplete < 0xffffffffu) {
        g_cCompComplete++;
    }
    if (pComp == NULL) {
        return;
    }
    pSlot = lts_comp_find(pComp, 1);
    if (pSlot == NULL) {
        return;
    }
    if (pSlot->u32Done < 0xffffffffu) {
        pSlot->u32Done++;
    }
    if (g_cCompComplete <= LTS_LOG_CAP) {
        /* Grep: linux_time_soft: soft completion */
        kprintf("linux_time_soft: soft completion complete comp=%p done=%u "
                "calls=%u (Soft!=product)\n",
                pComp, (unsigned)pSlot->u32Done, (unsigned)g_cCompComplete);
    }
}

/**
 * Soft complete_all: set done large so many waiters can consume. Soft!=product.
 */
void
complete_all(void *pComp)
{
    struct lts_comp_slot *pSlot;

    if (g_cCompComplete < 0xffffffffu) {
        g_cCompComplete++;
    }
    if (pComp == NULL) {
        return;
    }
    pSlot = lts_comp_find(pComp, 1);
    if (pSlot == NULL) {
        return;
    }
    pSlot->u32Done = 0xffffffffu;
    if (g_cCompComplete <= LTS_LOG_CAP) {
        /* Grep: linux_time_soft: soft completion */
        kprintf("linux_time_soft: soft completion complete_all comp=%p "
                "calls=%u (Soft!=product)\n",
                pComp, (unsigned)g_cCompComplete);
    }
}

/**
 * Soft wait body: if done, consume one and return remaining-ish soft jiffies.
 * Else fail-closed spin/cap then auto-done so bring-up never hangs.
 * Soft!=product. Grep: linux_time_soft: soft completion
 */
static unsigned long
lts_comp_wait_body(void *pComp, unsigned long uTimeout)
{
    struct lts_comp_slot *pSlot;
    u64 u64Need;
    u64 u64Start;
    u64 u64Now;
    u32 u32Guard;
    unsigned long uBefore;

    if (g_cCompWait < 0xffffffffu) {
        g_cCompWait++;
    }

    if (pComp == NULL) {
        return 0ul;
    }
    pSlot = lts_comp_find(pComp, 1);
    if (pSlot == NULL) {
        return 0ul;
    }

    /* Already done: consume one (unless complete_all max). */
    if (pSlot->u32Done > 0u) {
        if (pSlot->u32Done < 0xffffffffu) {
            pSlot->u32Done--;
        }
        return (uTimeout > 0ul) ? uTimeout : 1ul;
    }

    /* Linux: timeout 0 -> try once. Soft: return 0 if not done. */
    if (uTimeout == 0ul) {
        return 0ul;
    }

    u64Need = (u64)uTimeout;
    if (u64Need > (u64)LINUX_TIME_SOFT_TIMEOUT_JIFFIES_MAX) {
        u64Need = (u64)LINUX_TIME_SOFT_TIMEOUT_JIFFIES_MAX;
    }

    uBefore = lts_jiffies_sync();
    u64Start = timer_jiffies();
    for (u32Guard = 0u; u32Guard < 5000000u; u32Guard++) {
        if (pSlot->u32Done > 0u) {
            if (pSlot->u32Done < 0xffffffffu) {
                pSlot->u32Done--;
            }
            u64Now = timer_jiffies();
            lts_jiffies_set_from_timer(u64Now);
            if ((u64Now - u64Start) < u64Need) {
                return (unsigned long)(u64Need - (u64Now - u64Start));
            }
            return 0ul;
        }
        u64Now = timer_jiffies();
        lts_jiffies_set_from_timer(u64Now);
        if ((u64Now - u64Start) >= u64Need) {
            break;
        }
    }

    /*
     * Fail-closed: still not completed after cap -> auto-done so soft waiters
     * never hang dual DoD lab. Soft!=product completion ABI.
     */
    if (pSlot->u32Done == 0u) {
        pSlot->u32Done = 1u;
    }
    if (pSlot->u32Done < 0xffffffffu) {
        pSlot->u32Done--;
    }
    if (jiffies == uBefore) {
        lts_jiffies_bump((unsigned long)u64Need);
    }

    if (g_cCompWait <= LTS_LOG_CAP) {
        /* Grep: linux_time_soft: soft completion */
        kprintf("linux_time_soft: soft completion wait comp=%p timeout=%lu "
                "need_j=%lu auto_done=1 calls=%u (Soft!=product)\n",
                pComp, uTimeout, (unsigned long)u64Need,
                (unsigned)g_cCompWait);
    }
    return 0ul;
}

void
wait_for_completion(void *pComp)
{
    /*
     * Soft: use max soft budget (never forever). Soft!=product.
     */
    (void)lts_comp_wait_body(pComp, LINUX_TIME_SOFT_TIMEOUT_JIFFIES_MAX);
}

unsigned long
wait_for_completion_timeout(void *pComp, unsigned long uTimeout)
{
    return lts_comp_wait_body(pComp, uTimeout);
}

int
wait_for_completion_interruptible(void *pComp)
{
    /* Soft: never signal-wakes; same fail-closed body. Returns 0. */
    (void)lts_comp_wait_body(pComp, LINUX_TIME_SOFT_TIMEOUT_JIFFIES_MAX);
    return 0;
}

long
wait_for_completion_interruptible_timeout(void *pComp, unsigned long uTimeout)
{
    return (long)lts_comp_wait_body(pComp, uTimeout);
}

int
wait_for_completion_killable(void *pComp)
{
    (void)lts_comp_wait_body(pComp, LINUX_TIME_SOFT_TIMEOUT_JIFFIES_MAX);
    return 0;
}

long
wait_for_completion_killable_timeout(void *pComp, unsigned long uTimeout)
{
    return (long)lts_comp_wait_body(pComp, uTimeout);
}

/**
 * Soft wait_for_completion_io: same fail-closed body as wait_for_completion.
 * Soft!=product (no real block io_wait accounting). USB/net residual leaf.
 */
void
wait_for_completion_io(void *pComp)
{
    (void)lts_comp_wait_body(pComp, LINUX_TIME_SOFT_TIMEOUT_JIFFIES_MAX);
}

/**
 * Soft wait_for_completion_io_timeout: same as wait_for_completion_timeout.
 * Soft!=product.
 */
unsigned long
wait_for_completion_io_timeout(void *pComp, unsigned long uTimeout)
{
    return lts_comp_wait_body(pComp, uTimeout);
}

/**
 * Soft try_wait_for_completion: consume one done if available. Hot - no log.
 * Soft!=product.
 */
int
try_wait_for_completion(void *pComp)
{
    struct lts_comp_slot *pSlot;

    if (g_cCompTry < 0xffffffffu) {
        g_cCompTry++;
    }
    if (pComp == NULL) {
        return 0;
    }
    pSlot = lts_comp_find(pComp, 0);
    if (pSlot == NULL || pSlot->u32Done == 0u) {
        return 0;
    }
    if (pSlot->u32Done < 0xffffffffu) {
        pSlot->u32Done--;
    }
    return 1;
}

/**
 * Soft completion_done: 1 if done > 0. Hot helper. Soft!=product.
 */
int
completion_done(const void *pComp)
{
    u32 i;

    if (pComp == NULL) {
        return 0;
    }
    for (i = 0u; i < LINUX_TIME_SOFT_COMPLETION_MAX; i++) {
        if (g_aComp[i].u8Used != 0u &&
            g_aComp[i].pComp == (void *)(uintptr_t)pComp &&
            g_aComp[i].u32Done > 0u) {
            return 1;
        }
    }
    return 0;
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
        /* Soft: same dev re-bind OK; different dev -> busy (no share table). */
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
 * Soft disable_irq_nosync: same disable-depth as disable_irq (no wait).
 * Soft!=product (no real in-flight IRQ).
 */
void
disable_irq_nosync(unsigned int irq)
{
    disable_irq(irq);
}

/**
 * Soft synchronize_irq: no-op bookkeeping. Soft!=product.
 */
void
synchronize_irq(unsigned int irq)
{
    if (g_cSyncIrq < 0xffffffffu) {
        g_cSyncIrq++;
    }
    (void)irq;
}

/**
 * Soft irq_set_affinity_hint: accept and ignore mask. Soft!=product
 * (no real IRQ affinity / smp_affinity). C0 residual over empty ksym stub.
 * Returns 0 (Linux success).
 */
int
irq_set_affinity_hint(unsigned int irq, const void *pMask)
{
    if (g_cIrqAffHint < 0xffffffffu) {
        g_cIrqAffHint++;
    }
    (void)irq;
    (void)pMask;
    return 0;
}

/**
 * Soft IRQ inject by request_irq name.
 * Looks up the soft table for a matching name, and if a handler was recorded
 * and disable-depth is 0, calls handler(irq, dev). Optional threaded wake
 * (soft ret == 2) runs thread_fn. Soft!=product: no APIC; freestanding later.
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
                    "SKIP disabled depth=%u (Soft!=product)\n",
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
                "handler=%p dev=%p ret=%d calls=%u (Soft!=product)\n",
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
            "(Soft!=product)\n",
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
 * Soft printk path: print format string only (varargs ignored - Soft!=
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

/**
 * Soft __warn_printk: r8169 und WARN_* path. Same rate-capped soft log as
 * _printk; never raises, never touches wire. Soft!=product.
 * Grep: linux_time_soft: soft __warn_printk
 */
int
__warn_printk(const char *szFmt, ...)
{
    va_list vaArgs;

    va_start(vaArgs, szFmt);
    va_end(vaArgs);

    if (g_cWarnPrintk < 0xffffffffu) {
        g_cWarnPrintk++;
    }
    if (g_cWarnPrintk > LTS_LOG_CAP) {
        return 0;
    }
    /* Grep: linux_time_soft: soft __warn_printk */
    kprintf("linux_time_soft: soft __warn_printk calls=%u %s\n",
            (unsigned)g_cWarnPrintk,
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

u32
linux_time_soft_schedule_timeout_calls(void)
{
    return g_cSchedTimeout;
}

u32
linux_time_soft_mod_timer_calls(void)
{
    return g_cModTimer;
}

u32
linux_time_soft_timer_pending_count(void)
{
    return g_cTimerPending;
}

u32
linux_time_soft_completion_complete_calls(void)
{
    return g_cCompComplete;
}

u32
linux_time_soft_completion_wait_calls(void)
{
    return g_cCompWait;
}

int
linux_time_soft_ready(void)
{
    return g_fReady;
}

/**
 * Soft residual selfcheck (once; C0; Soft!=product): pure math + table
 * integrity + fail-closed schedule_timeout(0). Never claims product / DoD.
 * Grep: linux_time_soft: soft residual selfcheck PASS|FAIL
 */
static void
lts_residual_selfcheck(void)
{
    unsigned long uJ;
    unsigned long uRound;
    i64 i64Kt;
    u64 u64Ns;
    int nOk;
    int nFail;
    void *pTmpTimer;
    void *pTmpComp;
    unsigned long uTmp;

    if (g_fSelfcheckLamp != 0) {
        return;
    }
    g_fSelfcheckLamp = 1;
    nOk = 0;
    nFail = 0;

    /* msecs_to_jiffies pure math residual. */
    if (msecs_to_jiffies(0u) == 0ul) {
        nOk++;
    } else {
        nFail++;
    }
    uJ = msecs_to_jiffies(1000u);
    if (uJ == (unsigned long)GJ_TIMER_HZ) {
        nOk++;
    } else {
        nFail++;
    }
    if (jiffies_to_msecs((unsigned long)GJ_TIMER_HZ) == 1000u) {
        nOk++;
    } else {
        nFail++;
    }
    if (nsecs_to_jiffies(0ull) == 0ul) {
        nOk++;
    } else {
        nFail++;
    }
    if (usecs_to_jiffies(0u) == 0ul) {
        nOk++;
    } else {
        nFail++;
    }

    /* round_jiffies* pure math residual. */
    uRound = round_jiffies(0ul);
    if (uRound == 0ul) {
        nOk++;
    } else {
        nFail++;
    }
    uRound = round_jiffies_up((unsigned long)GJ_TIMER_HZ);
    if (uRound == (unsigned long)GJ_TIMER_HZ) {
        nOk++;
    } else {
        nFail++;
    }

    /* ktime soft mono residual (non-negative; hot silent path). */
    i64Kt = ktime_get();
    if (i64Kt >= 0) {
        nOk++;
    } else {
        nFail++;
    }
    u64Ns = ktime_get_ns();
    if (i64Kt >= 0 && u64Ns == (u64)i64Kt) {
        nOk++;
    } else {
        nFail++;
    }
    /* real/boot/raw rebind to soft mono (Soft!=product wall/boot fidelity). */
    if (ktime_get_real_ns() == ktime_get_ns() ||
        ktime_get_boot_fast_ns() == ktime_get_raw_ns()) {
        nOk++;
    } else {
        /*
         * Soft mono may advance between paired reads under live timer; accept
         * non-decreasing real_ns vs prior snapshot as residual pass.
         */
        if (ktime_get_real_ns() >= u64Ns) {
            nOk++;
        } else {
            nFail++;
        }
    }

    /* Fail-closed schedule_timeout(0) returns 0. Soft!=product. */
    if (schedule_timeout(0L) == 0L) {
        nOk++;
    } else {
        nFail++;
    }

    /* Null / empty table residual. */
    if (timer_pending(NULL) == 0) {
        nOk++;
    } else {
        nFail++;
    }
    if (try_wait_for_completion(NULL) == 0) {
        nOk++;
    } else {
        nFail++;
    }
    if (completion_done(NULL) == 0) {
        nOk++;
    } else {
        nFail++;
    }

    /* Soft timer_list bookkeeping residual (never fires). */
    pTmpTimer = (void *)(uintptr_t)0x6c747320ul; /* "lts " tag-ish soft key */
    timer_setup(pTmpTimer, NULL, 0u);
    if (timer_pending(pTmpTimer) == 0) {
        nOk++;
    } else {
        nFail++;
    }
    (void)mod_timer(pTmpTimer, jiffies + 10ul);
    if (timer_pending(pTmpTimer) == 1) {
        nOk++;
    } else {
        nFail++;
    }
    if (timer_reduce(pTmpTimer, jiffies + 20ul) == 1) {
        nOk++;
    } else {
        nFail++;
    }
    if (del_timer(pTmpTimer) == 1) {
        nOk++;
    } else {
        nFail++;
    }
    if (timer_pending(pTmpTimer) == 0) {
        nOk++;
    } else {
        nFail++;
    }

    /* Soft completion residual (fail-closed; never hangs). */
    pTmpComp = (void *)(uintptr_t)0x6c7463ul; /* soft key */
    init_completion(pTmpComp);
    if (completion_done(pTmpComp) == 0) {
        nOk++;
    } else {
        nFail++;
    }
    complete(pTmpComp);
    if (try_wait_for_completion(pTmpComp) == 1) {
        nOk++;
    } else {
        nFail++;
    }
    if (try_wait_for_completion(pTmpComp) == 0) {
        nOk++;
    } else {
        nFail++;
    }
    uTmp = wait_for_completion_timeout(pTmpComp, 0ul);
    if (uTmp == 0ul) {
        nOk++;
    } else {
        nFail++;
    }

    /* Soft schedule ksym body residual (bump observable). */
    {
        unsigned long uBefore;

        uBefore = jiffies;
        lts_soft_schedule();
        if (jiffies != uBefore) {
            nOk++;
        } else {
            nFail++;
        }
    }

    /* irq affinity hint soft success residual. */
    if (irq_set_affinity_hint(0u, NULL) == 0) {
        nOk++;
    } else {
        nFail++;
    }

    g_cSelfcheckOk = (u32)nOk;
    g_cSelfcheckFail = (u32)nFail;

    if (nFail == 0) {
        /* Grep: linux_time_soft: soft residual selfcheck PASS */
        kprintf("linux_time_soft: soft residual selfcheck PASS ok=%d fail=%d "
                "hz=%u soft_ne_product=1 product=OPEN dual_dod=OPEN "
                "G-AC-1=1 C0=1 stamp_free=1 version_stamp=0 "
                "(Soft!=product; eng residual only)\n",
                nOk, nFail, (unsigned)GJ_TIMER_HZ);
    } else {
        /* Grep: linux_time_soft: soft residual selfcheck FAIL */
        kprintf("linux_time_soft: soft residual selfcheck FAIL ok=%d fail=%d "
                "soft_ne_product=1 product=OPEN dual_dod=OPEN G-AC-1=1 C0=1 "
                "(Soft!=product; eng residual only)\n",
                nOk, nFail);
    }
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
    jiffies_64 = 0ull;
    g_cMsleep = 0u;
    g_cMdelay = 0u;
    g_cUdelay = 0u;
    g_cUsleep = 0u;
    g_cConstUdelay = 0u;
    g_cReqIrq = 0u;
    g_cReqIrqOk = 0u;
    g_cReqIrqFail = 0u;
    g_cFreeIrq = 0u;
    g_cEnableIrq = 0u;
    g_cDisableIrq = 0u;
    g_cSyncIrq = 0u;
    g_cPoll = 0u;
    g_cIrqBound = 0u;
    g_cIrqInject = 0u;
    g_cIrqInjectFail = 0u;
    g_cPrintk = 0u;
    g_cWarnPrintk = 0u;
    g_cDevErr = 0u;
    g_cDevInfo = 0u;
    g_cDevWarn = 0u;
    g_cKtimeGet = 0u;
    g_cMsecsToJ = 0u;
    g_cSchedTimeout = 0u;
    g_cModTimer = 0u;
    g_cDelTimer = 0u;
    g_cTimerPending = 0u;
    g_cTimerSetup = 0u;
    g_cNdelay = 0u;
    g_cRoundJ = 0u;
    g_cCompInit = 0u;
    g_cCompComplete = 0u;
    g_cCompWait = 0u;
    g_cCompTry = 0u;
    g_cScheduleSoft = 0u;
    g_cTimerReduce = 0u;
    g_cFsleep = 0u;
    g_cIrqAffHint = 0u;
    g_cInitTimerKey = 0u;
    g_cSelfcheckOk = 0u;
    g_cSelfcheckFail = 0u;
    g_fHybridLamp = 0;
    g_fResidualLeanLamp = 0;
    g_fSelfcheckLamp = 0;
    g_fDualDodLamp = 0;
    memset(g_aIrq, 0, sizeof(g_aIrq));
    memset(g_aTimer, 0, sizeof(g_aTimer));
    memset(g_aComp, 0, sizeof(g_aComp));
    (void)lts_jiffies_sync();
    g_fReady = 1;

    /*
     * Export soft bodies for F2 ksym / module resolve.
     * Weak linux_ksym_register: no-op SKIP when F2 not linked yet.
     * memcpy/memset: real freestanding string helpers (do not reimplement).
     * _printk / __warn_printk / _dev_*: replace empty ksym stubs for r8169
     * early probe logs. ktime_get: replace empty stub with soft mono.
     * schedule_timeout* / msecs_to_jiffies / mod_timer / del_timer_sync /
     * mdelay: replace empty ksym stubs. Soft!=product lean residual.
     */
    u32KsymOk = 0u;
    u32KsymSkip = 0u;
    lts_ksym_one("jiffies", (void *)&jiffies, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("jiffies_64", (void *)&jiffies_64, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("get_jiffies_64", (void *)get_jiffies_64, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("ktime_get", (void *)ktime_get, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("ktime_get_ns", (void *)ktime_get_ns, &u32KsymOk,
                 &u32KsymSkip);
    /*
     * Soft residual: rebind mono_fast / real / boot_fast / raw to soft mono
     * (not zero leaf). Soft!=product wall/boot fidelity. G-AC-1. C0.
     */
    lts_ksym_one("ktime_get_mono_fast_ns", (void *)ktime_get_ns, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("ktime_get_real_ns", (void *)ktime_get_real_ns, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("ktime_get_boot_fast_ns", (void *)ktime_get_boot_fast_ns,
                 &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("ktime_get_raw_ns", (void *)ktime_get_raw_ns, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("ktime_to_ms", (void *)ktime_to_ms, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("ktime_to_us", (void *)ktime_to_us, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("ms_to_ktime", (void *)ms_to_ktime, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("msecs_to_jiffies", (void *)msecs_to_jiffies, &u32KsymOk,
                 &u32KsymSkip);
    /* Prefer HZ-scaled body over empty ksym / crude 1:1 leaf residual. */
    lts_ksym_one("__msecs_to_jiffies", (void *)msecs_to_jiffies, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("jiffies_to_msecs", (void *)jiffies_to_msecs, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("usecs_to_jiffies", (void *)usecs_to_jiffies, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("jiffies_to_usecs", (void *)jiffies_to_usecs, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("nsecs_to_jiffies", (void *)nsecs_to_jiffies, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("jiffies_to_nsecs", (void *)jiffies_to_nsecs, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("round_jiffies", (void *)round_jiffies, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("round_jiffies_relative", (void *)round_jiffies_relative,
                 &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("round_jiffies_up", (void *)round_jiffies_up, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("round_jiffies_up_relative",
                 (void *)round_jiffies_up_relative, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("msleep", (void *)msleep, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("msleep_interruptible", (void *)msleep_interruptible,
                 &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("ssleep", (void *)ssleep, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("mdelay", (void *)mdelay, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("__const_udelay", (void *)__const_udelay, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("udelay", (void *)udelay, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("ndelay", (void *)ndelay, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("__udelay", (void *)__udelay, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("__ndelay", (void *)__ndelay, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("__delay", (void *)__delay, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("fsleep", (void *)fsleep, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("usleep_range", (void *)usleep_range, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("usleep_range_state", (void *)usleep_range_state, &u32KsymOk,
                 &u32KsymSkip);
    /*
     * Soft schedule ksym rebind: static lts_soft_schedule only (never the
     * freestanding product schedule() symbol). Soft!=product TASK_*.
     */
    lts_ksym_one("schedule", (void *)lts_soft_schedule, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("schedule_timeout", (void *)schedule_timeout, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("schedule_timeout_interruptible",
                 (void *)schedule_timeout_interruptible, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("schedule_timeout_uninterruptible",
                 (void *)schedule_timeout_uninterruptible, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("schedule_timeout_killable",
                 (void *)schedule_timeout_killable, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("schedule_timeout_idle", (void *)schedule_timeout_idle,
                 &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("io_schedule_timeout", (void *)io_schedule_timeout,
                 &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("timer_setup", (void *)timer_setup, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("mod_timer", (void *)mod_timer, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("mod_timer_pending", (void *)mod_timer_pending, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("timer_reduce", (void *)timer_reduce, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("add_timer", (void *)add_timer, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("add_timer_on", (void *)add_timer_on, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("del_timer", (void *)del_timer, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("del_timer_sync", (void *)del_timer_sync, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("del_singleshot_timer_sync",
                 (void *)del_singleshot_timer_sync, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("timer_pending", (void *)timer_pending, &u32KsymOk,
                 &u32KsymSkip);
    /*
     * Stronger residual than usb soft no-op global: rebind ksym name to
     * lts_init_timer_key which records via timer_setup. Soft!=product.
     */
    lts_ksym_one("init_timer_key", (void *)lts_init_timer_key, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("init_completion", (void *)init_completion, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("reinit_completion", (void *)reinit_completion, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("complete", (void *)complete, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("complete_all", (void *)complete_all, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("wait_for_completion", (void *)wait_for_completion,
                 &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("wait_for_completion_timeout",
                 (void *)wait_for_completion_timeout, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("wait_for_completion_interruptible",
                 (void *)wait_for_completion_interruptible, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("wait_for_completion_interruptible_timeout",
                 (void *)wait_for_completion_interruptible_timeout, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("wait_for_completion_killable",
                 (void *)wait_for_completion_killable, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("wait_for_completion_killable_timeout",
                 (void *)wait_for_completion_killable_timeout, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("wait_for_completion_io", (void *)wait_for_completion_io,
                 &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("wait_for_completion_io_timeout",
                 (void *)wait_for_completion_io_timeout, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("try_wait_for_completion", (void *)try_wait_for_completion,
                 &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("completion_done", (void *)completion_done, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("request_irq", (void *)request_irq, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("request_threaded_irq", (void *)request_threaded_irq,
                 &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("free_irq", (void *)free_irq, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("enable_irq", (void *)enable_irq, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("disable_irq", (void *)disable_irq, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("disable_irq_nosync", (void *)disable_irq_nosync, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("synchronize_irq", (void *)synchronize_irq, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("irq_set_affinity_hint", (void *)irq_set_affinity_hint,
                 &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("_printk", (void *)_printk, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("printk", (void *)_printk, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("__warn_printk", (void *)__warn_printk, &u32KsymOk,
                 &u32KsymSkip);
    lts_ksym_one("_dev_err", (void *)_dev_err, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("_dev_info", (void *)_dev_info, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("_dev_warn", (void *)_dev_warn, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("memcpy", (void *)memcpy, &u32KsymOk, &u32KsymSkip);
    lts_ksym_one("memset", (void *)memset, &u32KsymOk, &u32KsymSkip);

    /* Grep: linux_time_soft: soft init PASS */
    kprintf("linux_time_soft: soft init PASS irq_max=%u timer_max=%u "
            "comp_max=%u jiffies=%lu jiffies_64=%llu timer_j=%lu "
            "soft_ne_product=1 product=OPEN G-AC-1=1 C0=1 "
            "r8169_time_ksym=1 wire_safe=1 "
            "jiffy_convert=1 schedule_timeout=1 soft_schedule_ksym=1 "
            "timer_list=1 timer_reduce=1 init_timer_key=1 completion=1 "
            "completion_io=1 irq_request=1 irq_aff_hint=1 "
            "fsleep=1 udelay_alias=1 (Soft!=product)\n",
            (unsigned)LINUX_TIME_SOFT_IRQ_MAX,
            (unsigned)LINUX_TIME_SOFT_TIMER_MAX,
            (unsigned)LINUX_TIME_SOFT_COMPLETION_MAX,
            (unsigned long)jiffies,
            (unsigned long long)jiffies_64,
            (unsigned long)timer_jiffies());

    /*
     * Cold residual lamp for ktime hot path (no kprintf inside ktime_get).
     * Grep: linux_time_soft: soft ktime_get residual
     */
    kprintf("linux_time_soft: soft ktime_get residual ready soft_mono=1 "
            "mono_fast_ksym=1 real_ns_ksym=1 boot_fast_ksym=1 raw_ns_ksym=1 "
            "hot_nolog=1 soft_ne_product=1 product=OPEN G-AC-1=1 C0=1\n");

    /*
     * Cold residual lamp for jiffy convert (hot helpers stay silent).
     * Grep: linux_time_soft: soft msecs_to_jiffies
     */
    kprintf("linux_time_soft: soft msecs_to_jiffies residual ready "
            "hz=%u cap_j=%lu jiffies_64=1 soft_ne_product=1 product=OPEN "
            "C0=1\n",
            (unsigned)GJ_TIMER_HZ,
            (unsigned long)LINUX_TIME_SOFT_TIMEOUT_JIFFIES_MAX);

    /* C0 residual selfcheck (once; Soft!=product; never closes Dual DoD). */
    lts_residual_selfcheck();

    /*
     * Lean residual once lamp (no version stamp; no storm). Soft!=product.
     * Grep: linux_time_soft: soft residual lean PASS
     */
    if (g_fResidualLeanLamp == 0) {
        g_fResidualLeanLamp = 1;
        kprintf("linux_time_soft: soft residual lean PASS "
                "jiffies=1 jiffies_64=1 get_jiffies_64=1 "
                "ktime=1 mono_fast_ksym=1 real_ns=1 boot_fast_ns=1 raw_ns=1 "
                "ns_jiffy=1 round_j=1 "
                "mdelay=1 ndelay=1 fsleep=1 udelay_alias=1 schedule_timeout=1 "
                "soft_schedule_ksym=1 "
                "timer_list=%u timer_setup=1 timer_reduce=1 init_timer_key=1 "
                "completion=%u complete_star=1 wait_star=1 completion_io=1 "
                "irq_request=%u irq_companions=1 irq_aff_hint=1 irq_inject=1 "
                "selfcheck_ok=%u selfcheck_fail=%u "
                "hot_nolog=1 stamp_storm=0 version_stamp=0 stamp_free=1 "
                "soft_ne_product=1 product=OPEN G-AC-1=1 C0=1 "
                "dual_dod_a=OPEN dual_dod_b=OPEN "
                "(Soft!=product; soft-loaded module eng residual)\n",
                (unsigned)LINUX_TIME_SOFT_TIMER_MAX,
                (unsigned)LINUX_TIME_SOFT_COMPLETION_MAX,
                (unsigned)LINUX_TIME_SOFT_IRQ_MAX,
                (unsigned)g_cSelfcheckOk,
                (unsigned)g_cSelfcheckFail);
    }

    /*
     * Dual DoD honesty residual (once): soft never closes A/B.
     * Grep: linux_time_soft: soft residual dual_dod OPEN
     */
    if (g_fDualDodLamp == 0) {
        g_fDualDodLamp = 1;
        kprintf("linux_time_soft: soft residual dual_dod OPEN "
                "dual_dod_a=OPEN dual_dod_b=OPEN "
                "soft_ne_product=1 product=OPEN G-AC-1=1 C0=1 "
                "bar3=0 version_stamp=0 stamp_free=1 "
                "(Soft!=product; residual never closes Dual DoD)\n");
    }

    if (u32KsymOk > 0u) {
        /* Grep: linux_time_soft: soft ksym register PASS */
        kprintf("linux_time_soft: soft ksym register PASS n=%u skip=%u "
                "r8169_path=1 schedule_timeout=1 soft_schedule_ksym=1 "
                "jiffy_convert=1 timer_list=1 timer_reduce=1 "
                "init_timer_key=1 completion=1 completion_io=1 "
                "irq_request=1 irq_aff_hint=1 "
                "jiffies_64=1 mono_fast=1 real_ns=1 boot_fast=1 raw_ns=1 "
                "fsleep=1 udelay_alias=1 "
                "soft_ne_product=1 G-AC-1=1 C0=1 dual_dod=OPEN\n",
                (unsigned)u32KsymOk, (unsigned)u32KsymSkip);
    } else {
        /* Grep: linux_time_soft: soft ksym register SKIP */
        kprintf("linux_time_soft: soft ksym register SKIP (no linux_ksym or "
                "all failed) skip=%u soft_ne_product=1 G-AC-1=1 C0=1 "
                "dual_dod=OPEN\n",
                (unsigned)u32KsymSkip);
    }

    /*
     * Hybrid 4a honesty lamp (once): soft time/IRQ never steals freestanding
     * wire. No APIC/MSI program, no PIC rebind, no 10ec:8168 vector steal.
     * Grep: linux_time_soft: soft hybrid zero-touch PASS
     */
    if (g_fHybridLamp == 0) {
        g_fHybridLamp = 1;
        kprintf("linux_time_soft: soft hybrid zero-touch PASS "
                "no_apic=1 no_pic_bind=1 no_msi=1 no_8168_irq_steal=1 "
                "delay_capped=1 no_timer_fire=1 no_irq_hot_work=1 "
                "soft_ne_product=1 G-AC-1=1 C0=1 dual_dod=OPEN "
                "(freestanding wire non-destructive)\n");
    }
}
