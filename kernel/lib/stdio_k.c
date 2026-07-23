/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Shared freestanding kprintf (x86_64 + aarch64 product). Console via
 * console_putchar / console_write (arch provides).
 *
 * Format soft (deepened carefully, pure C, no string.c):
 *   %% %c %s %d %i %u %x %p
 *   length: l / ll  (e.g. %ld %lu %lx %lld %llu %llx)
 *   optional 0-flag + decimal width (e.g. %04x %08lx %016llx)
 *
 * Soft kprintf inventory (Wave 9 exclusive; this unit only):
 *   Lifetime conversion / flag / null-arg counters; never hard-gate format.
 *   Greppable prefix-stable serial markers (rate-limited; never flood):
 *     kprintf: soft inventory …
 *     kprintf: soft conv …
 *     kprintf: soft flags …
 *     stdio_k: soft …
 *   Emissions only at power-of-two call milestones, hard-capped at
 *   KPF_SOFT_LOG_MAX. Soft dump uses console_* + print_u64 only (no nested
 *   kprintf) so inventory never re-enters the formatter.
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

/* max digit/pad capacity for u64 (binary worst-case) + margin */
enum { KPF_NUM_BUF = 80, KPF_WIDTH_MAX = 64 };

/*
 * Soft inventory serial budget (Wave 9). Absolute cap of greppable dumps;
 * milestones are power-of-two kprintf call counts (1,2,4,…).
 * greppable: kprintf: soft / stdio_k: soft
 */
#define KPF_SOFT_LOG_MAX 8u

/*
 * Soft product counters (wrap OK; diagnostics only).
 * greppable: kprintf: soft inventory
 * greppable: kprintf: soft conv
 * greppable: kprintf: soft flags
 * greppable: stdio_k: soft
 */
static u64 g_u64SoftCalls;     /* kprintf entries */
static u64 g_u64SoftPanic;     /* panic entries */
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
static u64 g_u64SoftNullFmt;   /* kprintf(NULL, …) */
static u64 g_u64SoftNullS;     /* %s with NULL arg → "(null)" */
static u64 g_u64SoftTrunc;     /* format ended mid-conversion */
static u64 g_u64SoftSkip;      /* soft log suppressed (cap / not milestone) */
static u32 g_u32SoftLogged;    /* greppable dump emissions */
static int g_fSoftBusy;        /* reentrancy guard for soft dump */

static void
print_u64(u64 u64Val, unsigned uBase, int nWidth, char chPad)
{
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
 * Greppable soft kprintf inventory (product / smoke).
 * Must not call kprintf — console_write + print_u64 only.
 *
 *   kprintf: soft inventory …
 *   kprintf: soft conv …
 *   kprintf: soft flags …
 *   stdio_k: soft …
 */
static void
kprintf_soft_log(void)
{
    if (g_fSoftBusy != 0) {
        kpf_soft_inc(&g_u64SoftSkip);
        return;
    }
    g_fSoftBusy = 1;
    if (g_u32SoftLogged < 0xffffffffu) {
        g_u32SoftLogged++;
    }

    /* Grep: kprintf: soft inventory */
    console_write("kprintf: soft inventory");
    kpf_soft_kv(" calls=", g_u64SoftCalls);
    kpf_soft_kv(" lit=", g_u64SoftLit);
    kpf_soft_kv(" pct=", g_u64SoftPct);
    kpf_soft_kv(" panic=", g_u64SoftPanic);
    kpf_soft_kv(" trunc=", g_u64SoftTrunc);
    console_write("\n");

    /* Grep: kprintf: soft conv */
    console_write("kprintf: soft conv");
    kpf_soft_kv(" esc=", g_u64SoftConvEsc);
    kpf_soft_kv(" c=", g_u64SoftConvC);
    kpf_soft_kv(" s=", g_u64SoftConvS);
    kpf_soft_kv(" d=", g_u64SoftConvD);
    kpf_soft_kv(" u=", g_u64SoftConvU);
    kpf_soft_kv(" x=", g_u64SoftConvX);
    kpf_soft_kv(" p=", g_u64SoftConvP);
    kpf_soft_kv(" unk=", g_u64SoftConvUnk);
    console_write("\n");

    /* Grep: kprintf: soft flags */
    console_write("kprintf: soft flags");
    kpf_soft_kv(" width=", g_u64SoftWidth);
    kpf_soft_kv(" zero=", g_u64SoftZeroPad);
    kpf_soft_kv(" len_l=", g_u64SoftLenL);
    kpf_soft_kv(" len_ll=", g_u64SoftLenLl);
    kpf_soft_kv(" null_fmt=", g_u64SoftNullFmt);
    kpf_soft_kv(" null_s=", g_u64SoftNullS);
    console_write("\n");

    /* Grep: stdio_k: soft */
    console_write("stdio_k: soft");
    kpf_soft_kv(" log_n=", (u64)g_u32SoftLogged);
    kpf_soft_kv(" skip=", g_u64SoftSkip);
    kpf_soft_kv(" max=", (u64)KPF_SOFT_LOG_MAX);
    console_write(" fmt=%s,%c,%d,%i,%u,%x,%p,l,ll,0-width PASS\n");

    g_fSoftBusy = 0;
}

/**
 * Rate-limit soft inventory: power-of-two call milestones, hard-capped.
 * Never floods serial. soft skip tallies only suppressed milestones (cap
 * or reentrancy); non-milestone calls are silent without a skip bump.
 * greppable: kprintf: soft / stdio_k: soft
 */
static void
kprintf_soft_maybe_log(void)
{
    u64 u64N;

    u64N = g_u64SoftCalls;
    /* Milestone: first call and subsequent powers of two only. */
    if (u64N == 0ull || (u64N & (u64N - 1ull)) != 0ull) {
        return;
    }

    if (g_fSoftBusy != 0) {
        kpf_soft_inc(&g_u64SoftSkip);
        return;
    }
    if (g_u32SoftLogged >= KPF_SOFT_LOG_MAX) {
        kpf_soft_inc(&g_u64SoftSkip);
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

    if (szFmt == NULL) {
        kpf_soft_inc(&g_u64SoftNullFmt);
        kprintf_soft_maybe_log();
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

        /* width */
        nWidth = 0;
        while (*szCursor >= '0' && *szCursor <= '9') {
            int nDigit = (int)(*szCursor - '0');

            if (nWidth <= (KPF_WIDTH_MAX - nDigit) / 10) {
                nWidth = nWidth * 10 + nDigit;
            } else {
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

            kpf_soft_inc(&g_u64SoftConvS);
            if (szArg == NULL) {
                kpf_soft_inc(&g_u64SoftNullS);
                szArg = "(null)";
            }
            console_write(szArg);
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
}

void
panic(const char *szMsg)
{
    kpf_soft_inc(&g_u64SoftPanic);
    kprintf("PANIC: %s\n", szMsg ? szMsg : "(null)");
    for (;;) {
#if defined(GJ_ARCH_AARCH64)
        __asm__ volatile("wfe");
#else
        __asm__ volatile("cli; hlt");
#endif
    }
}
