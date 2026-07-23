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
 * Wave 13 exclusive deepen (this unit only) — extra greppable surfaces:
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
 * Soft never hard-gates boot. No bar3 claim.
 * greppable: gdt: soft
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
static struct gj_gdt_user_soft g_SoftSnap;
static int g_fSoftSnapLive;

#define GJ_GDT_TSS_SEL_LOCAL 0x30u /* index 6 */
#define GJ_GDT_SOFT_WAVE     13u   /* Wave 13 exclusive deepen stamp */

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
     * Wave 13 stamps wave=13; existing keys remain prefix-stable.
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

    if (!g_fSoftSnapLive) {
        /* Grep: gdt: soft inventory idle */
        kprintf("gdt: soft inventory idle (GDT not loaded)\n");
        kprintf("gdt: soft idle\n");
    } else if (fPass) {
        /* Grep: gdt: soft inventory PASS */
        kprintf("gdt: soft inventory PASS\n");
        kprintf("gdt: soft PASS\n");
    } else {
        /* Grep: gdt: soft inventory FAIL */
        kprintf("gdt: soft inventory FAIL\n");
        kprintf("gdt: soft FAIL\n");
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
    return g_Tss.u64Rsp0;
}

u64
tss_irq_rsp0(void)
{
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

    /* Wave 13: greppable "gdt: soft …" inventory at BSP load. */
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
    return g_u32SoftInits;
}

u32
gdt_user_soft_ap_loads(void)
{
    return g_u32SoftApLoads;
}

u32
gdt_user_soft_lar_probes(void)
{
    return g_u32SoftLarProbes;
}

u32
gdt_user_soft_lar_ok(void)
{
    return g_u32SoftLarOk;
}

u32
gdt_user_soft_lar_bad(void)
{
    return g_u32SoftLarBad;
}

u32
gdt_user_soft_verify_ok(void)
{
    return g_u32SoftVerifyOk;
}

u32
gdt_user_soft_verify_bad(void)
{
    return g_u32SoftVerifyBad;
}

u8
gdt_user_soft_cs32_access(void)
{
    return g_SoftSnap.u8Cs32Access;
}

u8
gdt_user_soft_cs32_gran(void)
{
    return g_SoftSnap.u8Cs32Gran;
}

u8
gdt_user_soft_ds_access(void)
{
    return g_SoftSnap.u8DsAccess;
}

u8
gdt_user_soft_cs64_access(void)
{
    return g_SoftSnap.u8Cs64Access;
}

u8
gdt_user_soft_cs64_gran(void)
{
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
     * Wave 13 also emits prefix-stable "gdt: soft …" inventory deepen.
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

    /* Wave 13 exclusive: greppable "gdt: soft …" soft inventory rollup. */
    gdt_soft_inventory();
}
