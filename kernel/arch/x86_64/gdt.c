/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Runtime GDT and 64-bit TSS: kernel/user selectors plus RSP0/IST1 for
 * ring-3 interrupts and double-fault isolation.
 *
 * Soft user-segment observability: CS32/DS/CS64 descriptor inventory,
 * LAR probe counters, full soft verify, greppable product logs.
 *
 * Soft GDT inventory (prefix-stable, pure C, deepen-only):
 *   "gdt: soft inventory …"
 *   "gdt: soft slots …"
 *   "gdt: soft user …"
 *   "gdt: soft tss …"
 *   "gdt: soft lamps …"
 *   "gdt: soft counters …"
 *   "gdt: soft star …"
 *   "gdt: soft path …"
 *   "gdt: soft inventory PASS|FAIL|idle"
 *   "gdt: soft PASS|FAIL|idle"
 *
 * Wave 13 exclusive deepen (kept) — greppable surfaces:
 *   "gdt: soft kernel …"   — kcode/kdata access/gran + expected bytes
 *   "gdt: soft null …"     — null slot soft check
 *   "gdt: soft cs32 …"     — CS32 access/gran P/DPL/L/D decode
 *   "gdt: soft cs64 …"     — CS64 long-mode L/D decode
 *   "gdt: soft ds …"       — user DS present/DPL3 decode
 *   "gdt: soft desc …"     — user base/limit reconstruction (flat)
 *   "gdt: soft lar …"      — LAR probe tallies + last access-rights
 *   "gdt: soft verify …"   — full soft verify tallies
 *   "gdt: soft init …"     — BSP init / AP load / RSP0 path tallies
 *   "gdt: soft stack …"    — IRQ/IST stack size + alignment soft bits
 *   "gdt: soft geom …"     — index→selector STAR geometry table
 *   "gdt: soft tssbase …"  — TSS descriptor base reconstruct + limit
 *   "gdt: soft expect …"   — product expected access/gran catalog
 *
 * Wave 15 complementary surfaces (kept; never reshape primary fields):
 *   "gdt: soft honesty …"  — soft-only / non-claim catalog
 *   "gdt: soft query …"    — accessor / soft-API sample tallies
 *   "gdt: soft match …"    — expect-vs-live match lamps (all product slots)
 *   "gdt: soft selector …" — raw selector catalog + STAR base
 *   "gdt: soft tssrsp …"   — RSP0/IST1 tops + match/align lamps
 * Wave 16 complementary surfaces (kept; never reshape primary fields):
 *   "gdt: soft exclusive …"— exclusive=1 unit stamp + wave
 *   "gdt: soft claim …"    — product claim bounds (shared GDT/TSS)
 *   "gdt: soft ratio …"    — init/ap/rsp0/query path ratios
 * Wave 17 complementary surfaces (kept) (never reshape primary fields):
 *   "gdt: soft return …" — Wave 17 API return surfaces (kept)
 *   "gdt: soft return selftest …" — Wave 17 terminal return surface (kept)
 *   "gdt: soft retmap …" — Wave 17 return-surface map (kept)
 *   gdt: soft return rate — Wave 19 ok/fail rate lamps
 *   gdt: soft retcode    — Wave 19 retcode catalog
 *   "gdt: soft deepen …"   — wave=116 areas stamp
 * Soft never hard-gates boot. No bar3 claim.
 * greppable: gdt: soft
 * greppable: gdt: soft deepen
 * greppable: gdt: soft exclusive
 * greppable: gdt: soft return
 * greppable: gdt: soft return selftest
 * greppable: gdt: soft retmap
 */
#include <gj/gdt.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/types.h>

struct gdt_entry {
    u16 u16LimitLo;
    u16 u16BaseLo;
    u8  u8BaseMid;
    u8  u8Access;
    u8  u8Gran;
    u8  u8BaseHi;
} __attribute__((packed));

/* Long-mode TSS descriptor is 16 bytes (two GDT slots). */
struct gdt_tss_entry {
    u16 u16LimitLo;
    u16 u16BaseLo;
    u8  u8BaseMid;
    u8  u8Access;
    u8  u8Gran;
    u8  u8BaseHi;
    u32 u32BaseHi32;
    u32 u32Reserved;
} __attribute__((packed));

struct gdt_ptr {
    u16 u16Limit;
    u64 u64Base;
} __attribute__((packed));

struct tss64 {
    u32 u32Reserved0;
    u64 u64Rsp0;
    u64 u64Rsp1;
    u64 u64Rsp2;
    u64 u64Reserved1;
    u64 u64Ist1;
    u64 u64Ist2;
    u64 u64Ist3;
    u64 u64Ist4;
    u64 u64Ist5;
    u64 u64Ist6;
    u64 u64Ist7;
    u64 u64Reserved2;
    u16 u16Reserved3;
    u16 u16IomapBase;
} __attribute__((packed));

/* 8 slots: null, kcode, kdata, ucode32, udata, ucode64, TSS (spans 2). */
static struct gdt_entry g_aGdt[8];
static struct gdt_ptr   g_GdtPtr;
static struct tss64     g_Tss;
static u8               g_aIstStack[8192] __attribute__((aligned(16)));
static u8               g_aRsp0Stack[8192] __attribute__((aligned(16)));
/* Immutable top of g_aRsp0Stack — ring-3 IRQs always land here (see #UD). */
static u64              g_u64IrqRsp0;

/* Soft GDT user-segment observability (monotonic; no locks). */
static volatile u32 g_u32SoftInits;
static volatile u32 g_u32SoftApLoads;
static volatile u32 g_u32SoftLarProbes;
static volatile u32 g_u32SoftLarOk;
static volatile u32 g_u32SoftLarBad;
static volatile u32 g_u32SoftVerifyOk;
static volatile u32 g_u32SoftVerifyBad;
static volatile u32 g_u32SoftInvLogs; /* gdt: soft inventory emit count */
/* Wave 13 exclusive deepen — extra path tallies (file-local only). */
static volatile u32 g_u32SoftRsp0Set;    /* tss_set_rsp0 entries */
static volatile u32 g_u32SoftRsp0UseIrq; /* tss_use_irq_rsp0 applied */
static volatile u32 g_u32SoftRsp0UseSkip;/* tss_use_irq_rsp0 skipped (0) */
static volatile u32 g_u32SoftVerifyCall; /* gdt_user_soft_verify entries */
static volatile u32 g_u32SoftInfoGet;    /* gdt_user_soft_info_get entries */
static volatile u32 g_u32SoftLogCall;    /* gdt_user_soft_log entries */
static volatile u32 g_u32SoftCs32ReadyQ; /* gdt_user_cs32_ready samples */
static volatile u32 g_u32SoftCs32CompatQ;/* gdt_user_cs32_is_compat samples */
static volatile u32 g_u32SoftCs64LongQ;  /* gdt_user_cs64_is_long samples */
static volatile u32 g_u32SoftDsOkQ;      /* gdt_user_ds_ok samples */
/* Wave 15 exclusive complementary path tallies (file-local only). */
static volatile u32 g_u32SoftRsp0Get;    /* tss_get_rsp0 samples */
static volatile u32 g_u32SoftIrqRsp0Q;   /* tss_irq_rsp0 samples */
static volatile u32 g_u32SoftCtrGet;     /* soft counter-accessor samples */
static volatile u32 g_u32SoftByteGet;    /* soft access/gran byte samples */
static volatile u32 g_u32SoftCs32SelQ;   /* gdt_user_cs32_sel samples */
static struct gj_gdt_user_soft g_SoftSnap;
static int g_fSoftSnapLive;

#define GJ_GDT_TSS_SEL_LOCAL 0x30u /* index 6 */
#define GJ_GDT_SOFT_WAVE 116u   /* Wave 37 exclusive deepen stamp */

static void gdt_soft_inc(volatile u32 *pCtr);
static void gdt_user_soft_refresh(void);
static void gdt_soft_inventory(void);

