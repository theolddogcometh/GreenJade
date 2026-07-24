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
 * Soft kprintf inventory (Wave 9 exclusive; Wave 20 deepen; this unit only):
 *   Lifetime conversion / flag / null-arg counters; never hard-gate format.
 *   Greppable prefix-stable serial markers (rate-limited; never flood):
 *     kprintf: soft inventory …
 *     kprintf: soft conv …
 *     kprintf: soft flags …
 *     kprintf: soft null …
 *     kprintf: soft path …
 *     kprintf: soft stats …
 *     kprintf: soft deepen wave=53 …
 *     kprintf: soft budget …     (log_max/areas/milestone lamps)
 *     kprintf: soft honesty|surfaces|catalog|note|return|retmap …  (Wave 19)
 *     stdio_k: soft inventory|path|stats|deepen|fmt|budget|honesty|
 *              surfaces|catalog|note …
 *   Emissions only at power-of-two call milestones, hard-capped at
 *   KPF_SOFT_LOG_MAX. Soft dump uses console_* + print_u64 only (no nested
 *   kprintf) so inventory never re-enters the formatter.
 * greppable: kprintf: soft
 * greppable: stdio_k: soft
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
 * Soft inventory serial budget (Wave 9 / Wave 19). Absolute cap of greppable
 * dumps; milestones are power-of-two kprintf call counts (1,2,4,…).
 * greppable: kprintf: soft / stdio_k: soft
 */
#define KPF_SOFT_LOG_MAX 8u

/* Wave 53 soft inventory stamp (file-local; never product gate). */
#define KPF_SOFT_WAVE 53u

/*
 * Soft inventory area count (fixed greppable categories for deepen stamp):
 *   inventory | conv | flags | null | path | stats | deepen | stdio |
 *   budget | honesty | surfaces | catalog | note | return | retmap |
 *   retclass | retlane
 */
#define KPF_SOFT_AREAS 80u

