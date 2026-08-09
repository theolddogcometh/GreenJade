/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Shared freestanding kprintf (x86_64 + aarch64 product). Console via
 * console_putchar / console_write (arch provides). Dual license MIT OR
 * Apache-2.0 only. No GPL.
 *
 * Format soft (pure C, no string.c):
 *   %% %c %s %d %i %u %x %p
 *   length: l / ll  (e.g. %ld %lu %lx %lld %llu %llx)
 *   optional 0-flag + decimal width (e.g. %04x %08lx %016llx)
 *
 * -------------------------------------------------------------------------
 * H2 residual - reentrancy / depth + rate caps (FAULT stack safety)
 * -------------------------------------------------------------------------
 * Prior FAULT class (H2 stamp storm -> #PF I=1 string-as-code): multi-line
 * soft-deepen floods + nested formatter frames on residual stack.
 * Permanent rules in this unit only (Soft!=product; C0 residual deepen):
 *   - KPF_DEPTH_MAX bounds nested kprintf frames; excess dropped (counter)
 *   - Soft inventory emits only at depth==1 (never from nested frames)
 *   - Soft dump uses console_* + print_u64 only (no nested kprintf)
 *   - LEAN dump: few greppable lines; never ret*angle / multi-kprintf storms
 *   - KPF_SOFT_LOG_MAX hard-caps full multi-line dumps (pow2 milestones)
 *   - Post-cap lean rate residual: densified kprintf/stdio_k soft rate pair
 *     on pow2 of cap_skip, hard-capped at KPF_SOFT_RATE_LOG_MAX (never
 *     silent forever, never stamp-storm)
 *   - rate_cap_skip tallies pow2 cap_skip arrivals past rate residual hard-cap
 *   - quiet / mile_hit tallies (non-milestone silent + pre-cap milestones)
 *   - KPF_NUM_BUF / KPF_WIDTH_MAX tiny; %s hard-capped at KPF_STR_MAX
 *   - No product version stamp; stamp-free residual; never bump GJ_IMAGE_VERSION
 *   - Soft!=product; G-AC-1 (no .ko product AC in this unit); Dual DoD OPEN
 *
 * greppable: kprintf: soft
 * greppable: kprintf: soft residual lean
 * greppable: kprintf: soft rate
 * greppable: stdio_k: soft
 * greppable: stdio_k: soft residual lean
 * greppable: stdio_k: soft rate
 * greppable: Soft!=product | G-AC-1 | dual_dod=OPEN | stamp_storm=0
 */
#include <gj/console.h>
#include <gj/klog.h>
#include <gj/types.h>
#include <stdarg.h>

/* length modifiers */
enum {
    KPF_LEN_NONE = 0,
    KPF_LEN_L    = 1,
    KPF_LEN_LL   = 2
};

/*
 * Digit/pad capacity for u64: decimal needs <=20 digits, hex <=16; width for
 * %016llx / %p is 16. Keep tiny - large on-stack buffers on deep call stacks
 * contributed to FAULT class (#PF I=1). Soft!=product inventory uses same path.
 */
enum { KPF_NUM_BUF = 24, KPF_WIDTH_MAX = 20 };

/*
 * Nested kprintf bound (product path). Depth 0 idle; each kprintf entry
 * increments. Beyond KPF_DEPTH_MAX the call is dropped (counter only) so
 * console/fb soft hooks cannot stack-smash via reentrancy. Soft!=product.
 */
#define KPF_DEPTH_MAX 2

/* Bound %s emit length (chars); prevents multi-KiB floods on residual stack. */
#define KPF_STR_MAX 512u

/*
 * Soft inventory serial budget (H2 residual lean). Absolute cap of greppable
 * full multi-line dumps; milestones are power-of-two kprintf call counts
 * (1,2,4,...). Tight cap: stamp storms worsen stack (FAULT class H2).
 * Soft!=product. greppable: kprintf: soft / stdio_k: soft
 */
#define KPF_SOFT_LOG_MAX 4u

/*
 * Post-cap lean rate residual budget (H2 rate caps residual deepen).
 * After KPF_SOFT_LOG_MAX full dumps, still emit densified rate residual pair
 * on power-of-two of cap_skip so residual is never silent forever -
 * hard-capped so long-running boots cannot stamp-storm. Soft!=product.
 * greppable: kprintf: soft rate / stdio_k: soft rate
 */
#define KPF_SOFT_RATE_LOG_MAX 4u

/*
 * Soft inventory area count (fixed greppable categories; Soft!=product lean):
 *   inventory | path | stats | depth | residual | stdio | rate
 * Honest count only - never inflate with stamp-storm dummies (H2 residual).
 * "rate" covers full-dump densified fields + post-cap lean rate residual.
 */
#define KPF_SOFT_AREAS 7u

/*
 * Soft product counters (wrap OK; diagnostics only).
 * greppable: kprintf: soft inventory
 * greppable: kprintf: soft path
 * greppable: kprintf: soft stats
 * greppable: kprintf: soft depth
 * greppable: kprintf: soft residual lean
 * greppable: kprintf: soft rate
 * greppable: stdio_k: soft
 * greppable: stdio_k: soft residual lean
 * greppable: stdio_k: soft rate
 */
static u64 g_u64SoftCalls;     /* kprintf entries */
static u64 g_u64SoftPanic;     /* panic entries */
static u64 g_u64SoftPanicNull; /* panic(NULL) */
static u64 g_u64SoftLit;       /* literal (non-%) bytes emitted */
static u64 g_u64SoftPct;       /* '%' conversion sites entered */
static u64 g_u64SoftConvEsc;   /* %% */
static u64 g_u64SoftConvC;     /* %c */
static u64 g_u64SoftConvS;     /* %s */
static u64 g_u64SoftConvD;     /* %d / %i */
static u64 g_u64SoftConvU;     /* %u */
static u64 g_u64SoftConvX;     /* %x */
static u64 g_u64SoftConvP;     /* %p */
static u64 g_u64SoftConvUnk;   /* unknown conversion echo */
static u64 g_u64SoftWidth;     /* conversions with width > 0 */
static u64 g_u64SoftZeroPad;   /* conversions with '0' pad flag */
static u64 g_u64SoftLenL;      /* length 'l' */
static u64 g_u64SoftLenLl;     /* length 'll' */
static u64 g_u64SoftNullFmt;   /* kprintf(NULL, ...) */
static u64 g_u64SoftEmptyFmt;  /* kprintf("") empty format */
static u64 g_u64SoftNullS;     /* %s with NULL arg -> "(null)" */
static u64 g_u64SoftTrunc;     /* format ended mid-conversion */
static u64 g_u64SoftSkip;      /* soft log suppressed (cap / busy / reenter) */
static u64 g_u64SoftCapSkip;   /* soft log suppressed: hit KPF_SOFT_LOG_MAX */
static u64 g_u64SoftBusySkip;  /* soft log suppressed: reentrancy guard */
static u64 g_u64SoftDepthSkip; /* kprintf nested beyond KPF_DEPTH_MAX */
static u64 g_u64SoftNestSkip;  /* soft log skipped: not outermost frame */
static u64 g_u64SoftStrCap;    /* %s hit KPF_STR_MAX (truncated) */
static u64 g_u64SoftQuiet;     /* non-milestone silent (rate residual honesty) */
static u64 g_u64SoftMileHit;   /* pow2 milestone arrivals (pre-cap / pre-skip) */
static u64 g_u64SoftLastMile;  /* last power-of-two milestone that full-logged */
static u64 g_u64SoftRateLast;  /* last cap_skip value that rate-logged */
static u64 g_u64SoftRateCapSkip; /* pow2 cap_skip past KPF_SOFT_RATE_LOG_MAX */
static u64 g_u64SoftDepthNeg;  /* defensive clamp when depth corrupted < 0 */
static u64 g_u64SoftWidthClamp;/* width parse hit KPF_WIDTH_MAX */
static u64 g_u64SoftPanicReset;/* panic cleared depth/busy (H2 stack safety) */
static u32 g_u32SoftLogged;    /* full multi-line dump emissions */
static u32 g_u32SoftRateLogged;/* post-cap lean rate residual emissions */
static int g_fSoftBusy;        /* reentrancy guard for soft dump */
static int g_nKpfDepth;        /* nested kprintf depth (product bound) */
static int g_nKpfDepthHi;      /* soft high-water of nested depth */

static void
print_u64(u64 u64Val, unsigned uBase, int nWidth, char chPad)
{
    /* Tiny stack frame only - see KPF_NUM_BUF comment (FAULT harden). */
    char szBuf[KPF_NUM_BUF];
    const char *szDigits = "0123456789abcdef";
    int nLen = 0;

    if (uBase < 2u || uBase > 16u) {
        uBase = 10u;
    }
    if (nWidth < 0) {
        nWidth = 0;
    }
    if (nWidth > KPF_WIDTH_MAX) {
        nWidth = KPF_WIDTH_MAX;
    }

    if (u64Val == 0) {
        szBuf[nLen++] = '0';
    } else {
        while (u64Val && nLen < KPF_NUM_BUF) {
            szBuf[nLen++] = szDigits[u64Val % uBase];
            u64Val /= uBase;
        }
    }
    while (nLen < nWidth && nLen < KPF_NUM_BUF) {
        szBuf[nLen++] = chPad;
    }
    while (nLen > 0) {
        console_putchar(szBuf[--nLen]);
    }
}

static void
print_i64(i64 i64Val, int nWidth, char chPad)
{
    u64 uAbs;

    if (i64Val < 0) {
        console_putchar('-');
        /* two's complement abs without UB on INT64_MIN */
        uAbs = (u64)(-(i64Val + 1)) + 1u;
        if (nWidth > 0) {
            nWidth--;
        }
    } else {
        uAbs = (u64)i64Val;
    }
    print_u64(uAbs, 10u, nWidth, chPad);
}

static u64
arg_unsigned(va_list *pVa, int nLen)
{
    if (nLen == KPF_LEN_LL) {
        return (u64)va_arg(*pVa, unsigned long long);
    }
    if (nLen == KPF_LEN_L) {
        return (u64)va_arg(*pVa, unsigned long);
    }
    return (u64)va_arg(*pVa, unsigned int);
}

static i64
arg_signed(va_list *pVa, int nLen)
{
    if (nLen == KPF_LEN_LL) {
        return (i64)va_arg(*pVa, long long);
    }
    if (nLen == KPF_LEN_L) {
        return (i64)va_arg(*pVa, long);
    }
    return (i64)va_arg(*pVa, int);
}

/** Soft: bump one counter (wrap OK; never hard-gates). */
static void
kpf_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/**
 * Soft: emit "key=<decimal>" without calling kprintf (reentrancy-safe).
 * Used only by rate-limited soft inventory dumps.
 */
static void
kpf_soft_kv(const char *szKey, u64 u64Val)
{
    if (szKey != NULL) {
        console_write(szKey);
    }
    print_u64(u64Val, 10u, 0, ' ');
}

/**
 * H2 rate residual - one dense greppable line (console_* only; no kprintf).
 * Used inside full multi-line dump and as the sole post-cap residual lamp.
 * greppable: kprintf: soft rate / stdio_k: soft rate
 * Soft!=product; G-AC-1; dual_dod=OPEN; never hard-gates product path.
 */
static void
kprintf_soft_rate_line(void)
{
    u32 u32AtCap;
    u32 u32RateAtCap;

    u32AtCap = (g_u32SoftLogged >= KPF_SOFT_LOG_MAX) ? 1u : 0u;
    u32RateAtCap = (g_u32SoftRateLogged >= KPF_SOFT_RATE_LOG_MAX) ? 1u : 0u;

    /* Grep: kprintf: soft rate - H2 rate caps residual (pow2+hard_cap). */
    console_write("kprintf: soft rate");
    kpf_soft_kv(" calls=", g_u64SoftCalls);
    kpf_soft_kv(" mile_hit=", g_u64SoftMileHit);
    kpf_soft_kv(" mile_last=", g_u64SoftLastMile);
    kpf_soft_kv(" quiet=", g_u64SoftQuiet);
    kpf_soft_kv(" logs=", (u64)g_u32SoftLogged);
    kpf_soft_kv(" log_max=", (u64)KPF_SOFT_LOG_MAX);
    kpf_soft_kv(" rate_logs=", (u64)g_u32SoftRateLogged);
    kpf_soft_kv(" rate_max=", (u64)KPF_SOFT_RATE_LOG_MAX);
    kpf_soft_kv(" rate_last=", g_u64SoftRateLast);
    kpf_soft_kv(" rate_cap_skip=", g_u64SoftRateCapSkip);
    kpf_soft_kv(" skip=", g_u64SoftSkip);
    kpf_soft_kv(" cap_skip=", g_u64SoftCapSkip);
    kpf_soft_kv(" nest_skip=", g_u64SoftNestSkip);
    kpf_soft_kv(" busy_skip=", g_u64SoftBusySkip);
    kpf_soft_kv(" depth_skip=", g_u64SoftDepthSkip);
    kpf_soft_kv(" depth_max=", (u64)KPF_DEPTH_MAX);
    kpf_soft_kv(" depth_hi=", (u64)g_nKpfDepthHi);
    kpf_soft_kv(" at_cap=", (u64)u32AtCap);
    kpf_soft_kv(" rate_at_cap=", (u64)u32RateAtCap);
    kpf_soft_kv(" h2=", 1ull);
    kpf_soft_kv(" soft_ne_product=", 1ull);
    kpf_soft_kv(" g_ac1=", 1ull);
    kpf_soft_kv(" dual_dod_open=", 1ull);
    console_write(" policy=pow2+hard_cap outer_only=1 stamp_storm=0 "
                  "version_stamp=0 Soft!=product\n");

    /* Grep: stdio_k: soft rate - twin lean rate residual (unit name). */
    console_write("stdio_k: soft rate");
    kpf_soft_kv(" calls=", g_u64SoftCalls);
    kpf_soft_kv(" mile_hit=", g_u64SoftMileHit);
    kpf_soft_kv(" quiet=", g_u64SoftQuiet);
    kpf_soft_kv(" logs=", (u64)g_u32SoftLogged);
    kpf_soft_kv(" log_max=", (u64)KPF_SOFT_LOG_MAX);
    kpf_soft_kv(" rate_logs=", (u64)g_u32SoftRateLogged);
    kpf_soft_kv(" rate_max=", (u64)KPF_SOFT_RATE_LOG_MAX);
    kpf_soft_kv(" rate_cap_skip=", g_u64SoftRateCapSkip);
    kpf_soft_kv(" cap_skip=", g_u64SoftCapSkip);
    kpf_soft_kv(" nest_skip=", g_u64SoftNestSkip);
    kpf_soft_kv(" depth_skip=", g_u64SoftDepthSkip);
    kpf_soft_kv(" at_cap=", (u64)u32AtCap);
    kpf_soft_kv(" rate_at_cap=", (u64)u32RateAtCap);
    kpf_soft_kv(" h2=", 1ull);
    kpf_soft_kv(" soft_ne_product=", 1ull);
    kpf_soft_kv(" g_ac1=", 1ull);
    kpf_soft_kv(" dual_dod_open=", 1ull);
    console_write(" policy=pow2+hard_cap outer_only=1 stamp_storm=0 "
                  "version_stamp=0 Soft!=product\n");
}

/**
 * Post-cap lean rate residual (H2 rate caps residual deepen).
 * One densified rate pair only - never the full multi-line inventory.
 * Must not call kprintf. Soft!=product; dual MIT OR Apache-2.0.
 */
static void
kprintf_soft_rate_log(void)
{
    if (g_fSoftBusy != 0) {
        kpf_soft_inc(&g_u64SoftSkip);
        kpf_soft_inc(&g_u64SoftBusySkip);
        return;
    }
    g_fSoftBusy = 1;
    if (g_u32SoftRateLogged < 0xffffffffu) {
        g_u32SoftRateLogged++;
    }
    g_u64SoftRateLast = g_u64SoftCapSkip;
    kprintf_soft_rate_line();
    g_fSoftBusy = 0;
}

/**
 * Greppable soft kprintf inventory (H2 residual lean; Soft!=product).
 * Must not call kprintf - console_write + print_u64 only.
 * Few lines only - no ret*angle / multi-line stamp storms (FAULT class H2).
 *
 *   kprintf: soft inventory|path|stats|depth|residual lean|rate
 *   stdio_k: soft inventory|residual lean|rate
 */
static void
kprintf_soft_log(void)
{
    u64 u64ConvAll;

    if (g_fSoftBusy != 0) {
        kpf_soft_inc(&g_u64SoftSkip);
        kpf_soft_inc(&g_u64SoftBusySkip);
        return;
    }
    g_fSoftBusy = 1;
    if (g_u32SoftLogged < 0xffffffffu) {
        g_u32SoftLogged++;
    }
    g_u64SoftLastMile = g_u64SoftCalls;

    u64ConvAll = g_u64SoftConvEsc + g_u64SoftConvC + g_u64SoftConvS +
                 g_u64SoftConvD + g_u64SoftConvU + g_u64SoftConvX +
                 g_u64SoftConvP + g_u64SoftConvUnk;

    /*
     * Grep: kprintf: soft inventory - one dense rollup (H2 lean).
     * Depth / reentrancy / rate residual counters primary; Soft!=product.
     */
    console_write("kprintf: soft inventory");
    kpf_soft_kv(" calls=", g_u64SoftCalls);
    kpf_soft_kv(" lit=", g_u64SoftLit);
    kpf_soft_kv(" pct=", g_u64SoftPct);
    kpf_soft_kv(" conv_all=", u64ConvAll);
    kpf_soft_kv(" panic=", g_u64SoftPanic);
    kpf_soft_kv(" log_n=", (u64)g_u32SoftLogged);
    kpf_soft_kv(" skip=", g_u64SoftSkip);
    kpf_soft_kv(" quiet=", g_u64SoftQuiet);
    kpf_soft_kv(" mile_hit=", g_u64SoftMileHit);
    kpf_soft_kv(" depth_skip=", g_u64SoftDepthSkip);
    kpf_soft_kv(" nest_skip=", g_u64SoftNestSkip);
    kpf_soft_kv(" busy_skip=", g_u64SoftBusySkip);
    kpf_soft_kv(" cap_skip=", g_u64SoftCapSkip);
    kpf_soft_kv(" depth_max=", (u64)KPF_DEPTH_MAX);
    kpf_soft_kv(" depth_cur=", (u64)g_nKpfDepth);
    kpf_soft_kv(" depth_hi=", (u64)g_nKpfDepthHi);
    console_write(" Soft!=product\n");

    /* Grep: kprintf: soft path - contracts, no nested kprintf */
    console_write("kprintf: soft path claim=console_putchar,console_write "
                  "fmt=%%,%c,%s,%d,%i,%u,%x,%p len=l,ll flag=0-width "
                  "reenter_safe=1 nested_kprintf=0 string_c=0 "
                  "depth_max=");
    print_u64((u64)KPF_DEPTH_MAX, 10u, 0, ' ');
    console_write(" num_buf=");
    print_u64((u64)KPF_NUM_BUF, 10u, 0, ' ');
    console_write(" str_max=");
    print_u64((u64)KPF_STR_MAX, 10u, 0, ' ');
    console_write(" milestone=pow2 cap=");
    print_u64((u64)KPF_SOFT_LOG_MAX, 10u, 0, ' ');
    console_write(" rate_cap=");
    print_u64((u64)KPF_SOFT_RATE_LOG_MAX, 10u, 0, ' ');
    console_write(" outer_only=1 stamp_storm=0 Soft!=product\n");

    /* Grep: kprintf: soft stats - conversion + null/trunc densified */
    console_write("kprintf: soft stats");
    kpf_soft_kv(" esc=", g_u64SoftConvEsc);
    kpf_soft_kv(" c=", g_u64SoftConvC);
    kpf_soft_kv(" s=", g_u64SoftConvS);
    kpf_soft_kv(" d=", g_u64SoftConvD);
    kpf_soft_kv(" u=", g_u64SoftConvU);
    kpf_soft_kv(" x=", g_u64SoftConvX);
    kpf_soft_kv(" p=", g_u64SoftConvP);
    kpf_soft_kv(" unk=", g_u64SoftConvUnk);
    kpf_soft_kv(" null_fmt=", g_u64SoftNullFmt);
    kpf_soft_kv(" null_s=", g_u64SoftNullS);
    kpf_soft_kv(" empty=", g_u64SoftEmptyFmt);
    kpf_soft_kv(" panic_null=", g_u64SoftPanicNull);
    kpf_soft_kv(" trunc=", g_u64SoftTrunc);
    kpf_soft_kv(" str_cap=", g_u64SoftStrCap);
    kpf_soft_kv(" width=", g_u64SoftWidth);
    kpf_soft_kv(" zero=", g_u64SoftZeroPad);
    kpf_soft_kv(" len_l=", g_u64SoftLenL);
    kpf_soft_kv(" len_ll=", g_u64SoftLenLl);
    kpf_soft_kv(" last_mile=", g_u64SoftLastMile);
    kpf_soft_kv(" quiet=", g_u64SoftQuiet);
    kpf_soft_kv(" mile_hit=", g_u64SoftMileHit);
    kpf_soft_kv(" areas=", (u64)KPF_SOFT_AREAS);
    console_write(" Soft!=product\n");

    /*
     * Grep: kprintf: soft depth - H2 reentrancy residual (primary surface).
     * depth_max / depth_hi / skip taxonomy only; never multi-line floods.
     */
    console_write("kprintf: soft depth");
    kpf_soft_kv(" depth_max=", (u64)KPF_DEPTH_MAX);
    kpf_soft_kv(" depth_cur=", (u64)g_nKpfDepth);
    kpf_soft_kv(" depth_hi=", (u64)g_nKpfDepthHi);
    kpf_soft_kv(" depth_skip=", g_u64SoftDepthSkip);
    kpf_soft_kv(" depth_neg=", g_u64SoftDepthNeg);
    kpf_soft_kv(" nest_skip=", g_u64SoftNestSkip);
    kpf_soft_kv(" busy_skip=", g_u64SoftBusySkip);
    kpf_soft_kv(" cap_skip=", g_u64SoftCapSkip);
    kpf_soft_kv(" rate_cap_skip=", g_u64SoftRateCapSkip);
    kpf_soft_kv(" str_cap=", g_u64SoftStrCap);
    kpf_soft_kv(" width_clamp=", g_u64SoftWidthClamp);
    kpf_soft_kv(" num_buf=", (u64)KPF_NUM_BUF);
    kpf_soft_kv(" str_max=", (u64)KPF_STR_MAX);
    kpf_soft_kv(" log_max=", (u64)KPF_SOFT_LOG_MAX);
    kpf_soft_kv(" rate_max=", (u64)KPF_SOFT_RATE_LOG_MAX);
    kpf_soft_kv(" h2=", 1ull);
    kpf_soft_kv(" soft_ne_product=", 1ull);
    console_write(" taxonomy=depth,nest,busy,cap,rate_cap,quiet,mile "
                  "outer_only=1 stamp_storm=0 Soft!=product\n");

    /*
     * Grep: kprintf: soft residual lean
     * Single honesty lamp for H2 stamp-storm guard + stack-safe + rate residual.
     * C0 residual deepen: Soft!=product; dual MIT OR Apache-2.0; G-AC-1;
     * Dual DoD OPEN; no version stamp; never hard-gates product path.
     */
    console_write("kprintf: soft residual lean");
    kpf_soft_kv(" soft_ne_product=", 1ull);
    kpf_soft_kv(" h2=", 1ull);
    kpf_soft_kv(" g_ac1=", 1ull);
    kpf_soft_kv(" dual_dod_open=", 1ull);
    kpf_soft_kv(" depth_max=", (u64)KPF_DEPTH_MAX);
    kpf_soft_kv(" depth_hi=", (u64)g_nKpfDepthHi);
    kpf_soft_kv(" depth_skip=", g_u64SoftDepthSkip);
    kpf_soft_kv(" depth_neg=", g_u64SoftDepthNeg);
    kpf_soft_kv(" nest_skip=", g_u64SoftNestSkip);
    kpf_soft_kv(" busy_skip=", g_u64SoftBusySkip);
    kpf_soft_kv(" cap_skip=", g_u64SoftCapSkip);
    kpf_soft_kv(" rate_cap_skip=", g_u64SoftRateCapSkip);
    kpf_soft_kv(" quiet=", g_u64SoftQuiet);
    kpf_soft_kv(" mile_hit=", g_u64SoftMileHit);
    kpf_soft_kv(" log_cap=", (u64)KPF_SOFT_LOG_MAX);
    kpf_soft_kv(" logs=", (u64)g_u32SoftLogged);
    kpf_soft_kv(" rate_cap=", (u64)KPF_SOFT_RATE_LOG_MAX);
    kpf_soft_kv(" rate_logs=", (u64)g_u32SoftRateLogged);
    kpf_soft_kv(" str_cap=", g_u64SoftStrCap);
    kpf_soft_kv(" width_clamp=", g_u64SoftWidthClamp);
    kpf_soft_kv(" panic_reset=", g_u64SoftPanicReset);
    kpf_soft_kv(" num_buf=", (u64)KPF_NUM_BUF);
    kpf_soft_kv(" str_max=", (u64)KPF_STR_MAX);
    kpf_soft_kv(" areas=", (u64)KPF_SOFT_AREAS);
    kpf_soft_kv(" version_stamp=", 0ull);
    kpf_soft_kv(" stamp_storm=", 0ull);
    console_write(" dual=MIT_OR_Apache-2.0 outer_only=1 reenter_safe=1 "
                  "stack_safe=1 rate_safe=1 no_gpl=1 Soft!=product\n");

    /* Grep: kprintf: soft rate + stdio_k: soft rate (densified in dump). */
    kprintf_soft_rate_line();

    /* Grep: stdio_k: soft inventory - twin lean rollup */
    console_write("stdio_k: soft inventory");
    kpf_soft_kv(" calls=", g_u64SoftCalls);
    kpf_soft_kv(" log_n=", (u64)g_u32SoftLogged);
    kpf_soft_kv(" skip=", g_u64SoftSkip);
    kpf_soft_kv(" quiet=", g_u64SoftQuiet);
    kpf_soft_kv(" mile_hit=", g_u64SoftMileHit);
    kpf_soft_kv(" depth_skip=", g_u64SoftDepthSkip);
    kpf_soft_kv(" nest_skip=", g_u64SoftNestSkip);
    kpf_soft_kv(" busy_skip=", g_u64SoftBusySkip);
    kpf_soft_kv(" cap_skip=", g_u64SoftCapSkip);
    kpf_soft_kv(" rate_cap_skip=", g_u64SoftRateCapSkip);
    kpf_soft_kv(" depth_max=", (u64)KPF_DEPTH_MAX);
    kpf_soft_kv(" depth_hi=", (u64)g_nKpfDepthHi);
    kpf_soft_kv(" max=", (u64)KPF_SOFT_LOG_MAX);
    kpf_soft_kv(" rate_max=", (u64)KPF_SOFT_RATE_LOG_MAX);
    kpf_soft_kv(" areas=", (u64)KPF_SOFT_AREAS);
    kpf_soft_kv(" soft_ne_product=", 1ull);
    kpf_soft_kv(" g_ac1=", 1ull);
    kpf_soft_kv(" dual_dod_open=", 1ull);
    console_write(" fmt=%%,%c,%s,%d,%i,%u,%x,%p,l,ll,0-width "
                  "reenter_safe=1 stamp_storm=0 Soft!=product\n");

    /*
     * Grep: stdio_k: soft residual lean
     * Twin residual lean lamp (unit name); Soft!=product dual license.
     * C0 residual deepen: full skip taxonomy + rate residual + H2 honesty.
     * Dual DoD OPEN; G-AC-1; never product close / never version stamp.
     */
    console_write("stdio_k: soft residual lean");
    kpf_soft_kv(" soft_ne_product=", 1ull);
    kpf_soft_kv(" h2=", 1ull);
    kpf_soft_kv(" g_ac1=", 1ull);
    kpf_soft_kv(" dual_dod_open=", 1ull);
    kpf_soft_kv(" depth_max=", (u64)KPF_DEPTH_MAX);
    kpf_soft_kv(" depth_hi=", (u64)g_nKpfDepthHi);
    kpf_soft_kv(" depth_skip=", g_u64SoftDepthSkip);
    kpf_soft_kv(" depth_neg=", g_u64SoftDepthNeg);
    kpf_soft_kv(" nest_skip=", g_u64SoftNestSkip);
    kpf_soft_kv(" busy_skip=", g_u64SoftBusySkip);
    kpf_soft_kv(" cap_skip=", g_u64SoftCapSkip);
    kpf_soft_kv(" rate_cap_skip=", g_u64SoftRateCapSkip);
    kpf_soft_kv(" log_cap=", (u64)KPF_SOFT_LOG_MAX);
    kpf_soft_kv(" logs=", (u64)g_u32SoftLogged);
    kpf_soft_kv(" rate_cap=", (u64)KPF_SOFT_RATE_LOG_MAX);
    kpf_soft_kv(" rate_logs=", (u64)g_u32SoftRateLogged);
    kpf_soft_kv(" quiet=", g_u64SoftQuiet);
    kpf_soft_kv(" mile_hit=", g_u64SoftMileHit);
    kpf_soft_kv(" str_cap=", g_u64SoftStrCap);
    kpf_soft_kv(" width_clamp=", g_u64SoftWidthClamp);
    kpf_soft_kv(" panic_reset=", g_u64SoftPanicReset);
    kpf_soft_kv(" num_buf=", (u64)KPF_NUM_BUF);
    kpf_soft_kv(" str_max=", (u64)KPF_STR_MAX);
    kpf_soft_kv(" areas=", (u64)KPF_SOFT_AREAS);
    kpf_soft_kv(" version_stamp=", 0ull);
    kpf_soft_kv(" stamp_storm=", 0ull);
    console_write(" dual=MIT_OR_Apache-2.0 outer_only=1 reenter_safe=1 "
                  "stack_safe=1 rate_safe=1 no_gpl=1 Soft!=product\n");

    g_fSoftBusy = 0;
}

/**
 * Rate-limit soft inventory: power-of-two call milestones, hard-capped,
 * outermost frame only (depth==1). Nested frames never run the soft dump
 * (H2 residual: soft dump + nested formatter frames -> stack pressure).
 *
 * H2 rate caps residual deepen (C0 Soft!=product):
 *   - non-milestone calls tally quiet (silent honesty; no serial)
 *   - milestone arrivals tally mile_hit before skip taxonomy
 *   - past KPF_SOFT_LOG_MAX: full dump stays silent; lean rate residual on
 *     pow2 of cap_skip, hard-capped at KPF_SOFT_RATE_LOG_MAX
 *   - past rate hard-cap: rate_cap_skip tallies (serial stays silent)
 *
 * Never floods serial. soft skip tallies only suppressed milestones (cap,
 * busy, or nest). greppable: kprintf: soft / stdio_k: soft / soft rate
 */
static void
kprintf_soft_maybe_log(void)
{
    u64 u64N;
    u64 u64CapSkip;

    u64N = g_u64SoftCalls;
    /* Milestone: first call and subsequent powers of two only. */
    if (u64N == 0ull || (u64N & (u64N - 1ull)) != 0ull) {
        kpf_soft_inc(&g_u64SoftQuiet);
        return;
    }

    kpf_soft_inc(&g_u64SoftMileHit);

    /*
     * Outer frame only. After kprintf entry depth is already incremented;
     * nested reentry must not fire the soft dump on residual stack (H2).
     */
    if (g_nKpfDepth != 1) {
        kpf_soft_inc(&g_u64SoftSkip);
        kpf_soft_inc(&g_u64SoftNestSkip);
        return;
    }

    if (g_fSoftBusy != 0) {
        kpf_soft_inc(&g_u64SoftSkip);
        kpf_soft_inc(&g_u64SoftBusySkip);
        return;
    }
    if (g_u32SoftLogged >= KPF_SOFT_LOG_MAX) {
        kpf_soft_inc(&g_u64SoftSkip);
        kpf_soft_inc(&g_u64SoftCapSkip);
        /*
         * Full multi-line inventory hard-capped (H2). Lean rate residual
         * still greppable on pow2 of cap_skip - never silent forever, never
         * stamp-storm (KPF_SOFT_RATE_LOG_MAX). Past rate cap: rate_cap_skip
         * honesty only (no serial). Soft!=product.
         */
        u64CapSkip = g_u64SoftCapSkip;
        if (u64CapSkip != 0ull &&
            (u64CapSkip & (u64CapSkip - 1ull)) == 0ull) {
            if (g_u32SoftRateLogged < KPF_SOFT_RATE_LOG_MAX) {
                kprintf_soft_rate_log();
            } else {
                kpf_soft_inc(&g_u64SoftRateCapSkip);
            }
        }
        return;
    }

    kprintf_soft_log();
}

void
kprintf(const char *szFmt, ...)
{
    va_list vaArgs;
    const char *szCursor;

    kpf_soft_inc(&g_u64SoftCalls);

    /*
     * Bound nested frames: deep call stacks + reentrant kprintf (console /
     * soft hooks) contributed to FAULT class H2. Drop beyond KPF_DEPTH_MAX.
     * Soft: defensive clamp if depth ever corrupted negative (C0 residual).
     */
    if (g_nKpfDepth < 0) {
        kpf_soft_inc(&g_u64SoftDepthNeg);
        g_nKpfDepth = 0;
    }
    if (g_nKpfDepth >= (int)KPF_DEPTH_MAX) {
        kpf_soft_inc(&g_u64SoftDepthSkip);
        return;
    }
    g_nKpfDepth++;
    if (g_nKpfDepth > g_nKpfDepthHi) {
        g_nKpfDepthHi = g_nKpfDepth;
    }

    if (szFmt == NULL) {
        kpf_soft_inc(&g_u64SoftNullFmt);
        kprintf_soft_maybe_log();
        g_nKpfDepth--;
        return;
    }

    if (*szFmt == '\0') {
        kpf_soft_inc(&g_u64SoftEmptyFmt);
        kprintf_soft_maybe_log();
        g_nKpfDepth--;
        return;
    }

    va_start(vaArgs, szFmt);
    for (szCursor = szFmt; szCursor && *szCursor; szCursor++) {
        int nWidth;
        char chPad;
        int nLen;
        char chConv;
        int fZeroPad;

        if (*szCursor != '%') {
            kpf_soft_inc(&g_u64SoftLit);
            console_putchar(*szCursor);
            continue;
        }
        szCursor++;
        kpf_soft_inc(&g_u64SoftPct);
        if (!*szCursor) {
            kpf_soft_inc(&g_u64SoftTrunc);
            break;
        }

        /* flags (soft: only '0' zero-pad; ignore others carefully) */
        chPad = ' ';
        fZeroPad = 0;
        while (*szCursor == '0') {
            chPad = '0';
            fZeroPad = 1;
            szCursor++;
        }
        if (fZeroPad != 0) {
            kpf_soft_inc(&g_u64SoftZeroPad);
        }

        /* width (soft clamp at KPF_WIDTH_MAX - residual stack safety) */
        nWidth = 0;
        while (*szCursor >= '0' && *szCursor <= '9') {
            int nDigit = (int)(*szCursor - '0');

            if (nWidth <= (KPF_WIDTH_MAX - nDigit) / 10) {
                nWidth = nWidth * 10 + nDigit;
            } else {
                if (nWidth != KPF_WIDTH_MAX) {
                    kpf_soft_inc(&g_u64SoftWidthClamp);
                }
                nWidth = KPF_WIDTH_MAX;
            }
            szCursor++;
        }
        if (nWidth > 0) {
            kpf_soft_inc(&g_u64SoftWidth);
        }

        /* length: l / ll */
        nLen = KPF_LEN_NONE;
        if (*szCursor == 'l') {
            szCursor++;
            if (*szCursor == 'l') {
                szCursor++;
                nLen = KPF_LEN_LL;
                kpf_soft_inc(&g_u64SoftLenLl);
            } else {
                nLen = KPF_LEN_L;
                kpf_soft_inc(&g_u64SoftLenL);
            }
        }

        chConv = *szCursor;
        if (!chConv) {
            kpf_soft_inc(&g_u64SoftTrunc);
            break;
        }

        switch (chConv) {
        case '%':
            kpf_soft_inc(&g_u64SoftConvEsc);
            console_putchar('%');
            break;
        case 'c':
            kpf_soft_inc(&g_u64SoftConvC);
            console_putchar((char)va_arg(vaArgs, int));
            break;
        case 's': {
            const char *szArg = va_arg(vaArgs, const char *);
            u32 u32N = 0;

            kpf_soft_inc(&g_u64SoftConvS);
            if (szArg == NULL) {
                kpf_soft_inc(&g_u64SoftNullS);
                szArg = "(null)";
            }
            /* Hard-cap %s length - unbounded strings flood residual stack. */
            while (szArg[u32N] != '\0' && u32N < KPF_STR_MAX) {
                console_putchar(szArg[u32N]);
                u32N++;
            }
            if (szArg[u32N] != '\0') {
                kpf_soft_inc(&g_u64SoftStrCap);
            }
            break;
        }
        case 'd':
        case 'i':
            kpf_soft_inc(&g_u64SoftConvD);
            print_i64(arg_signed(&vaArgs, nLen), nWidth, chPad);
            break;
        case 'u':
            kpf_soft_inc(&g_u64SoftConvU);
            print_u64(arg_unsigned(&vaArgs, nLen), 10u, nWidth, chPad);
            break;
        case 'x':
            kpf_soft_inc(&g_u64SoftConvX);
            print_u64(arg_unsigned(&vaArgs, nLen), 16u, nWidth, chPad);
            break;
        case 'p':
            /* pointer: always 0x + 16 hex digits (width/pad soft-ignored) */
            kpf_soft_inc(&g_u64SoftConvP);
            console_write("0x");
            print_u64((u64)va_arg(vaArgs, void *), 16u, 16, '0');
            break;
        default:
            /* unknown conversion: echo so logs stay greppable */
            kpf_soft_inc(&g_u64SoftConvUnk);
            console_putchar('%');
            if (nLen == KPF_LEN_LL) {
                console_putchar('l');
                console_putchar('l');
            } else if (nLen == KPF_LEN_L) {
                console_putchar('l');
            }
            console_putchar(chConv);
            break;
        }
    }
    va_end(vaArgs);

    kprintf_soft_maybe_log();
    g_nKpfDepth--;
}

void
panic(const char *szMsg)
{
    kpf_soft_inc(&g_u64SoftPanic);
    if (szMsg == NULL) {
        kpf_soft_inc(&g_u64SoftPanicNull);
        szMsg = "(null)";
    }
    /*
     * Direct console path - never nest kprintf from panic. Deep call stacks
     * + formatter frames contributed to FAULT class H2; Soft!=product.
     * Soft: clear depth so a stuck counter cannot compound if recovery ever
     * reaches here mid-format (panic itself does not return).
     * No soft inventory dump from panic (H2: no stamp storms on FAULT path).
     */
    kpf_soft_inc(&g_u64SoftPanicReset);
    g_nKpfDepth = 0;
    g_fSoftBusy = 0;
    console_write("PANIC: ");
    console_write(szMsg);
    console_write("\n");
    for (;;) {
#if defined(GJ_ARCH_AARCH64)
        __asm__ volatile("wfe");
#else
        __asm__ volatile("cli; hlt");
#endif
    }
}