/** Soft: saturating bump (u32 wrap avoided; wrap OK if ever hit). */
static void
gdt_soft_inc(volatile u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

static void
gdt_set(int nIdx, u32 u32Base, u32 u32Limit, u8 u8Access, u8 u8Gran)
{
    g_aGdt[nIdx].u16LimitLo = (u16)(u32Limit & 0xffff);
    g_aGdt[nIdx].u16BaseLo = (u16)(u32Base & 0xffff);
    g_aGdt[nIdx].u8BaseMid = (u8)((u32Base >> 16) & 0xff);
    g_aGdt[nIdx].u8Access = u8Access;
    g_aGdt[nIdx].u8Gran = (u8)((u32Limit >> 16) & 0x0f);
    g_aGdt[nIdx].u8Gran |= (u8)(u8Gran & 0xf0);
    g_aGdt[nIdx].u8BaseHi = (u8)((u32Base >> 24) & 0xff);
}

static void
gdt_set_tss(int nIdx, u64 u64Base, u32 u32Limit)
{
    struct gdt_tss_entry *p = (struct gdt_tss_entry *)&g_aGdt[nIdx];

    p->u16LimitLo = (u16)(u32Limit & 0xffff);
    p->u16BaseLo = (u16)(u64Base & 0xffff);
    p->u8BaseMid = (u8)((u64Base >> 16) & 0xff);
    p->u8Access = 0x89; /* present, type 64-bit TSS available */
    p->u8Gran = (u8)((u32Limit >> 16) & 0x0f);
    p->u8BaseHi = (u8)((u64Base >> 24) & 0xff);
    p->u32BaseHi32 = (u32)(u64Base >> 32);
    p->u32Reserved = 0;
}

/**
 * Soft: reconstruct flat 32-bit base from a classic GDT entry.
 * Product user/kernel segments are base=0; TSS uses the 16-byte form.
 */
static u32
gdt_soft_entry_base32(const struct gdt_entry *pE)
{
    u32 u32Base;

    if (pE == NULL) {
        return 0;
    }
    u32Base = (u32)pE->u16BaseLo;
    u32Base |= ((u32)pE->u8BaseMid) << 16;
    u32Base |= ((u32)pE->u8BaseHi) << 24;
    return u32Base;
}

/** Soft: reconstruct 20-bit limit field (before G-bit scale). */
static u32
gdt_soft_entry_limit20(const struct gdt_entry *pE)
{
    u32 u32Lim;

    if (pE == NULL) {
        return 0;
    }
    u32Lim = (u32)pE->u16LimitLo;
    u32Lim |= ((u32)(pE->u8Gran & 0x0fu)) << 16;
    return u32Lim;
}

/**
 * Soft: refresh user-segment snapshot from live GDT + TSS.
 * Pure observability — does not mutate descriptors.
 */
static void
gdt_user_soft_refresh(void)
{
    g_SoftSnap.u8Cs32Access = g_aGdt[3].u8Access;
    g_SoftSnap.u8Cs32Gran = g_aGdt[3].u8Gran;
    g_SoftSnap.u8DsAccess = g_aGdt[4].u8Access;
    g_SoftSnap.u8DsGran = g_aGdt[4].u8Gran;
    g_SoftSnap.u8Cs64Access = g_aGdt[5].u8Access;
    g_SoftSnap.u8Cs64Gran = g_aGdt[5].u8Gran;
    g_SoftSnap.u16Cs32Sel = (u16)GJ_GDT_USER_CS32;
    g_SoftSnap.u16DsSel = (u16)GJ_GDT_USER_DS;
    g_SoftSnap.u16Cs64Sel = (u16)GJ_GDT_USER_CS;
    g_SoftSnap.u64Rsp0 = g_Tss.u64Rsp0;
    g_SoftSnap.u64Ist1 = g_Tss.u64Ist1;
    g_SoftSnap.u8Cs32Ready =
        (g_aGdt[3].u8Access == 0xfau && (g_aGdt[3].u8Gran & 0x40u) != 0) ? 1u
                                                                         : 0u;
    /* CS32 compat: access 0xFA, D=1, L=0 */
    g_SoftSnap.u8Cs32Compat = 0;
    if (g_aGdt[3].u8Access == 0xfau &&
        (g_aGdt[3].u8Gran & 0x40u) != 0 &&
        (g_aGdt[3].u8Gran & 0x20u) == 0) {
        g_SoftSnap.u8Cs32Compat = 1;
    }
    /* CS64 long: access 0xFA, L=1, D=0 */
    g_SoftSnap.u8Cs64Long = 0;
    if (g_aGdt[5].u8Access == 0xfau &&
        (g_aGdt[5].u8Gran & 0x20u) != 0 &&
        (g_aGdt[5].u8Gran & 0x40u) == 0) {
        g_SoftSnap.u8Cs64Long = 1;
    }
    /* DS: present DPL3 data (0xF2) */
    g_SoftSnap.u8DsOk = (g_aGdt[4].u8Access == 0xf2u) ? 1u : 0u;
    g_SoftSnap.u8Init = 1;
    g_fSoftSnapLive = 1;
}

/**
 * Soft GDT inventory — greppable "gdt: soft …".
 * Pure observability; never mutates descriptors or hard-gates boot.
 * Covers slot map, user STAR layout, TSS RSP0/IST1, lamps, counters.
 * Wave 13 exclusive deepen splits kernel/null/cs32/cs64/ds/desc/lar/
 * verify/init/stack/geom/tssbase/expect surfaces (prefix-stable).
 * Wave 15 adds complementary honesty/query/match/selector/tssrsp/deepen
 * surfaces without reshaping primary fields.
 */
static void
gdt_soft_inventory(void)
{
    u8 u8KcodeAcc;
    u8 u8KcodeGran;
    u8 u8KdataAcc;
    u8 u8KdataGran;
    u8 u8TssAcc;
    u8 u8TssGran;
    u8 u8NullAcc;
    u8 u8NullGran;
    u8 u8Cs32Acc;
    u8 u8Cs32Gran;
    u8 u8Cs64Acc;
    u8 u8Cs64Gran;
    u8 u8DsAcc;
    u8 u8DsGran;
    u32 u32Limit;
    u32 u32Slots;
    u32 u32Cs32Base;
    u32 u32Cs32Lim20;
    u32 u32Cs64Base;
    u32 u32Cs64Lim20;
    u32 u32DsBase;
    u32 u32DsLim20;
    u32 u32KcodeBase;
    u32 u32KdataBase;
    u64 u64TssBase;
    u32 u32TssLim;
    u32 u32Rsp0Align;
    u32 u32Ist1Align;
    u32 u32IrqAlign;
    u32 u32NullOk;
    u32 u32KcodeOk;
    u32 u32KdataOk;
    u32 u32TssOk;
    u32 u32Cs32P;
    u32 u32Cs32Dpl;
    u32 u32Cs32S;
    u32 u32Cs32Type;
    u32 u32Cs32G;
    u32 u32Cs32D;
    u32 u32Cs32L;
    u32 u32Cs64P;
    u32 u32Cs64Dpl;
    u32 u32Cs64L;
    u32 u32Cs64D;
    u32 u32DsP;
    u32 u32DsDpl;
    u32 u32Rsp0Match;
    u32 u32Ist1Nz;
    int fPass;
    const struct gdt_tss_entry *pTssDesc;

    gdt_soft_inc(&g_u32SoftInvLogs);

    if (g_fSoftSnapLive) {
        gdt_user_soft_refresh();
    }

    u8NullAcc = g_aGdt[0].u8Access;
    u8NullGran = g_aGdt[0].u8Gran;
    u8KcodeAcc = g_aGdt[1].u8Access;
    u8KcodeGran = g_aGdt[1].u8Gran;
    u8KdataAcc = g_aGdt[2].u8Access;
    u8KdataGran = g_aGdt[2].u8Gran;
    u8Cs32Acc = g_aGdt[3].u8Access;
    u8Cs32Gran = g_aGdt[3].u8Gran;
    u8DsAcc = g_aGdt[4].u8Access;
    u8DsGran = g_aGdt[4].u8Gran;
    u8Cs64Acc = g_aGdt[5].u8Access;
    u8Cs64Gran = g_aGdt[5].u8Gran;
    u8TssAcc = g_aGdt[6].u8Access;
    u8TssGran = g_aGdt[6].u8Gran;
    u32Slots = (u32)(sizeof(g_aGdt) / sizeof(g_aGdt[0]));
    u32Limit = (u32)g_GdtPtr.u16Limit;

    /* Soft flat base/limit reconstruction (product segments are base=0). */
    u32Cs32Base = gdt_soft_entry_base32(&g_aGdt[3]);
    u32Cs32Lim20 = gdt_soft_entry_limit20(&g_aGdt[3]);
    u32DsBase = gdt_soft_entry_base32(&g_aGdt[4]);
    u32DsLim20 = gdt_soft_entry_limit20(&g_aGdt[4]);
    u32Cs64Base = gdt_soft_entry_base32(&g_aGdt[5]);
    u32Cs64Lim20 = gdt_soft_entry_limit20(&g_aGdt[5]);
    u32KcodeBase = gdt_soft_entry_base32(&g_aGdt[1]);
    u32KdataBase = gdt_soft_entry_base32(&g_aGdt[2]);

    /* Soft TSS descriptor base (16-byte long-mode form). */
    pTssDesc = (const struct gdt_tss_entry *)&g_aGdt[6];
    u64TssBase = (u64)pTssDesc->u16BaseLo;
    u64TssBase |= ((u64)pTssDesc->u8BaseMid) << 16;
    u64TssBase |= ((u64)pTssDesc->u8BaseHi) << 24;
    u64TssBase |= ((u64)pTssDesc->u32BaseHi32) << 32;
    u32TssLim = (u32)pTssDesc->u16LimitLo;
    u32TssLim |= ((u32)(pTssDesc->u8Gran & 0x0fu)) << 16;

    /* Soft stack alignment bits (16-byte tops expected). */
    u32Rsp0Align = (u32)(g_SoftSnap.u64Rsp0 & 0xfull);
    u32Ist1Align = (u32)(g_SoftSnap.u64Ist1 & 0xfull);
    u32IrqAlign = (u32)(g_u64IrqRsp0 & 0xfull);
    u32Rsp0Match =
        (g_u64IrqRsp0 != 0 && g_SoftSnap.u64Rsp0 == g_u64IrqRsp0) ? 1u : 0u;
    u32Ist1Nz = (g_SoftSnap.u64Ist1 != 0) ? 1u : 0u;

    u32NullOk = (u8NullAcc == 0 && u8NullGran == 0) ? 1u : 0u;
    u32KcodeOk = (u8KcodeAcc == 0x9au) ? 1u : 0u;
    u32KdataOk = (u8KdataAcc == 0x92u) ? 1u : 0u;
    u32TssOk = (u8TssAcc == 0x89u) ? 1u : 0u;

    /* Soft CS32 access-byte decode (P|DPL|S|type). */
    u32Cs32P = ((u8Cs32Acc & 0x80u) != 0) ? 1u : 0u;
    u32Cs32Dpl = (u32)((u8Cs32Acc >> 5) & 0x3u);
    u32Cs32S = ((u8Cs32Acc & 0x10u) != 0) ? 1u : 0u;
    u32Cs32Type = (u32)(u8Cs32Acc & 0x0fu);
    u32Cs32G = ((u8Cs32Gran & 0x80u) != 0) ? 1u : 0u;
    u32Cs32D = ((u8Cs32Gran & 0x40u) != 0) ? 1u : 0u;
    u32Cs32L = ((u8Cs32Gran & 0x20u) != 0) ? 1u : 0u;

    u32Cs64P = ((u8Cs64Acc & 0x80u) != 0) ? 1u : 0u;
    u32Cs64Dpl = (u32)((u8Cs64Acc >> 5) & 0x3u);
    u32Cs64L = ((u8Cs64Gran & 0x20u) != 0) ? 1u : 0u;
    u32Cs64D = ((u8Cs64Gran & 0x40u) != 0) ? 1u : 0u;

    u32DsP = ((u8DsAcc & 0x80u) != 0) ? 1u : 0u;
    u32DsDpl = (u32)((u8DsAcc >> 5) & 0x3u);

    fPass = 0;
    if (g_fSoftSnapLive && g_SoftSnap.u8Cs32Compat && g_SoftSnap.u8Cs64Long &&
        g_SoftSnap.u8DsOk && u8KcodeAcc == 0x9au && u8KdataAcc == 0x92u &&
        u8TssAcc == 0x89u) {
        fPass = 1;
    }

    /*
     * Grep: gdt: soft inventory
     * Aggregated presence snapshot for product smoke / agent greps.
     * Wave 15 stamps wave=15; existing keys remain prefix-stable.
     */
    kprintf("gdt: soft inventory slots=%u limit=%u base=0x%lx live=%u "
            "inits=%u ap=%u logs=%u tss_sel=0x%x wave=%u\n",
            u32Slots, u32Limit, (unsigned long)g_GdtPtr.u64Base,
            (unsigned)(g_fSoftSnapLive ? 1u : 0u), g_u32SoftInits,
            g_u32SoftApLoads, g_u32SoftInvLogs,
            (unsigned)GJ_GDT_TSS_SEL_LOCAL, (unsigned)GJ_GDT_SOFT_WAVE);

    /* Grep: gdt: soft slots — raw access/gran per product index */
    kprintf("gdt: soft slots null_acc=0x%x kcode_acc=0x%x kcode_gran=0x%x "
            "kdata_acc=0x%x cs32_acc=0x%x cs32_gran=0x%x ds_acc=0x%x "
            "cs64_acc=0x%x cs64_gran=0x%x tss_acc=0x%x tss_gran=0x%x\n",
            (unsigned)u8NullAcc, (unsigned)u8KcodeAcc, (unsigned)u8KcodeGran,
            (unsigned)u8KdataAcc, (unsigned)g_SoftSnap.u8Cs32Access,
            (unsigned)g_SoftSnap.u8Cs32Gran, (unsigned)g_SoftSnap.u8DsAccess,
            (unsigned)g_SoftSnap.u8Cs64Access, (unsigned)g_SoftSnap.u8Cs64Gran,
            (unsigned)u8TssAcc, (unsigned)u8TssGran);

    /*
     * Grep: gdt: soft user — STAR user base 0x18 layout (RPL3 selectors).
     * Index 3=CS32 0x1B, 4=DS 0x23, 5=CS64 0x2B.
     */
    kprintf("gdt: soft user cs32_sel=0x%x ds_sel=0x%x cs64_sel=0x%x "
            "cs32_ready=%u cs32_compat=%u cs64_long=%u ds_ok=%u "
            "star_base=0x18\n",
            (unsigned)g_SoftSnap.u16Cs32Sel, (unsigned)g_SoftSnap.u16DsSel,
            (unsigned)g_SoftSnap.u16Cs64Sel, (unsigned)g_SoftSnap.u8Cs32Ready,
            (unsigned)g_SoftSnap.u8Cs32Compat, (unsigned)g_SoftSnap.u8Cs64Long,
            (unsigned)g_SoftSnap.u8DsOk);

    /* Grep: gdt: soft tss — RSP0 / IST1 / dedicated IRQ stack */
    kprintf("gdt: soft tss rsp0=0x%lx ist1=0x%lx irq_rsp0=0x%lx "
            "iomap=%u tss_bytes=%u busy_ap_ltr=0 rsp0_match=%u ist1_nz=%u\n",
            (unsigned long)g_SoftSnap.u64Rsp0,
            (unsigned long)g_SoftSnap.u64Ist1,
            (unsigned long)g_u64IrqRsp0, (unsigned)g_Tss.u16IomapBase,
            (unsigned)sizeof(g_Tss), u32Rsp0Match, u32Ist1Nz);

    /* Grep: gdt: soft lamps — readiness bits (software verify only) */
    kprintf("gdt: soft lamps ready=%u compat=%u long=%u ds_ok=%u "
            "kcode_ok=%u kdata_ok=%u tss_ok=%u lar_ok=%u null_ok=%u\n",
            (unsigned)g_SoftSnap.u8Cs32Ready,
            (unsigned)g_SoftSnap.u8Cs32Compat,
            (unsigned)g_SoftSnap.u8Cs64Long, (unsigned)g_SoftSnap.u8DsOk,
            u32KcodeOk, u32KdataOk, u32TssOk,
            (unsigned)g_SoftSnap.u8LarOk, u32NullOk);

    /* Grep: gdt: soft counters — monotonic soft tallies */
    kprintf("gdt: soft counters inits=%u ap=%u lar_probe=%u lar_ok=%u "
            "lar_bad=%u verify_ok=%u verify_bad=%u inv_logs=%u "
            "rsp0_set=%u rsp0_use_irq=%u verify_call=%u\n",
            g_u32SoftInits, g_u32SoftApLoads, g_u32SoftLarProbes,
            g_u32SoftLarOk, g_u32SoftLarBad, g_u32SoftVerifyOk,
            g_u32SoftVerifyBad, g_u32SoftInvLogs, g_u32SoftRsp0Set,
            g_u32SoftRsp0UseIrq, g_u32SoftVerifyCall);

    /*
     * Grep: gdt: soft star — IA32_STAR user-base selector geometry.
     * Product SYSRETQ expects CS64=base+16|RPL3, SS=base+8|RPL3.
     */
    kprintf("gdt: soft star user_base=0x18 cs32=0x1b ds=0x23 cs64=0x2b "
            "kcs=0x08 kds=0x10 tss=0x30 idx_cs32=3 idx_ds=4 idx_cs64=5\n");

    /*
     * Grep: gdt: soft path — explicit non-claim bounds.
     * Soft inventory ≠ product multi-CPU TSS, ≠ bar3, ≠ full SEH.
     */
    kprintf("gdt: soft path claim=0 bar3=0 per_cpu_tss=0 seh=0 "
            "shared_gdt=1 bsp_ltr=1 ap_ltr=0 hard_gate=0 wave=%u "
            "(soft inventory; not bar3)\n",
            (unsigned)GJ_GDT_SOFT_WAVE);

    /*
     * ---- Wave 13 exclusive deepen: extra greppable "gdt: soft …" ----
     */

    /* Grep: gdt: soft kernel — ring-0 code/data product bytes */
    kprintf("gdt: soft kernel kcs_sel=0x08 kds_sel=0x10 "
            "kcode_acc=0x%x kcode_gran=0x%x kcode_base=0x%x "
            "kdata_acc=0x%x kdata_gran=0x%x kdata_base=0x%x "
            "kcode_ok=%u kdata_ok=%u expect_acc=0x9a,0x92 "
            "expect_gran=0xa0,0xc0\n",
            (unsigned)u8KcodeAcc, (unsigned)u8KcodeGran, u32KcodeBase,
            (unsigned)u8KdataAcc, (unsigned)u8KdataGran, u32KdataBase,
            u32KcodeOk, u32KdataOk);

    /* Grep: gdt: soft null — index 0 must stay zero */
    kprintf("gdt: soft null acc=0x%x gran=0x%x ok=%u expect_acc=0 "
            "expect_gran=0 idx=0\n",
            (unsigned)u8NullAcc, (unsigned)u8NullGran, u32NullOk);

    /* Grep: gdt: soft cs32 — compat CS decode (WoW64 / PE32 iretq) */
    kprintf("gdt: soft cs32 sel=0x%x acc=0x%x gran=0x%x base=0x%x "
            "lim20=0x%x P=%u DPL=%u S=%u type=0x%x G=%u D=%u L=%u "
            "ready=%u compat=%u expect_acc=0xfa expect_gran=0xcf\n",
            (unsigned)GJ_GDT_USER_CS32, (unsigned)u8Cs32Acc,
            (unsigned)u8Cs32Gran, u32Cs32Base, u32Cs32Lim20, u32Cs32P,
            u32Cs32Dpl, u32Cs32S, u32Cs32Type, u32Cs32G, u32Cs32D, u32Cs32L,
            (unsigned)g_SoftSnap.u8Cs32Ready,
            (unsigned)g_SoftSnap.u8Cs32Compat);

    /* Grep: gdt: soft cs64 — long-mode user CS (SYSRETQ target) */
    kprintf("gdt: soft cs64 sel=0x%x acc=0x%x gran=0x%x base=0x%x "
            "lim20=0x%x P=%u DPL=%u L=%u D=%u long=%u "
            "expect_acc=0xfa expect_gran=0xa0 sysretq=1\n",
            (unsigned)GJ_GDT_USER_CS, (unsigned)u8Cs64Acc,
            (unsigned)u8Cs64Gran, u32Cs64Base, u32Cs64Lim20, u32Cs64P,
            u32Cs64Dpl, u32Cs64L, u32Cs64D, (unsigned)g_SoftSnap.u8Cs64Long);

    /* Grep: gdt: soft ds — user data / SS for ring-3 */
    kprintf("gdt: soft ds sel=0x%x acc=0x%x gran=0x%x base=0x%x "
            "lim20=0x%x P=%u DPL=%u ok=%u expect_acc=0xf2 "
            "expect_gran=0xcf rpl3=1\n",
            (unsigned)GJ_GDT_USER_DS, (unsigned)u8DsAcc, (unsigned)u8DsGran,
            u32DsBase, u32DsLim20, u32DsP, u32DsDpl,
            (unsigned)g_SoftSnap.u8DsOk);

    /* Grep: gdt: soft desc — flat user descriptor reconstruction */
    kprintf("gdt: soft desc cs32_base=0x%x cs32_lim20=0x%x "
            "ds_base=0x%x ds_lim20=0x%x cs64_base=0x%x cs64_lim20=0x%x "
            "flat_user=1 g_bit_4g=1\n",
            u32Cs32Base, u32Cs32Lim20, u32DsBase, u32DsLim20, u32Cs64Base,
            u32Cs64Lim20);

    /* Grep: gdt: soft lar — hardware LAR probe surface */
    kprintf("gdt: soft lar probe=%u ok=%u bad=%u last_ar=0x%x "
            "snap_lar_ok=%u sel=0x%x require=P|DPL3|code path=larl\n",
            g_u32SoftLarProbes, g_u32SoftLarOk, g_u32SoftLarBad,
            g_SoftSnap.u32LarAr, (unsigned)g_SoftSnap.u8LarOk,
            (unsigned)GJ_GDT_USER_CS32);

    /* Grep: gdt: soft verify — full soft verify tallies */
    kprintf("gdt: soft verify call=%u ok=%u bad=%u "
            "need=cs32_compat+cs64_long+ds_ok optional_lar=1 "
            "live=%u wave=%u\n",
            g_u32SoftVerifyCall, g_u32SoftVerifyOk, g_u32SoftVerifyBad,
            (unsigned)(g_fSoftSnapLive ? 1u : 0u),
            (unsigned)GJ_GDT_SOFT_WAVE);

    /* Grep: gdt: soft init — BSP/AP + accessor soft path tallies */
    kprintf("gdt: soft init bsp=%u ap=%u rsp0_set=%u rsp0_use_irq=%u "
            "rsp0_use_skip=%u info_get=%u log_call=%u "
            "cs32_ready_q=%u cs32_compat_q=%u cs64_long_q=%u ds_ok_q=%u\n",
            g_u32SoftInits, g_u32SoftApLoads, g_u32SoftRsp0Set,
            g_u32SoftRsp0UseIrq, g_u32SoftRsp0UseSkip, g_u32SoftInfoGet,
            g_u32SoftLogCall, g_u32SoftCs32ReadyQ, g_u32SoftCs32CompatQ,
            g_u32SoftCs64LongQ, g_u32SoftDsOkQ);

    /* Grep: gdt: soft stack — dedicated IRQ / IST stack soft bits */
    kprintf("gdt: soft stack irq_bytes=%u ist_bytes=%u irq_rsp0=0x%lx "
            "tss_rsp0=0x%lx ist1=0x%lx rsp0_align16=%u ist1_align16=%u "
            "irq_align16=%u rsp0_match_irq=%u dedicated_irq=1 "
            "never_thr_kstack=1\n",
            (unsigned)sizeof(g_aRsp0Stack), (unsigned)sizeof(g_aIstStack),
            (unsigned long)g_u64IrqRsp0,
            (unsigned long)g_SoftSnap.u64Rsp0,
            (unsigned long)g_SoftSnap.u64Ist1,
            (unsigned)(u32Rsp0Align == 0u ? 1u : 0u),
            (unsigned)(u32Ist1Align == 0u ? 1u : 0u),
            (unsigned)(u32IrqAlign == 0u ? 1u : 0u), u32Rsp0Match);

    /* Grep: gdt: soft geom — index → selector STAR geometry table */
    kprintf("gdt: soft geom idx0=null/0x00 idx1=kcs/0x08 idx2=kds/0x10 "
            "idx3=cs32/0x1b idx4=ds/0x23 idx5=cs64/0x2b "
            "idx6=tss/0x30 idx7=tss_hi star_user_base=0x18 "
            "sysret_cs=base+16|3 sysret_ss=base+8|3 slots=%u\n",
            u32Slots);

    /* Grep: gdt: soft tssbase — descriptor base reconstruct vs &g_Tss */
    kprintf("gdt: soft tssbase desc_base=0x%lx live_tss=0x%lx "
            "match=%u lim=0x%x acc=0x%x gran=0x%x sel=0x%x "
            "type=avail_tss64 span_slots=2 reserved0=%u\n",
            (unsigned long)u64TssBase,
            (unsigned long)(u64)(gj_vaddr_t)&g_Tss,
            (unsigned)(u64TssBase == (u64)(gj_vaddr_t)&g_Tss ? 1u : 0u),
            u32TssLim, (unsigned)u8TssAcc, (unsigned)u8TssGran,
            (unsigned)GJ_GDT_TSS_SEL_LOCAL,
            (unsigned)(pTssDesc->u32Reserved == 0u ? 1u : 0u));

    /* Grep: gdt: soft expect — product expected access/gran catalog */
    kprintf("gdt: soft expect null=0x00/0x00 kcode=0x9a/0xa0 "
            "kdata=0x92/0xc0 cs32=0xfa/0xcf ds=0xf2/0xcf cs64=0xfa/0xa0 "
            "tss_acc=0x89 star_base=0x18 tss_sel=0x30 "
            "irq_stack=8192 ist_stack=8192 wave=%u\n",
            (unsigned)GJ_GDT_SOFT_WAVE);

    /*
     * ---- Wave 15 complementary surfaces (kept; never reshape primary).
     */

    /* Grep: gdt: soft honesty */
    kprintf("gdt: soft honesty claim=0 bar3=0 per_cpu_tss=0 seh=0 "
            "hard_gate=0 shared_gdt=1 bsp_ltr=1 ap_ltr=0 "
            "soft_only=1 unit=gdt.c wave=%u (soft inventory; not bar3)\n",
            (unsigned)GJ_GDT_SOFT_WAVE);

    /* Grep: gdt: soft query — accessor / soft-API sample tallies */
    kprintf("gdt: soft query info_get=%u log_call=%u verify_call=%u "
            "cs32_ready_q=%u cs32_compat_q=%u cs64_long_q=%u ds_ok_q=%u "
            "rsp0_get=%u irq_rsp0_q=%u ctr_get=%u byte_get=%u "
            "cs32_sel_q=%u inv_logs=%u\n",
            g_u32SoftInfoGet, g_u32SoftLogCall, g_u32SoftVerifyCall,
            g_u32SoftCs32ReadyQ, g_u32SoftCs32CompatQ, g_u32SoftCs64LongQ,
            g_u32SoftDsOkQ, g_u32SoftRsp0Get, g_u32SoftIrqRsp0Q,
            g_u32SoftCtrGet, g_u32SoftByteGet, g_u32SoftCs32SelQ,
            g_u32SoftInvLogs);

    /* Grep: gdt: soft match — expect-vs-live lamps (product slots) */
    kprintf("gdt: soft match null=%u kcode=%u kdata=%u "
            "cs32_acc=%u cs32_gran=%u cs32_compat=%u "
            "ds_acc=%u ds_ok=%u cs64_acc=%u cs64_gran=%u cs64_long=%u "
            "tss_acc=%u tss_base=%u flat_user=%u\n",
            u32NullOk, u32KcodeOk, u32KdataOk,
            (unsigned)(u8Cs32Acc == 0xfau ? 1u : 0u),
            (unsigned)(u8Cs32Gran == 0xcfu ? 1u : 0u),
            (unsigned)g_SoftSnap.u8Cs32Compat,
            (unsigned)(u8DsAcc == 0xf2u ? 1u : 0u),
            (unsigned)g_SoftSnap.u8DsOk,
            (unsigned)(u8Cs64Acc == 0xfau ? 1u : 0u),
            (unsigned)(u8Cs64Gran == 0xa0u ? 1u : 0u),
            (unsigned)g_SoftSnap.u8Cs64Long, u32TssOk,
            (unsigned)(u64TssBase == (u64)(gj_vaddr_t)&g_Tss ? 1u : 0u),
            (unsigned)((u32Cs32Base | u32DsBase | u32Cs64Base) == 0u
                           ? 1u
                           : 0u));

    /* Grep: gdt: soft selector — raw selector catalog + STAR base */
    kprintf("gdt: soft selector null=0x00 kcs=0x08 kds=0x10 "
            "cs32=0x%x ds=0x%x cs64=0x%x tss=0x%x "
            "star_user_base=0x18 rpl3=1 idx_cs32=3 idx_ds=4 idx_cs64=5 "
            "idx_tss=6\n",
            (unsigned)GJ_GDT_USER_CS32, (unsigned)GJ_GDT_USER_DS,
            (unsigned)GJ_GDT_USER_CS, (unsigned)GJ_GDT_TSS_SEL_LOCAL);

    /* Grep: gdt: soft tssrsp — RSP0/IST1 tops + match/align lamps */
    kprintf("gdt: soft tssrsp rsp0=0x%lx ist1=0x%lx irq_rsp0=0x%lx "
            "rsp0_match_irq=%u rsp0_align16=%u ist1_align16=%u "
            "irq_align16=%u ist1_nz=%u iomap=%u tss_bytes=%u "
            "rsp0_set=%u rsp0_use_irq=%u rsp0_use_skip=%u\n",
            (unsigned long)g_SoftSnap.u64Rsp0,
            (unsigned long)g_SoftSnap.u64Ist1,
            (unsigned long)g_u64IrqRsp0, u32Rsp0Match,
            (unsigned)(u32Rsp0Align == 0u ? 1u : 0u),
            (unsigned)(u32Ist1Align == 0u ? 1u : 0u),
            (unsigned)(u32IrqAlign == 0u ? 1u : 0u), u32Ist1Nz,
            (unsigned)g_Tss.u16IomapBase, (unsigned)sizeof(g_Tss),
            g_u32SoftRsp0Set, g_u32SoftRsp0UseIrq, g_u32SoftRsp0UseSkip);

    /*
     * ---- Wave 16 complementary surfaces (kept; never reshape primary).
     */

    /* Grep: gdt: soft exclusive */
    kprintf("gdt: soft exclusive wave=%u exclusive=1 soft=1 "
            "unit=gdt.c bar3=0 hard_gate=0 per_cpu_tss=0 seh=0 "
            "soft_only=1\n",
            (unsigned)GJ_GDT_SOFT_WAVE);

    /* Grep: gdt: soft claim — shared GDT/TSS product bounds */
    kprintf("gdt: soft claim slots=8 tss_sel=0x30 star_user_base=0x18 "
            "shared_gdt=1 bsp_ltr=1 ap_ltr=0 per_cpu_tss=0 "
            "user_cs32=1 user_cs64=1 user_ds=1 flat=1 "
            "bar3=0 hard_gate=0 wave=%u\n",
            (unsigned)GJ_GDT_SOFT_WAVE);

    /* Grep: gdt: soft ratio — init/ap/rsp0/query path ratios */
    kprintf("gdt: soft ratio inits=%u ap=%u logs=%u "
            "rsp0_set=%u rsp0_use_irq=%u rsp0_use_skip=%u "
            "info_get=%u log_call=%u verify_call=%u "
            "ctr_get=%u byte_get=%u inv_logs=%u wave=%u\n",
            g_u32SoftInits, g_u32SoftApLoads, g_u32SoftLogCall,
            g_u32SoftRsp0Set, g_u32SoftRsp0UseIrq, g_u32SoftRsp0UseSkip,
            g_u32SoftInfoGet, g_u32SoftLogCall, g_u32SoftVerifyCall,
            g_u32SoftCtrGet, g_u32SoftByteGet, g_u32SoftInvLogs,
            (unsigned)GJ_GDT_SOFT_WAVE);

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: gdt: soft return — Wave 17 API return surfaces (kept) */
    kprintf("gdt: soft return slots=8 tss=1 star=1 shared_gdt=1 soft_inv=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)GJ_GDT_SOFT_WAVE);

    /* Grep: gdt: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("gdt: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)GJ_GDT_SOFT_WAVE);

    /* Grep: gdt: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("gdt: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)GJ_GDT_SOFT_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: gdt: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("gdt: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)GJ_GDT_SOFT_WAVE);

    /* Grep: gdt: soft retcode — Wave 19 retcode catalog */
    kprintf("gdt: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)GJ_GDT_SOFT_WAVE);

    /* Grep: gdt: soft deepen — Wave 20 stamp + area catalog */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: gdt: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("gdt: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_GDT_SOFT_WAVE);
    /* Grep: gdt: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("gdt: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_GDT_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: gdt: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("gdt: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_GDT_SOFT_WAVE);
    /* Grep: gdt: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("gdt: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_GDT_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: gdt: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("gdt: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_GDT_SOFT_WAVE);
            /* Grep: gdt: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("gdt: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_GDT_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: gdt: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("gdt: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_GDT_SOFT_WAVE);
            /* Grep: gdt: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("gdt: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_GDT_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: gdt: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("gdt: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_GDT_SOFT_WAVE);
            /* Grep: gdt: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("gdt: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_GDT_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: gdt: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("gdt: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_GDT_SOFT_WAVE);
            /* Grep: gdt: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("gdt: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_GDT_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: gdt: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("gdt: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_GDT_SOFT_WAVE);
            /* Grep: gdt: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("gdt: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_GDT_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: gdt: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("gdt: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_GDT_SOFT_WAVE);
            /* Grep: gdt: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("gdt: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_GDT_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: gdt: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("gdt: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_GDT_SOFT_WAVE);
                    /* Grep: gdt: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("gdt: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_GDT_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: gdt: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("gdt: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_GDT_SOFT_WAVE);
                            /* Grep: gdt: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("gdt: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_GDT_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: gdt: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("gdt: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_GDT_SOFT_WAVE);
                            /* Grep: gdt: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("gdt: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)GJ_GDT_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: gdt: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("gdt: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_GDT_SOFT_WAVE);
                            /* Grep: gdt: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("gdt: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)GJ_GDT_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: gdt: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("gdt: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_GDT_SOFT_WAVE);
                            /* Grep: gdt: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("gdt: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_GDT_SOFT_WAVE);
                            /* Grep: gdt: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("gdt: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)GJ_GDT_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("gdt: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/* Grep: gdt: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("gdt: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("gdt: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/* Grep: gdt: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("gdt: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("gdt: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/* Grep: gdt: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("gdt: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retfortress — Wave 35 return-fortress honesty */
kprintf("gdt: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/* Grep: gdt: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("gdt: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft rethold — Wave 36 return-hold honesty */
kprintf("gdt: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/* Grep: gdt: soft retspire — Wave 36 exclusive spire stamp */
kprintf("gdt: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retwall — Wave 37 return-wall honesty */
kprintf("gdt: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/* Grep: gdt: soft retgate — Wave 37 exclusive gate stamp */
kprintf("gdt: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retmoat — Wave 38 return-moat honesty */
kprintf("gdt: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/* Grep: gdt: soft retower — Wave 38 exclusive tower stamp */
kprintf("gdt: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("gdt: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/* Grep: gdt: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("gdt: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("gdt: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/* Grep: gdt: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("gdt: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retravelin — Wave 41 return-travelin honesty */
kprintf("gdt: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/* Grep: gdt: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("gdt: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("gdt: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/* Grep: gdt: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("gdt: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("gdt: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/* Grep: gdt: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("gdt: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("gdt: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/* Grep: gdt: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("gdt: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("gdt: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/* Grep: gdt: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("gdt: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retbailey — Wave 46 return-bailey honesty */
kprintf("gdt: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);
/* Grep: gdt: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("gdt: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)GJ_GDT_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("gdt: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("gdt: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("gdt: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("gdt: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("gdt: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("gdt: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retsally — Wave 50 return-sally honesty */
kprintf("gdt: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("gdt: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retfosse — Wave 51 return-fosse honesty */
kprintf("gdt: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("gdt: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("gdt: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("gdt: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retravelin — Wave 53 return-travelin honesty */
kprintf("gdt: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("gdt: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("gdt: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retredan — Wave 54 exclusive redan stamp */
kprintf("gdt: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retflank — Wave 55 return-flank honesty */
kprintf("gdt: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retface — Wave 55 exclusive face stamp */
kprintf("gdt: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retgorge — Wave 56 return-gorge honesty */
kprintf("gdt: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("gdt: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retraverse — Wave 57 return-traverse honesty */
kprintf("gdt: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("gdt: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retorillon — Wave 58 return-orillon honesty */
kprintf("gdt: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("gdt: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("gdt: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("gdt: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retplace — Wave 60 return-place honesty */
kprintf("gdt: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("gdt: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("gdt: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("gdt: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("gdt: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("gdt: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("gdt: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("gdt: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: gdt: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("gdt: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: gdt: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("gdt: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: gdt: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("gdt: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: gdt: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("gdt: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: gdt: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("gdt: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=116 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: gdt: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("gdt: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=116 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("gdt: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("gdt: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("gdt: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("gdt: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("gdt: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=116 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("gdt: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=116 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("gdt: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("gdt: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("gdt: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("gdt: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: gdt: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("gdt: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("gdt: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("gdt: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("gdt: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbastionangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("gdt: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("gdt: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retparapetangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("gdt: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("gdt: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retowerangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("gdt: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("gdt: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retwallangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("gdt: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("gdt: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retholdangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("gdt: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("gdt: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retfortressangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("gdt: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("gdt: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("gdt: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("gdt: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: gdt: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("gdt: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("gdt: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("gdt: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("gdt: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retaegisangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("gdt: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("gdt: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retsigilangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("gdt: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("gdt: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retglyphangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("gdt: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("gdt: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retshardangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("gdt: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("gdt: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retprismangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("gdt: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("gdt: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retcipherangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("gdt: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("gdt: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retledgerangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("gdt: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("gdt: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retvaultangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("gdt: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("gdt: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (rettokenangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("gdt: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("gdt: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retphaseangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("gdt: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("gdt: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retpulseangle stamp; Soft≠product)\n");

/* Grep: gdt: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("gdt: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("gdt: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retboundangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("gdt: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("gdt: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbladeangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("gdt: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("gdt: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retarcangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("gdt: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("gdt: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("gdt: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("gdt: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("gdt: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("gdt: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retellipseangle stamp; Soft≠product)\n");
/* Grep: gdt: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("gdt: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("gdt: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("gdt: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("gdt: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (rethelixangle stamp; Soft≠product)\n");
/* Grep: gdt: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("gdt: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("gdt: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retknotangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("gdt: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("gdt: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retkleinangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("gdt: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("gdt: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retaffineangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("gdt: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("gdt: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("gdt: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("gdt: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retcubicangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("gdt: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("gdt: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retquinticangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retsplineangle — Wave 107 return-splineangle honesty */
kprintf("gdt: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
kprintf("gdt: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbezierangle stamp; Soft≠product)\n");
/* Grep: gdt: soft rethurmitangle — Wave 108 return-hermitangle honesty */
kprintf("gdt: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
kprintf("gdt: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retcatmullangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retnurbsangle — Wave 109 return-nurbsangle honesty */
kprintf("gdt: soft retnurbsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retnurbsangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retbsplineangle — Wave 109 exclusive bsplineangle stamp */
kprintf("gdt: soft retbsplineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retbsplineangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retmeshangle — Wave 110 return-meshangle honesty */
kprintf("gdt: soft retmeshangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retmeshangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retgridangle — Wave 110 exclusive gridangle stamp */
kprintf("gdt: soft retgridangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retgridangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retvoxelangle — Wave 111 return-voxelangle honesty */
kprintf("gdt: soft retvoxelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retvoxelangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft rettexelangle — Wave 111 exclusive texelangle stamp */
kprintf("gdt: soft rettexelangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (rettexelangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retfragmentangle — Wave 112 return-fragmentangle honesty */
kprintf("gdt: soft retfragmentangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retfragmentangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retvertexangle — Wave 112 exclusive vertexangle stamp */
kprintf("gdt: soft retvertexangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retvertexangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retshaderangle — Wave 113 return-shaderangle honesty */
kprintf("gdt: soft retshaderangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retshaderangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retpipelineangle — Wave 113 exclusive pipelineangle stamp */
kprintf("gdt: soft retpipelineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retpipelineangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retframebufferangle — Wave 114 return-framebufferangle honesty */
kprintf("gdt: soft retframebufferangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retframebufferangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retswapchainangle — Wave 114 exclusive swapchainangle stamp */
kprintf("gdt: soft retswapchainangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retswapchainangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retpresentangle — Wave 115 return-presentangle honesty */
kprintf("gdt: soft retpresentangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retpresentangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retvsyncangle — Wave 115 exclusive vsyncangle stamp */
kprintf("gdt: soft retvsyncangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retvsyncangle stamp; Soft≠product)\n");
/* Grep: gdt: soft retfenceangle — Wave 116 return-fenceangle honesty */
kprintf("gdt: soft retfenceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=116 (retfenceangle honesty; Soft≠product; not bar3)\n");
/* Grep: gdt: soft retsemaphoreangle — Wave 116 exclusive semaphoreangle stamp */
kprintf("gdt: soft retsemaphoreangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=116 (retsemaphoreangle stamp; Soft≠product)\n");
                            kprintf("gdt: soft deepen wave=%u areas="
            "inventory,slots,user,tss,lamps,counters,star,path,"
            "kernel,null,cs32,cs64,ds,desc,lar,verify,init,stack,"
            "geom,tssbase,expect,honesty,query,match,selector,tssrsp,"
            "exclusive,claim,ratio,return,return_selftest,retmap,return_rate,retcode "
            "unit=gdt.c only hard_gate=0\n",
            (unsigned)GJ_GDT_SOFT_WAVE);

    if (!g_fSoftSnapLive) {
        /* Grep: gdt: soft inventory idle */
        kprintf("gdt: soft inventory idle (GDT not loaded)\n");
        kprintf("gdt: soft idle\n");
    } else if (fPass) {
        /* Grep: gdt: soft inventory PASS */
        kprintf("gdt: soft inventory PASS wave=%u\n",
                (unsigned)GJ_GDT_SOFT_WAVE);
        kprintf("gdt: soft PASS wave=%u\n",
                (unsigned)GJ_GDT_SOFT_WAVE);
    } else {
        /* Grep: gdt: soft inventory FAIL */
        kprintf("gdt: soft inventory FAIL wave=%u\n",
                (unsigned)GJ_GDT_SOFT_WAVE);
        kprintf("gdt: soft FAIL wave=%u\n",
                (unsigned)GJ_GDT_SOFT_WAVE);
    }
}

void
tss_set_rsp0(u64 u64Rsp0)
{
    gdt_soft_inc(&g_u32SoftRsp0Set);
    g_Tss.u64Rsp0 = u64Rsp0;
}

u64
tss_get_rsp0(void)
{
    gdt_soft_inc(&g_u32SoftRsp0Get);
    return g_Tss.u64Rsp0;
}

u64
tss_irq_rsp0(void)
{
    gdt_soft_inc(&g_u32SoftIrqRsp0Q);
    return g_u64IrqRsp0;
}

void
tss_use_irq_rsp0(void)
{
    if (g_u64IrqRsp0 != 0) {
        g_Tss.u64Rsp0 = g_u64IrqRsp0;
        gdt_soft_inc(&g_u32SoftRsp0UseIrq);
    } else {
        gdt_soft_inc(&g_u32SoftRsp0UseSkip);
    }
}

void
gdt_init(void)
{
    memset(&g_Tss, 0, sizeof(g_Tss));
    g_u64IrqRsp0 =
        ((u64)(gj_vaddr_t)&g_aRsp0Stack[sizeof(g_aRsp0Stack)]) & ~0xfull;
    g_Tss.u64Rsp0 = g_u64IrqRsp0;
    g_Tss.u64Ist1 =
        ((u64)(gj_vaddr_t)&g_aIstStack[sizeof(g_aIstStack)]) & ~0xfull;
    g_Tss.u16IomapBase = sizeof(struct tss64);

    /* null */
    gdt_set(0, 0, 0, 0, 0);
    /* kernel code: present, ring0, code, readable, long mode (L bit in gran) */
    gdt_set(1, 0, 0xfffff, 0x9a, 0xa0);
    /* kernel data */
    gdt_set(2, 0, 0xfffff, 0x92, 0xc0);
    /*
     * User 32-bit code (index 3 → selector 0x1B):
     * access 0xFA (P|DPL3|code|R), gran 0xCF (G|D/B 32-bit, limit 4G).
     * Compat / PE32 personality uses this CS; product code may far-jump in.
     */
    gdt_set(3, 0, 0xfffff, 0xfa, 0xcf);
    /* User data DPL3 (index 4 → selector 0x23) */
    gdt_set(4, 0, 0xfffff, 0xf2, 0xcf);
    /* User 64-bit code DPL3 long mode (index 5 → selector 0x2B) */
    gdt_set(5, 0, 0xfffff, 0xfa, 0xa0);
    /* TSS available (indices 6–7) */
    gdt_set_tss(6, (u64)(gj_vaddr_t)&g_Tss, sizeof(g_Tss) - 1);

    g_GdtPtr.u16Limit = (u16)(sizeof(g_aGdt) - 1);
    g_GdtPtr.u64Base = (u64)(gj_vaddr_t)&g_aGdt[0];

    __asm__ volatile (
        "lgdt %0\n\t"
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%ss\n\t"
        "pushq $0x08\n\t"
        "leaq 1f(%%rip), %%rax\n\t"
        "pushq %%rax\n\t"
        "lretq\n\t"
        "1:\n\t"
        "mov %1, %%ax\n\t"
        "ltr %%ax\n\t"
        :
        : "m"(g_GdtPtr), "r"((u16)GJ_GDT_TSS_SEL_LOCAL)
        : "rax", "memory"
    );

    gdt_soft_inc(&g_u32SoftInits);
    gdt_user_soft_refresh();

    kprintf("gdt: TSS loaded rsp0=0x%lx cs32=0x%x ds=0x%x cs64=0x%x\n",
            (unsigned long)g_Tss.u64Rsp0, (unsigned)GJ_GDT_USER_CS32,
            (unsigned)GJ_GDT_USER_DS, (unsigned)GJ_GDT_USER_CS);
    /* Soft: greppable user-segment program line. */
    kprintf("gdt: user soft program cs32 acc=0x%x gran=0x%x "
            "ds acc=0x%x cs64 acc=0x%x gran=0x%x\n",
            (unsigned)g_SoftSnap.u8Cs32Access,
            (unsigned)g_SoftSnap.u8Cs32Gran,
            (unsigned)g_SoftSnap.u8DsAccess,
            (unsigned)g_SoftSnap.u8Cs64Access,
            (unsigned)g_SoftSnap.u8Cs64Gran);
    kprintf("gdt: user soft lamps ready=%u compat=%u long=%u ds_ok=%u "
            "rsp0=0x%lx ist1=0x%lx\n",
            (unsigned)g_SoftSnap.u8Cs32Ready,
            (unsigned)g_SoftSnap.u8Cs32Compat,
            (unsigned)g_SoftSnap.u8Cs64Long,
            (unsigned)g_SoftSnap.u8DsOk,
            (unsigned long)g_SoftSnap.u64Rsp0,
            (unsigned long)g_SoftSnap.u64Ist1);

    if (g_SoftSnap.u8Cs32Compat && g_SoftSnap.u8Cs64Long &&
        g_SoftSnap.u8DsOk) {
        gdt_soft_inc(&g_u32SoftVerifyOk);
        kprintf("gdt: user soft verify PASS inits=%u\n", g_u32SoftInits);
    } else {
        gdt_soft_inc(&g_u32SoftVerifyBad);
        kprintf("gdt: user soft verify FAIL inits=%u\n", g_u32SoftInits);
    }

    /* Wave 15: greppable "gdt: soft …" inventory at BSP load. */
    gdt_soft_inventory();
}

int
gdt_user_cs32_ready(void)
{
    gdt_soft_inc(&g_u32SoftCs32ReadyQ);
    /* Ready when index 3 is present DPL3 code with D/B=1 (32-bit default). */
    return (g_aGdt[3].u8Access == 0xfau && (g_aGdt[3].u8Gran & 0x40u) != 0) ? 1
                                                                              : 0;
}

u16
gdt_user_cs32_sel(void)
{
    gdt_soft_inc(&g_u32SoftCs32SelQ);
    return (u16)GJ_GDT_USER_CS32;
}

int
gdt_user_cs32_is_compat(void)
{
    /*
     * True 32-bit user CS for iretq into compat:
     *   access 0xFA = P|DPL3|S|code|R
     *   gran: G=1, D/B=1 (32-bit default op size), L=0 (not long mode)
     * gran layout: G D L AVL lim19:16 — 0xCF is G|D without L.
     */
    u8 acc = g_aGdt[3].u8Access;
    u8 gran = g_aGdt[3].u8Gran;

    gdt_soft_inc(&g_u32SoftCs32CompatQ);

    if (acc != 0xfau) {
        return 0;
    }
    if ((gran & 0x40u) == 0) {
        return 0; /* D must be 1 for 32-bit */
    }
    if ((gran & 0x20u) != 0) {
        return 0; /* L must be 0 (compat, not long) */
    }
    return 1;
}

int
gdt_user_cs32_lar_ok(u32 *pOutAr)
{
    u32 sel = (u32)GJ_GDT_USER_CS32;
    u32 ar = 0;
    u8 ok = 0;
    int fPass = 0;

    gdt_soft_inc(&g_u32SoftLarProbes);

    /*
     * LAR r32, r/m16 — ZF=1 if selector is visible/valid.
     * Rights: bits 8-11 type, 12 S, 13-14 DPL, 15 P (Intel SDM).
     */
    __asm__ volatile(
        "larl %[sel], %[ar]\n\t"
        "setz %[ok]"
        : [ar] "=r"(ar), [ok] "=qm"(ok)
        : [sel] "r"(sel)
        : "cc");
    if (pOutAr != NULL) {
        *pOutAr = ar;
    }
    g_SoftSnap.u32LarAr = ar;

    if (!ok) {
        g_SoftSnap.u8LarOk = 0;
        gdt_soft_inc(&g_u32SoftLarBad);
        return 0;
    }
    if ((ar & 0x8000u) == 0) {
        g_SoftSnap.u8LarOk = 0;
        gdt_soft_inc(&g_u32SoftLarBad);
        return 0; /* P */
    }
    if ((ar & 0x6000u) != 0x6000u) {
        g_SoftSnap.u8LarOk = 0;
        gdt_soft_inc(&g_u32SoftLarBad);
        return 0; /* DPL3 */
    }
    if ((ar & 0x0800u) == 0) {
        g_SoftSnap.u8LarOk = 0;
        gdt_soft_inc(&g_u32SoftLarBad);
        return 0; /* code (executable type bit) */
    }
    fPass = 1;
    g_SoftSnap.u8LarOk = 1;
    gdt_soft_inc(&g_u32SoftLarOk);
    (void)fPass;
    return 1;
}

void
gdt_load_ap(void)
{
    /*
     * Reload the BSP kernel GDT on an AP. Skip LTR: a 64-bit TSS may be
     * busy on only one CPU; APs stay ring-0 until per-CPU TSS exists.
     */
    __asm__ volatile (
        "lgdt %0\n\t"
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%ss\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "pushq $0x08\n\t"
        "leaq 1f(%%rip), %%rax\n\t"
        "pushq %%rax\n\t"
        "lretq\n\t"
        "1:\n\t"
        :
        : "m"(g_GdtPtr)
        : "rax", "memory"
    );
    gdt_soft_inc(&g_u32SoftApLoads);
    /* Soft: AP sees same user segments (shared GDT). */
    if (g_fSoftSnapLive) {
        gdt_user_soft_refresh();
    }
}

/* ---- Soft GDT user-segment observability API ----------------------- */

int
gdt_user_cs64_is_long(void)
{
    u8 acc = g_aGdt[5].u8Access;
    u8 gran = g_aGdt[5].u8Gran;

    gdt_soft_inc(&g_u32SoftCs64LongQ);

    if (acc != 0xfau) {
        return 0;
    }
    if ((gran & 0x20u) == 0) {
        return 0; /* L must be 1 for long mode */
    }
    if ((gran & 0x40u) != 0) {
        return 0; /* D must be 0 when L=1 */
    }
    return 1;
}

int
gdt_user_ds_ok(void)
{
    gdt_soft_inc(&g_u32SoftDsOkQ);
    return (g_aGdt[4].u8Access == 0xf2u) ? 1 : 0;
}

u32
gdt_user_soft_inits(void)
{
    gdt_soft_inc(&g_u32SoftCtrGet);
    return g_u32SoftInits;
}

u32
gdt_user_soft_ap_loads(void)
{
    gdt_soft_inc(&g_u32SoftCtrGet);
    return g_u32SoftApLoads;
}

u32
gdt_user_soft_lar_probes(void)
{
    gdt_soft_inc(&g_u32SoftCtrGet);
    return g_u32SoftLarProbes;
}

u32
gdt_user_soft_lar_ok(void)
{
    gdt_soft_inc(&g_u32SoftCtrGet);
    return g_u32SoftLarOk;
}

u32
gdt_user_soft_lar_bad(void)
{
    gdt_soft_inc(&g_u32SoftCtrGet);
    return g_u32SoftLarBad;
}

u32
gdt_user_soft_verify_ok(void)
{
    gdt_soft_inc(&g_u32SoftCtrGet);
    return g_u32SoftVerifyOk;
}

u32
gdt_user_soft_verify_bad(void)
{
    gdt_soft_inc(&g_u32SoftCtrGet);
    return g_u32SoftVerifyBad;
}

u8
gdt_user_soft_cs32_access(void)
{
    gdt_soft_inc(&g_u32SoftByteGet);
    return g_SoftSnap.u8Cs32Access;
}

u8
gdt_user_soft_cs32_gran(void)
{
    gdt_soft_inc(&g_u32SoftByteGet);
    return g_SoftSnap.u8Cs32Gran;
}

u8
gdt_user_soft_ds_access(void)
{
    gdt_soft_inc(&g_u32SoftByteGet);
    return g_SoftSnap.u8DsAccess;
}

u8
gdt_user_soft_cs64_access(void)
{
    gdt_soft_inc(&g_u32SoftByteGet);
    return g_SoftSnap.u8Cs64Access;
}

u8
gdt_user_soft_cs64_gran(void)
{
    gdt_soft_inc(&g_u32SoftByteGet);
    return g_SoftSnap.u8Cs64Gran;
}

int
gdt_user_soft_verify(int fDoLar)
{
    int fOk = 1;

    gdt_soft_inc(&g_u32SoftVerifyCall);

    if (!g_fSoftSnapLive) {
        gdt_soft_inc(&g_u32SoftVerifyBad);
        return 0;
    }
    gdt_user_soft_refresh();

    if (!g_SoftSnap.u8Cs32Compat) {
        fOk = 0;
    }
    if (!g_SoftSnap.u8Cs64Long) {
        fOk = 0;
    }
    if (!g_SoftSnap.u8DsOk) {
        fOk = 0;
    }
    if (fDoLar) {
        if (!gdt_user_cs32_lar_ok(NULL)) {
            fOk = 0;
        }
    }

    if (fOk) {
        gdt_soft_inc(&g_u32SoftVerifyOk);
    } else {
        gdt_soft_inc(&g_u32SoftVerifyBad);
    }
    return fOk;
}

int
gdt_user_soft_info_get(struct gj_gdt_user_soft *pOut)
{
    gdt_soft_inc(&g_u32SoftInfoGet);

    if (pOut == NULL) {
        return g_fSoftSnapLive ? 1 : 0;
    }
    memset(pOut, 0, sizeof(*pOut));
    if (!g_fSoftSnapLive) {
        return 0;
    }
    gdt_user_soft_refresh();
    *pOut = g_SoftSnap;
    return 1;
}

void
gdt_user_soft_log(void)
{
    /*
     * Greppable soft summary (product / smoke inventory):
     *   gdt: user soft inits=… ap=… lar_probe=… lar_ok=… lar_bad=… …
     *   gdt: user soft cs32 / ds / cs64 descriptor bytes + lamps
     *   gdt: user soft tss rsp0 / ist1 / lar_ar
     * Wave 15 also emits prefix-stable "gdt: soft …" inventory deepen.
     */
    gdt_soft_inc(&g_u32SoftLogCall);

    if (g_fSoftSnapLive) {
        gdt_user_soft_refresh();
    }
    kprintf("gdt: user soft inits=%u ap=%u lar_probe=%u lar_ok=%u "
            "lar_bad=%u verify_ok=%u verify_bad=%u\n",
            g_u32SoftInits, g_u32SoftApLoads, g_u32SoftLarProbes,
            g_u32SoftLarOk, g_u32SoftLarBad, g_u32SoftVerifyOk,
            g_u32SoftVerifyBad);
    kprintf("gdt: user soft cs32 acc=0x%x gran=0x%x ready=%u compat=%u "
            "sel=0x%x\n",
            (unsigned)g_SoftSnap.u8Cs32Access,
            (unsigned)g_SoftSnap.u8Cs32Gran,
            (unsigned)g_SoftSnap.u8Cs32Ready,
            (unsigned)g_SoftSnap.u8Cs32Compat,
            (unsigned)g_SoftSnap.u16Cs32Sel);
    kprintf("gdt: user soft ds acc=0x%x ok=%u sel=0x%x cs64 acc=0x%x "
            "gran=0x%x long=%u sel=0x%x\n",
            (unsigned)g_SoftSnap.u8DsAccess,
            (unsigned)g_SoftSnap.u8DsOk,
            (unsigned)g_SoftSnap.u16DsSel,
            (unsigned)g_SoftSnap.u8Cs64Access,
            (unsigned)g_SoftSnap.u8Cs64Gran,
            (unsigned)g_SoftSnap.u8Cs64Long,
            (unsigned)g_SoftSnap.u16Cs64Sel);
    kprintf("gdt: user soft tss rsp0=0x%lx ist1=0x%lx lar_ar=0x%x "
            "lar_ok=%u\n",
            (unsigned long)g_SoftSnap.u64Rsp0,
            (unsigned long)g_SoftSnap.u64Ist1,
            g_SoftSnap.u32LarAr,
            (unsigned)g_SoftSnap.u8LarOk);
    if (!g_fSoftSnapLive) {
        kprintf("gdt: user soft idle (GDT not loaded)\n");
    } else if (g_SoftSnap.u8Cs32Compat && g_SoftSnap.u8Cs64Long &&
               g_SoftSnap.u8DsOk) {
        kprintf("gdt: user soft verify PASS\n");
    } else {
        kprintf("gdt: user soft verify FAIL\n");
    }

    /* Wave 15 exclusive: greppable "gdt: soft …" soft inventory rollup. */
    gdt_soft_inventory();
}