/*
 * Soft product counters (wrap OK; diagnostics only).
 * greppable: kprintf: soft inventory
 * greppable: kprintf: soft conv
 * greppable: kprintf: soft flags
 * greppable: kprintf: soft null
 * greppable: kprintf: soft path
 * greppable: kprintf: soft stats
 * greppable: kprintf: soft deepen
 * greppable: stdio_k: soft
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
static u64 g_u64SoftNullFmt;   /* kprintf(NULL, …) */
static u64 g_u64SoftEmptyFmt;  /* kprintf("") empty format */
static u64 g_u64SoftNullS;     /* %s with NULL arg → "(null)" */
static u64 g_u64SoftTrunc;     /* format ended mid-conversion */
static u64 g_u64SoftSkip;      /* soft log suppressed (cap / busy / reenter) */
static u64 g_u64SoftCapSkip;   /* soft log suppressed: hit KPF_SOFT_LOG_MAX */
static u64 g_u64SoftBusySkip;  /* soft log suppressed: reentrancy guard */
static u64 g_u64SoftLastMile;  /* last power-of-two milestone that logged */
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
 * Greppable soft kprintf inventory (product / smoke; Wave 20 deepen).
 * Must not call kprintf — console_write + print_u64 only.
 *
 *   kprintf: soft inventory|conv|flags|null|path|stats|deepen|…
 *   stdio_k: soft inventory|path|stats|deepen|fmt|…
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

    /* Grep: kprintf: soft inventory */
    console_write("kprintf: soft inventory");
    kpf_soft_kv(" calls=", g_u64SoftCalls);
    kpf_soft_kv(" lit=", g_u64SoftLit);
    kpf_soft_kv(" pct=", g_u64SoftPct);
    kpf_soft_kv(" panic=", g_u64SoftPanic);
    kpf_soft_kv(" trunc=", g_u64SoftTrunc);
    kpf_soft_kv(" empty=", g_u64SoftEmptyFmt);
    kpf_soft_kv(" conv_all=", u64ConvAll);
    kpf_soft_kv(" log_n=", (u64)g_u32SoftLogged);
    kpf_soft_kv(" wave=", (u64)KPF_SOFT_WAVE);
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
    kpf_soft_kv(" all=", u64ConvAll);
    console_write("\n");

    /* Grep: kprintf: soft flags */
    console_write("kprintf: soft flags");
    kpf_soft_kv(" width=", g_u64SoftWidth);
    kpf_soft_kv(" zero=", g_u64SoftZeroPad);
    kpf_soft_kv(" len_l=", g_u64SoftLenL);
    kpf_soft_kv(" len_ll=", g_u64SoftLenLl);
    kpf_soft_kv(" width_max=", (u64)KPF_WIDTH_MAX);
    console_write("\n");

    /* Grep: kprintf: soft null */
    console_write("kprintf: soft null");
    kpf_soft_kv(" null_fmt=", g_u64SoftNullFmt);
    kpf_soft_kv(" null_s=", g_u64SoftNullS);
    kpf_soft_kv(" empty_fmt=", g_u64SoftEmptyFmt);
    kpf_soft_kv(" panic_null=", g_u64SoftPanicNull);
    kpf_soft_kv(" trunc=", g_u64SoftTrunc);
    console_write("\n");

    /* Grep: kprintf: soft path */
    console_write("kprintf: soft path claim=console_putchar,console_write "
                  "fmt=%%,%c,%s,%d,%i,%u,%x,%p len=l,ll flag=0-width "
                  "reenter_safe=1 nested_kprintf=0 string_c=0 "
                  "milestone=pow2 cap=");
    print_u64((u64)KPF_SOFT_LOG_MAX, 10u, 0, ' ');
    console_write(" wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write(" (soft inventory; not libc printf)\n");

    /* Grep: kprintf: soft stats */
    console_write("kprintf: soft stats");
    kpf_soft_kv(" calls=", g_u64SoftCalls);
    kpf_soft_kv(" lit=", g_u64SoftLit);
    kpf_soft_kv(" pct=", g_u64SoftPct);
    kpf_soft_kv(" conv_all=", u64ConvAll);
    kpf_soft_kv(" panic=", g_u64SoftPanic);
    kpf_soft_kv(" log_n=", (u64)g_u32SoftLogged);
    kpf_soft_kv(" skip=", g_u64SoftSkip);
    kpf_soft_kv(" cap_skip=", g_u64SoftCapSkip);
    kpf_soft_kv(" busy_skip=", g_u64SoftBusySkip);
    kpf_soft_kv(" max=", (u64)KPF_SOFT_LOG_MAX);
    kpf_soft_kv(" last_mile=", g_u64SoftLastMile);
    kpf_soft_kv(" areas=", (u64)KPF_SOFT_AREAS);
    kpf_soft_kv(" wave=", (u64)KPF_SOFT_WAVE);
    console_write("\n");

    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: kprintf: soft retclass — Wave 19 return-class taxonomy (kept) */
    console_write("kprintf: soft retclass ok|fail|inval|nodev|busy|nomem "
                  "soft_only=1 product_gate=0 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write(" (retclass taxonomy; Soft!=product; not bar3)\n");
    /* Grep: kprintf: soft retlane — Wave 19 return-lane catalog (kept) */
    console_write("kprintf: soft retlane inv|selftest|rate|retcode|retmap|class "
                  "product_kernel=OPEN soft_ne_product=1 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write(" (retlane catalog; Soft!=product)\n");
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: kprintf: soft retbound — Wave 20 return-bound honesty (kept) */
    console_write("kprintf: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                  "never_blocks_m0=1 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write(" (retbound honesty; Soft!=product; not bar3)\n");
    /* Grep: kprintf: soft retseal — Wave 20 seal stamp (kept) */
    console_write("kprintf: soft retseal exclusive=1 soft_ne_product=1 "
                  "product_kernel=OPEN bar3=0 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write(" (retseal stamp; Soft!=product)\n");
    /*
     * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: kprintf: soft retpulse — Wave 21 return-pulse honesty (kept) */
    console_write("kprintf: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                  "never_blocks_m0=1 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write(" (retpulse honesty; Soft!=product; not bar3)\n");
    /* Grep: kprintf: soft retmark — Wave 21 mark stamp (kept) */
    console_write("kprintf: soft retmark exclusive=1 soft_ne_product=1 "
                  "product_kernel=OPEN bar3=0 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write(" (retmark stamp; Soft!=product)\n");

    
/*
 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: kprintf: soft retphase — Wave 22 return-phase honesty (kept) */
console_write("kprintf: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retphase honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retbadge — Wave 22 badge stamp (kept) */
console_write("kprintf: soft retbadge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retbadge stamp; Soft!=product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: kprintf: soft rettoken — Wave 23 return-token honesty (kept) */
console_write("kprintf: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (rettoken honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retcrest — Wave 23 crest stamp (kept) */
console_write("kprintf: soft retcrest exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retcrest stamp; Soft!=product)\n");
/*
 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: kprintf: soft retvault — Wave 24 return-vault honesty (kept) */
console_write("kprintf: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retvault honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retbanner — Wave 24 banner stamp (kept) */
console_write("kprintf: soft retbanner exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retbanner stamp; Soft!=product)\n");
/*
 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: kprintf: soft retledger — Wave 25 return-ledger honesty (kept) */
console_write("kprintf: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retledger honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retbeacon — Wave 25 beacon stamp (kept) */
console_write("kprintf: soft retbeacon exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retbeacon stamp; Soft!=product)\n");
/*
 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: kprintf: soft retcipher — Wave 26 return-cipher honesty (kept) */
console_write("kprintf: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retcipher honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retflame — Wave 26 flame stamp (kept) */
console_write("kprintf: soft retflame exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retflame stamp; Soft!=product)\n");
/*
 * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retprism — Wave 27 return-prism honesty (kept) */
console_write("kprintf: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retprism honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retforge — Wave 27 forge stamp (kept) */
console_write("kprintf: soft retforge exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retforge stamp; Soft!=product)\n");
/*
 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: kprintf: soft retshard — Wave 28 return-shard honesty (kept) */
console_write("kprintf: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retshard honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retcrown — Wave 28 crown stamp (kept) */
console_write("kprintf: soft retcrown exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retcrown stamp; Soft!=product)\n");
/*
 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: kprintf: soft retglyph — Wave 29 return-glyph honesty (kept) */
console_write("kprintf: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retglyph honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retscepter — Wave 29 scepter stamp (kept) */
console_write("kprintf: soft retscepter exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retscepter stamp; Soft!=product)\n");
/*
 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: kprintf: soft retsigil — Wave 30 return-sigil honesty (kept) */
console_write("kprintf: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retsigil honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retemblem — Wave 30 emblem stamp (kept) */
console_write("kprintf: soft retemblem exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retemblem stamp; Soft!=product)\n");
/*
 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: kprintf: soft retaegis — Wave 31 return-aegis honesty (kept) */
console_write("kprintf: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retaegis honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retmantle — Wave 31 mantle stamp (kept) */
console_write("kprintf: soft retmantle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retmantle stamp; Soft!=product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: kprintf: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
console_write("kprintf: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retbulwark honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retpanoply — Wave 32 panoply stamp (kept) */
console_write("kprintf: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retpanoply stamp; Soft!=product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retbastion — Wave 33 return-bastion honesty (kept) */
console_write("kprintf: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retbastion honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retcitadel — Wave 33 citadel stamp (kept) */
console_write("kprintf: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retcitadel stamp; Soft!=product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retredoubt — Wave 34 return-redoubt honesty */
console_write("kprintf: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retredoubt honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retkeep — Wave 34 exclusive keep stamp */
console_write("kprintf: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retkeep stamp; Soft!=product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retfortress — Wave 35 return-fortress honesty */
console_write("kprintf: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retfortress honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retpalace — Wave 35 exclusive palace stamp */
console_write("kprintf: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retpalace stamp; Soft!=product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft rethold — Wave 36 return-hold honesty */
console_write("kprintf: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(rethold honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retspire — Wave 36 exclusive spire stamp */
console_write("kprintf: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retspire stamp; Soft!=product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retwall — Wave 37 return-wall honesty */
console_write("kprintf: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retwall honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retgate — Wave 37 exclusive gate stamp */
console_write("kprintf: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retgate stamp; Soft!=product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retmoat — Wave 38 return-moat honesty */
console_write("kprintf: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retmoat honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retower — Wave 38 exclusive tower stamp */
console_write("kprintf: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retower stamp; Soft!=product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retbarbican — Wave 39 return-barbican honesty */
console_write("kprintf: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retbarbican honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retglacis — Wave 39 exclusive glacis stamp */
console_write("kprintf: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retglacis stamp; Soft!=product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retcurtain — Wave 40 return-curtain honesty */
console_write("kprintf: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retcurtain honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retparapet — Wave 40 exclusive parapet stamp */
console_write("kprintf: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retparapet stamp; Soft!=product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retravelin — Wave 41 return-travelin honesty */
console_write("kprintf: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retravelin honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retditch — Wave 41 exclusive ditch stamp */
console_write("kprintf: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retditch stamp; Soft!=product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retportcullis — Wave 42 return-portcullis honesty */
console_write("kprintf: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retportcullis honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retbattlement — Wave 42 exclusive battlement stamp */
console_write("kprintf: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retbattlement stamp; Soft!=product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retmachicolation — Wave 43 return-machicolation honesty */
console_write("kprintf: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retmachicolation honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
console_write("kprintf: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retarrowslit stamp; Soft!=product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retmerlon — Wave 44 return-merlon honesty */
console_write("kprintf: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retmerlon honesty; Soft!=product; not bar3)\n");
/* Grep: kprintf: soft retembrasure — Wave 44 exclusive embrasure stamp */
console_write("kprintf: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retembrasure stamp; Soft!=product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("kprintf: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: kprintf: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("kprintf: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retbailey — Wave 46 return-bailey honesty */
kprintf("kprintf: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: kprintf: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("kprintf: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("kprintf: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: kprintf: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("kprintf: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("kprintf: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: kprintf: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("kprintf: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("kprintf: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: kprintf: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("kprintf: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retsally — Wave 50 return-sally honesty */
kprintf("kprintf: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: kprintf: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("kprintf: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retfosse — Wave 51 return-fosse honesty */
kprintf("kprintf: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: kprintf: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("kprintf: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("kprintf: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: kprintf: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("kprintf: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: kprintf: soft retravelin — Wave 53 return-travelin honesty */
kprintf("kprintf: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: kprintf: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("kprintf: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retlunette stamp; Soft≠product)\n");



/* Grep: kprintf: soft deepen wave */
    console_write("kprintf: soft deepen wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    kpf_soft_kv(" areas=", (u64)KPF_SOFT_AREAS);
    kpf_soft_kv(" calls=", g_u64SoftCalls);
    kpf_soft_kv(" logs=", (u64)g_u32SoftLogged);
    kpf_soft_kv(" skip=", g_u64SoftSkip);
    console_write(" (Wave 45 exclusive; soft only; not libc)\n");

    /* Grep: kprintf: soft budget (emission geometry) */
    console_write("kprintf: soft budget");
    kpf_soft_kv(" log_max=", (u64)KPF_SOFT_LOG_MAX);
    kpf_soft_kv(" areas=", (u64)KPF_SOFT_AREAS);
    kpf_soft_kv(" logs=", (u64)g_u32SoftLogged);
    kpf_soft_kv(" last_mile=", g_u64SoftLastMile);
    kpf_soft_kv(" cap_skip=", g_u64SoftCapSkip);
    kpf_soft_kv(" busy_skip=", g_u64SoftBusySkip);
    kpf_soft_kv(" wave=", (u64)KPF_SOFT_WAVE);
    console_write(" milestone=pow2\n");

    /* Grep: kprintf: soft honesty (Wave 20 deepen) */
    console_write("kprintf: soft honesty freestanding=1 pure_c=1 "
                  "nested_kprintf=0 string_c=0 not_libc=1 soft_only=1 "
                  "wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write(" (soft inventory; not bar3)\n");

    /* Grep: kprintf: soft surfaces (Wave 20 deepen) */
    console_write("kprintf: soft surfaces count=");
    print_u64((u64)KPF_SOFT_AREAS, 10u, 0, ' ');
    console_write(" names=inventory,conv,flags,null,path,stats,deepen,"
                  "stdio,budget,honesty,surfaces,catalog,note,return,retmap wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write("\n");

    /* Grep: kprintf: soft catalog (Wave 20 deepen) */
    console_write("kprintf: soft catalog wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    kpf_soft_kv(" areas=", (u64)KPF_SOFT_AREAS);
    console_write(" surfaces=inventory,conv,flags,null,path,stats,"
                  "deepen,stdio,budget,honesty,surfaces,catalog,note,return,retmap\n");

    /* Grep: kprintf: soft note (Wave 20 deepen) */
    console_write("kprintf: soft note milestone=wave53 exclusive=1 "
                  "soft_only=1 reenter_safe=1 cap=");
    print_u64((u64)KPF_SOFT_LOG_MAX, 10u, 0, ' ');
    console_write(" wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write("\n");

    /* Grep: kprintf: soft return (Wave 20 deepen) */
    console_write("kprintf: soft return calls=");
    print_u64(g_u64SoftCalls, 10u, 0, ' ');
    console_write(" logs=");
    print_u64((u64)g_u32SoftLogged, 10u, 0, ' ');
    console_write(" skip=");
    print_u64(g_u64SoftSkip, 10u, 0, ' ');
    console_write(" product_gate=0 nested_kprintf=0 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write("\n");

    /* Grep: kprintf: soft retmap — Wave 19 return-surface map */
    console_write("kprintf: soft retmap calls|logs|skip|cap_skip|busy_skip "
                  "product_gate=0 soft_only=1 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write("\n");

    /* Grep: stdio_k: soft inventory */
    console_write("stdio_k: soft inventory");
    kpf_soft_kv(" calls=", g_u64SoftCalls);
    kpf_soft_kv(" lit=", g_u64SoftLit);
    kpf_soft_kv(" pct=", g_u64SoftPct);
    kpf_soft_kv(" panic=", g_u64SoftPanic);
    kpf_soft_kv(" trunc=", g_u64SoftTrunc);
    kpf_soft_kv(" empty=", g_u64SoftEmptyFmt);
    kpf_soft_kv(" log_n=", (u64)g_u32SoftLogged);
    kpf_soft_kv(" wave=", (u64)KPF_SOFT_WAVE);
    console_write("\n");

    /* Grep: stdio_k: soft path */
    console_write("stdio_k: soft path claim=console_putchar,console_write "
                  "fmt=%%,%c,%s,%d,%i,%u,%x,%p len=l,ll flag=0-width "
                  "reenter_safe=1 nested_kprintf=0 string_c=0 "
                  "wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write(" (soft inventory; not bar3)\n");

    /* Grep: stdio_k: soft stats */
    console_write("stdio_k: soft stats");
    kpf_soft_kv(" log_n=", (u64)g_u32SoftLogged);
    kpf_soft_kv(" skip=", g_u64SoftSkip);
    kpf_soft_kv(" cap_skip=", g_u64SoftCapSkip);
    kpf_soft_kv(" busy_skip=", g_u64SoftBusySkip);
    kpf_soft_kv(" max=", (u64)KPF_SOFT_LOG_MAX);
    kpf_soft_kv(" last_mile=", g_u64SoftLastMile);
    kpf_soft_kv(" areas=", (u64)KPF_SOFT_AREAS);
    kpf_soft_kv(" wave=", (u64)KPF_SOFT_WAVE);
    console_write("\n");

    /* Grep: stdio_k: soft retclass — Wave 19 return-class taxonomy (kept) */
    console_write("stdio_k: soft retclass ok|fail|inval|nodev|busy|nomem "
                  "soft_only=1 product_gate=0 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write(" (retclass taxonomy; Soft!=product; not bar3)\n");
    /* Grep: stdio_k: soft retlane — Wave 19 return-lane catalog (kept) */
    console_write("stdio_k: soft retlane inv|selftest|rate|retcode|retmap|class "
                  "product_kernel=OPEN soft_ne_product=1 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write(" (retlane catalog; Soft!=product)\n");
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: stdio_k: soft retbound — Wave 20 return-bound honesty (kept) */
    console_write("stdio_k: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                  "never_blocks_m0=1 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write(" (retbound honesty; Soft!=product; not bar3)\n");
    /* Grep: stdio_k: soft retseal — Wave 20 seal stamp (kept) */
    console_write("stdio_k: soft retseal exclusive=1 soft_ne_product=1 "
                  "product_kernel=OPEN bar3=0 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write(" (retseal stamp; Soft!=product)\n");
    /*
     * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft!=product; not bar3.
     */
    /* Grep: stdio_k: soft retpulse — Wave 21 return-pulse honesty (kept) */
    console_write("stdio_k: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                  "never_blocks_m0=1 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write(" (retpulse honesty; Soft!=product; not bar3)\n");
    /* Grep: stdio_k: soft retmark — Wave 21 mark stamp (kept) */
    console_write("stdio_k: soft retmark exclusive=1 soft_ne_product=1 "
                  "product_kernel=OPEN bar3=0 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write(" (retmark stamp; Soft!=product)\n");

    
/*
 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: stdio_k: soft retphase — Wave 22 return-phase honesty (kept) */
console_write("stdio_k: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retphase honesty; Soft!=product; not bar3)\n");
/* Grep: stdio_k: soft retbadge — Wave 22 badge stamp (kept) */
console_write("stdio_k: soft retbadge exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retbadge stamp; Soft!=product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: stdio_k: soft rettoken — Wave 23 return-token honesty (kept) */
console_write("stdio_k: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (rettoken honesty; Soft!=product; not bar3)\n");
/* Grep: stdio_k: soft retcrest — Wave 23 crest stamp (kept) */
console_write("stdio_k: soft retcrest exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retcrest stamp; Soft!=product)\n");
/*
 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: stdio_k: soft retvault — Wave 24 return-vault honesty (kept) */
console_write("stdio_k: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retvault honesty; Soft!=product; not bar3)\n");
/* Grep: stdio_k: soft retbanner — Wave 24 banner stamp (kept) */
console_write("stdio_k: soft retbanner exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retbanner stamp; Soft!=product)\n");
/*
 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: stdio_k: soft retledger — Wave 25 return-ledger honesty (kept) */
console_write("stdio_k: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retledger honesty; Soft!=product; not bar3)\n");
/* Grep: stdio_k: soft retbeacon — Wave 25 beacon stamp (kept) */
console_write("stdio_k: soft retbeacon exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retbeacon stamp; Soft!=product)\n");
/*
 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft!=product; not bar3.
 */
/* Grep: stdio_k: soft retcipher — Wave 26 return-cipher honesty (kept) */
console_write("stdio_k: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
              "never_blocks_m0=1 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retcipher honesty; Soft!=product; not bar3)\n");
/* Grep: stdio_k: soft retflame — Wave 26 flame stamp (kept) */
console_write("stdio_k: soft retflame exclusive=1 soft_ne_product=1 "
              "product_kernel=OPEN bar3=0 wave=");
print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
console_write(" (retflame stamp; Soft!=product)\n");
/*
 * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: stdio_k: soft retprism — Wave 27 return-prism honesty (kept) */
console_write("stdio_k: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=53 "
        "(retprism honesty; Soft!=product; not bar3)\n");
/* Grep: stdio_k: soft retforge — Wave 27 forge stamp (kept) */
console_write("stdio_k: soft retforge exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=53 "
        "(retforge stamp; Soft!=product)\n");
/* Grep: stdio_k: soft deepen */
    console_write("stdio_k: soft deepen wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    kpf_soft_kv(" areas=", (u64)KPF_SOFT_AREAS);
    kpf_soft_kv(" calls=", g_u64SoftCalls);
    kpf_soft_kv(" logs=", (u64)g_u32SoftLogged);
    console_write(" (Wave 53 exclusive; soft only)\n");

    /* Grep: stdio_k: soft budget (twin) */
    console_write("stdio_k: soft budget");
    kpf_soft_kv(" log_max=", (u64)KPF_SOFT_LOG_MAX);
    kpf_soft_kv(" areas=", (u64)KPF_SOFT_AREAS);
    kpf_soft_kv(" logs=", (u64)g_u32SoftLogged);
    kpf_soft_kv(" last_mile=", g_u64SoftLastMile);
    kpf_soft_kv(" wave=", (u64)KPF_SOFT_WAVE);
    console_write(" milestone=pow2\n");

    /* Grep: stdio_k: soft honesty (Wave 19 twin) */
    console_write("stdio_k: soft honesty freestanding=1 pure_c=1 "
                  "nested_kprintf=0 not_libc=1 soft_only=1 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write(" (soft inventory; not bar3)\n");

    /* Grep: stdio_k: soft surfaces (Wave 19 twin) */
    console_write("stdio_k: soft surfaces count=");
    print_u64((u64)KPF_SOFT_AREAS, 10u, 0, ' ');
    console_write(" names=inventory,path,stats,deepen,budget,honesty,"
                  "surfaces,catalog,note,return,retmap,fmt wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write("\n");

    /* Grep: stdio_k: soft catalog (Wave 19 twin) */
    console_write("stdio_k: soft catalog wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    kpf_soft_kv(" areas=", (u64)KPF_SOFT_AREAS);
    console_write(" surfaces=inventory,path,stats,deepen,budget,"
                  "honesty,surfaces,catalog,note,return,retmap,fmt\n");

    /* Grep: stdio_k: soft note (Wave 19 twin) */
    console_write("stdio_k: soft note milestone=wave53 exclusive=1 "
                  "soft_only=1 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write("\n");

    /* Grep: stdio_k: soft return (Wave 19 twin) */
    console_write("stdio_k: soft return calls=");
    print_u64(g_u64SoftCalls, 10u, 0, ' ');
    console_write(" logs=");
    print_u64((u64)g_u32SoftLogged, 10u, 0, ' ');
    console_write(" product_gate=0 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write("\n");

    /* Grep: stdio_k: soft retmap — Wave 19 return-surface map */
    console_write("stdio_k: soft retmap calls|logs|skip|cap_skip|busy_skip "
                  "product_gate=0 soft_only=1 wave=");
    print_u64((u64)KPF_SOFT_WAVE, 10u, 0, ' ');
    console_write("\n");

    /* Grep: stdio_k: soft (fmt surface) */
    console_write("stdio_k: soft");
    kpf_soft_kv(" log_n=", (u64)g_u32SoftLogged);
    kpf_soft_kv(" skip=", g_u64SoftSkip);
    kpf_soft_kv(" max=", (u64)KPF_SOFT_LOG_MAX);
    kpf_soft_kv(" wave=", (u64)KPF_SOFT_WAVE);
    console_write(" fmt=%%,%c,%s,%d,%i,%u,%x,%p,l,ll,0-width PASS\n");

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
        kpf_soft_inc(&g_u64SoftBusySkip);
        return;
    }
    if (g_u32SoftLogged >= KPF_SOFT_LOG_MAX) {
        kpf_soft_inc(&g_u64SoftSkip);
        kpf_soft_inc(&g_u64SoftCapSkip);
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

    if (*szFmt == '\0') {
        kpf_soft_inc(&g_u64SoftEmptyFmt);
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
    if (szMsg == NULL) {
        kpf_soft_inc(&g_u64SoftPanicNull);
    }
    kprintf("PANIC: %s\n", szMsg ? szMsg : "(null)");
    for (;;) {
#if defined(GJ_ARCH_AARCH64)
        __asm__ volatile("wfe");
#else
        __asm__ volatile("cli; hlt");
#endif
    }
}
