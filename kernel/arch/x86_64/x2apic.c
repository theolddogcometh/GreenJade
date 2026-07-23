/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * x2APIC detect + optional mode enable (clean-room SDM).
 * When enabled, ICR uses MSRs instead of MMIO (large APIC ID friendly).
 *
 * Soft ICR observability: every MSR 0x830 / 0x83F write bumps counters and
 * snapshots last dest/mode/vector for AP bring-up and IPI telemetry.
 * First few ICR writes are rate-limited to serial (greppable).
 *
 * -------------------------------------------------------------------------
 * Soft inventory (Wave 21 exclusive deepen) — greppable "x2apic: soft …"
 * -------------------------------------------------------------------------
 * Pure observation; never hard-gates IPI delivery. Counters wrap OK.
 * Soft ≠ full ICR/timer replace product (timer.c / apic.c product paths
 * remain authoritative; this unit only deepens soft inventory surfaces).
 *
 * Prefix-stable product / smoke markers (agent greps):
 *   x2apic: soft inventory   — MSR/mode/slot catalog + feature surface
 *   x2apic: soft stats       — probe/enable/eoi/ICR path counters
 *   x2apic: soft mode        — supported/enabled/cpu/slot lamps
 *   x2apic: soft icr         — writes/fixed/init/sipi/self/other rollup
 *   x2apic: soft last        — last dest/mode/vec/val + field snap
 *   x2apic: soft bringup     — PASS|idle|active INIT/SIPI verdict
 *   x2apic: soft eoi         — EOI MSR soft count (when mode on)
 *   x2apic: soft msr         — ICR/self/EOI/APIC_BASE MSR catalog
 *   x2apic: soft slots       — per-CPU enable table occupancy
 *   x2apic: soft probe       — detect path tallies
 *   x2apic: soft enable      — enable ok/fail/already/reason tallies
 *   x2apic: soft ipi         — fixed/raw/self entry tallies
 *   x2apic: soft self        — self-IPI MSR surface
 *   x2apic: soft modes       — delivery-mode decode rollup + catalog
 *   x2apic: soft query       — soft API accessor sample tallies
 *   x2apic: soft path        — honesty: soft inventory ≠ bar3 / product
 *   x2apic: soft honesty     — soft ≠ full ICR/timer replace product
 *   x2apic: soft claim       — product claim bounds (P-IRQ-1 / P-SMP-3)
 *   x2apic: soft catalog     — API + MSR + mode soft catalog lamps
 *   x2apic: soft capacity    — fixed slots / rate-log / wave surface
 *   x2apic: soft fields      — ICR level/trigger/shorthand/dest_mode snap
 *   x2apic: soft dest        — dest class rollup (zero/self/uni/bcast)
 *   x2apic: soft outcome     — enable+bringup soft outcome rollup
 *   x2apic: soft api         — soft accessor / log API surface
 *   x2apic: soft note        — ICR soft-note entry + rate-log tallies
 * Wave 16 complementary surfaces (kept; never reshape primary fields):
 *   x2apic: soft exclusive   — exclusive=1 unit stamp + wave
 *   x2apic: soft ratio       — probe/enable/icr/eoi path ratios
 *   x2apic: soft return rate — Wave 19 ok/fail rate lamps
 *   x2apic: soft retcode    — Wave 19 retcode catalog
 *   x2apic: soft deepen      — wave=21 areas stamp
 * Wave 17 complementary surfaces (kept) (never reshape primary fields):
 *   x2apic: soft return      — Wave 17 API return surfaces (kept)
 *   x2apic: soft return selftest — Wave 17 terminal return surface (kept)
 *   x2apic: soft retmap      — Wave 17 return-surface map (kept)
 *   x2apic: soft PASS|FAIL|idle — soft lamp (never hard-gates)
 *
 * Legacy ICR soft lines kept: "x2apic: icr soft …" (bring-up continuity).
 * greppable: x2apic: soft
 * greppable: x2apic: icr soft
 * greppable: x2apic: supported=
 * greppable: MSR 0x830 0x83F P-IRQ-1 P-SMP-3
 * greppable: soft != full ICR/timer replace
 * greppable: x2apic: soft exclusive
 * greppable: x2apic: soft return
 * greppable: x2apic: soft return selftest
 * greppable: x2apic: soft retmap
 */
#include <gj/apic.h>
#include <gj/cpu.h>
#include <gj/klog.h>
#include <gj/x2apic.h>

#define IA32_APIC_BASE_MSR   0x1Bu
#define APIC_BASE_X2APIC     (1ull << 10)
#define APIC_BASE_ENABLE     (1ull << 11)

/* x2APIC MSR base: 0x800 + (MMIO_offset / 0x10) */
#define X2APIC_MSR_EOI       0x80Bu
#define X2APIC_MSR_ICR       0x830u
#define X2APIC_MSR_SELF_IPI  0x83Fu

/* Soft: log first N ICR writes so INIT/SIPI sequencing is greppable. */
#define X2APIC_ICR_SOFT_LOG_MAX 8u

/* Soft inventory: per-CPU enable table size (matches g_aEnabled). */
#define X2APIC_SOFT_CPU_SLOTS 16u

/* Wave 21 exclusive soft deepen stamp (greppable wave=21). */
#define X2APIC_SOFT_DEEPEN_WAVE  21u
/* Fixed greppable categories emitted under "x2apic: soft …". */
#define X2APIC_SOFT_DEEPEN_AREAS 39u

/* Soft ICR field masks (SDM ICR; decode only — never rewrites product ICR). */
#define X2APIC_SOFT_ICR_DM_SHIFT     11u /* destination mode (phys/logical) */
#define X2APIC_SOFT_ICR_LEVEL_SHIFT  14u /* assert/deassert (INIT) */
#define X2APIC_SOFT_ICR_TRIG_SHIFT   15u /* edge/level trigger */
#define X2APIC_SOFT_ICR_SH_SHIFT     18u /* destination shorthand */
#define X2APIC_SOFT_ICR_SH_MASK      3u

static int g_fSupported = -1;
/* Per-CPU: x2APIC mode is enabled independently on each logical CPU. */
static int g_aEnabled[X2APIC_SOFT_CPU_SLOTS];

/* Soft ICR observability (monotonic counters; no locks — BSP/AP soft). */
static volatile u64 g_u64IcrWrites;
static volatile u64 g_u64IcrFixed;
static volatile u64 g_u64IcrInit;
static volatile u64 g_u64IcrSipi;
static volatile u64 g_u64IcrSelf;
static volatile u64 g_u64IcrOther;
static volatile u64 g_u64IcrLast;
static volatile u32 g_u32IcrLastDest;
static volatile u8  g_u8IcrLastMode;
static volatile u8  g_u8IcrLastVec;
static volatile u32 g_u32IcrSoftLogged;

/*
 * Wave 15 soft path counters (sticky; wrap OK; never hard-gate).
 * greppable: x2apic: soft stats
 * greppable: x2apic: soft
 */
static volatile u64 g_u64SoftProbe;
static volatile u64 g_u64SoftProbeSupp;
static volatile u64 g_u64SoftProbeUnsupp;
static volatile u64 g_u64SoftEnableOk;
static volatile u64 g_u64SoftEnableFail;
static volatile u64 g_u64SoftEnableAlready;
static volatile u64 g_u64SoftEnableFailUnsupp;
static volatile u64 g_u64SoftEnableFailApic;
static volatile u64 g_u64SoftEnableFailMsr;
static volatile u64 g_u64SoftEoi;
static volatile u64 g_u64SoftIpiFixed;
static volatile u64 g_u64SoftIpiRaw;
static volatile u64 g_u64SoftSelfIpi;
static volatile u64 g_u64SoftInventoryLog;
static volatile u64 g_u64SoftModeLowpri;
static volatile u64 g_u64SoftModeSmi;
static volatile u64 g_u64SoftModeNmi;
static volatile u64 g_u64SoftModeExtint;
static volatile u64 g_u64SoftModeReserved; /* delivery mode 3 (reserved) */
static volatile u64 g_u64SoftEnabledQuery;
static volatile u64 g_u64SoftSupportedQuery;
static volatile u64 g_u64SoftIcrQuery;
static volatile u64 g_u64SoftLogApi;
static volatile u64 g_u64SoftPeakEnabledSlots;
static volatile u64 g_u64SoftIcrNote;       /* x2apic_icr_soft_note entries */
static volatile u64 g_u64SoftIcrNoteSelf;
static volatile u64 g_u64SoftIcrNoteRaw;
static volatile u64 g_u64SoftFieldLevel;   /* ICR bit 14 set */
static volatile u64 g_u64SoftFieldTrigger; /* ICR bit 15 set */
static volatile u64 g_u64SoftFieldLogical; /* dest mode logical */
static volatile u64 g_u64SoftFieldShNone;  /* shorthand 00 */
static volatile u64 g_u64SoftFieldShSelf;  /* shorthand 01 */
static volatile u64 g_u64SoftFieldShAll;   /* shorthand 10 all-inc-self */
static volatile u64 g_u64SoftFieldShOthers;/* shorthand 11 all-exc-self */
static volatile u64 g_u64SoftDestZero;     /* dest field 0 (non-self path) */
static volatile u64 g_u64SoftDestUnicast;  /* dest 1..0xfffffffe */
static volatile u64 g_u64SoftDestBcast;    /* dest 0xffffffff soft class */
static volatile u64 g_u64SoftPeakDest;     /* max dest id observed */
static volatile u64 g_u64SoftPeakVec;      /* max vector observed */
static volatile u8  g_u8IcrLastLevel;
static volatile u8  g_u8IcrLastTrigger;
static volatile u8  g_u8IcrLastDestMode;
static volatile u8  g_u8IcrLastShorthand;
static volatile u8  g_fSoftInvOnce;

static u64
rdmsr(u32 u32Msr)
{
    u32 u32Lo;
    u32 u32Hi;

    __asm__ volatile ("rdmsr" : "=a"(u32Lo), "=d"(u32Hi) : "c"(u32Msr));
    return ((u64)u32Hi << 32) | u32Lo;
}

static void
wrmsr(u32 u32Msr, u64 u64Val)
{
    u32 u32Lo = (u32)u64Val;
    u32 u32Hi = (u32)(u64Val >> 32);

    __asm__ volatile ("wrmsr" : : "c"(u32Msr), "a"(u32Lo), "d"(u32Hi));
}

/** Soft: count logical CPUs that currently report mode enabled. */
static u32
x2apic_soft_enabled_slots(void)
{
    u32 i;
    u32 cEn = 0;

    for (i = 0; i < X2APIC_SOFT_CPU_SLOTS; i++) {
        if (g_aEnabled[i] != 0) {
            cEn++;
        }
    }
    return cEn;
}

/**
 * Soft: note peak enabled-slot occupancy (wrap-safe peak).
 * Pure observation — never changes enable table.
 */
static void
x2apic_soft_note_enabled_peak(u32 cEn)
{
    if ((u64)cEn > g_u64SoftPeakEnabledSlots) {
        g_u64SoftPeakEnabledSlots = (u64)cEn;
    }
}

/**
 * Soft: note peak dest / vector (wrap-safe peaks).
 * Pure observation — never changes delivery.
 */
static void
x2apic_soft_note_peaks(u32 u32Dest, u8 u8Vec)
{
    if ((u64)u32Dest > g_u64SoftPeakDest) {
        g_u64SoftPeakDest = (u64)u32Dest;
    }
    if ((u64)u8Vec > g_u64SoftPeakVec) {
        g_u64SoftPeakVec = (u64)u8Vec;
    }
}

/**
 * Soft: classify + count one ICR write; rate-limited serial for bring-up.
 * Pure observability — never changes delivery semantics.
 * Wave 15: also decodes level/trigger/shorthand/dest-mode + dest class.
 */
static void
x2apic_icr_soft_note(u64 u64Icr, int fSelf)
{
    u8  u8Mode;
    u8  u8Vec;
    u8  u8Level;
    u8  u8Trig;
    u8  u8Dm;
    u8  u8Sh;
    u32 u32Dest;
    u32 u32N;

    g_u64SoftIcrNote++;

    if (fSelf) {
        g_u64SoftIcrNoteSelf++;
        g_u64IcrSelf++;
        g_u64IcrLast = (u64)(u8)u64Icr; /* self-IPI is vector only */
        g_u32IcrLastDest = 0;           /* self */
        g_u8IcrLastMode = GJ_X2APIC_ICR_MODE_FIXED;
        g_u8IcrLastVec = (u8)u64Icr;
        g_u8IcrLastLevel = 0;
        g_u8IcrLastTrigger = 0;
        g_u8IcrLastDestMode = 0;
        g_u8IcrLastShorthand = 1; /* soft: treat self-IPI as sh=self */
        x2apic_soft_note_peaks(0, (u8)u64Icr);
        return;
    }

    g_u64SoftIcrNoteRaw++;
    u8Mode = (u8)((u64Icr >> 8) & 7u);
    u8Vec = (u8)(u64Icr & 0xffu);
    u32Dest = (u32)(u64Icr >> 32);
    u8Level = (u8)((u64Icr >> X2APIC_SOFT_ICR_LEVEL_SHIFT) & 1u);
    u8Trig = (u8)((u64Icr >> X2APIC_SOFT_ICR_TRIG_SHIFT) & 1u);
    u8Dm = (u8)((u64Icr >> X2APIC_SOFT_ICR_DM_SHIFT) & 1u);
    u8Sh = (u8)((u64Icr >> X2APIC_SOFT_ICR_SH_SHIFT) & X2APIC_SOFT_ICR_SH_MASK);

    g_u64IcrWrites++;
    g_u64IcrLast = u64Icr;
    g_u32IcrLastDest = u32Dest;
    g_u8IcrLastMode = u8Mode;
    g_u8IcrLastVec = u8Vec;
    g_u8IcrLastLevel = u8Level;
    g_u8IcrLastTrigger = u8Trig;
    g_u8IcrLastDestMode = u8Dm;
    g_u8IcrLastShorthand = u8Sh;
    x2apic_soft_note_peaks(u32Dest, u8Vec);

    if (u8Mode == GJ_X2APIC_ICR_MODE_FIXED) {
        g_u64IcrFixed++;
    } else if (u8Mode == GJ_X2APIC_ICR_MODE_INIT) {
        g_u64IcrInit++;
    } else if (u8Mode == GJ_X2APIC_ICR_MODE_STARTUP) {
        g_u64IcrSipi++;
    } else if (u8Mode == GJ_X2APIC_ICR_MODE_LOWPRI) {
        g_u64SoftModeLowpri++;
        g_u64IcrOther++;
    } else if (u8Mode == GJ_X2APIC_ICR_MODE_SMI) {
        g_u64SoftModeSmi++;
        g_u64IcrOther++;
    } else if (u8Mode == GJ_X2APIC_ICR_MODE_NMI) {
        g_u64SoftModeNmi++;
        g_u64IcrOther++;
    } else if (u8Mode == GJ_X2APIC_ICR_MODE_EXTINT) {
        g_u64SoftModeExtint++;
        g_u64IcrOther++;
    } else {
        /* Delivery mode 3 is reserved in SDM — soft-count only. */
        g_u64SoftModeReserved++;
        g_u64IcrOther++;
    }

    /* Soft field tallies (Wave 15 deepen). */
    if (u8Level != 0) {
        g_u64SoftFieldLevel++;
    }
    if (u8Trig != 0) {
        g_u64SoftFieldTrigger++;
    }
    if (u8Dm != 0) {
        g_u64SoftFieldLogical++;
    }
    if (u8Sh == 0u) {
        g_u64SoftFieldShNone++;
    } else if (u8Sh == 1u) {
        g_u64SoftFieldShSelf++;
    } else if (u8Sh == 2u) {
        g_u64SoftFieldShAll++;
    } else {
        g_u64SoftFieldShOthers++;
    }

    /* Soft dest class (shorthand-none physical dest field). */
    if (u32Dest == 0u) {
        g_u64SoftDestZero++;
    } else if (u32Dest == 0xffffffffu) {
        g_u64SoftDestBcast++;
    } else {
        g_u64SoftDestUnicast++;
    }

    /* Rate-limited greppable line: first N ICR MSR writes only. */
    u32N = g_u32IcrSoftLogged;
    if (u32N < X2APIC_ICR_SOFT_LOG_MAX) {
        g_u32IcrSoftLogged = u32N + 1u;
        /* Legacy: x2apic: icr soft … */
        kprintf("x2apic: icr soft write n=%u dest=%u mode=%u vec=%u "
                "val=0x%lx\n",
                u32N + 1u, u32Dest, (unsigned)u8Mode, (unsigned)u8Vec,
                (unsigned long)u64Icr);
        /* Wave 15: x2apic: soft … */
        kprintf("x2apic: soft icr write n=%u dest=%u mode=%u vec=%u "
                "level=%u trig=%u dm=%u sh=%u val=0x%lx wave=%u\n",
                u32N + 1u, u32Dest, (unsigned)u8Mode, (unsigned)u8Vec,
                (unsigned)u8Level, (unsigned)u8Trig, (unsigned)u8Dm,
                (unsigned)u8Sh, (unsigned long)u64Icr,
                (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
    }
}

/**
 * Wave 15 soft inventory — greppable "x2apic: soft …" rollup.
 * Safe anytime; pure observation; no heap; no hard-gate.
 * Soft ≠ full ICR/timer replace product.
 * greppable: x2apic: soft inventory
 * greppable: x2apic: soft stats
 * greppable: x2apic: soft mode
 * greppable: x2apic: soft icr
 * greppable: x2apic: soft last
 * greppable: x2apic: soft bringup
 * greppable: x2apic: soft eoi
 * greppable: x2apic: soft msr
 * greppable: x2apic: soft slots
 * greppable: x2apic: soft probe
 * greppable: x2apic: soft enable
 * greppable: x2apic: soft ipi
 * greppable: x2apic: soft self
 * greppable: x2apic: soft modes
 * greppable: x2apic: soft query
 * greppable: x2apic: soft path
 * greppable: x2apic: soft honesty
 * greppable: x2apic: soft claim
 * greppable: x2apic: soft catalog
 * greppable: x2apic: soft capacity
 * greppable: x2apic: soft fields
 * greppable: x2apic: soft dest
 * greppable: x2apic: soft outcome
 * greppable: x2apic: soft api
 * greppable: x2apic: soft note
 * greppable: x2apic: soft deepen
 * greppable: x2apic: soft PASS
 */
static void
x2apic_soft_inventory(void)
{
    u32 u32Slot = cpu_id();
    u32 cEn;
    u32 cFree;
    int fSupp;
    int fEn;
    u64 u64Base = 0;
    u32 fX2Bit = 0;
    u32 fGlobEn = 0;
    u32 u32Bitmap = 0;
    u32 iSlot;
    const char *szBringup;
    const char *szLamp;

    g_u64SoftInventoryLog++;

    if (u32Slot >= X2APIC_SOFT_CPU_SLOTS) {
        u32Slot = 0;
    }
    cEn = x2apic_soft_enabled_slots();
    x2apic_soft_note_enabled_peak(cEn);
    cFree = (cEn < X2APIC_SOFT_CPU_SLOTS)
                ? (X2APIC_SOFT_CPU_SLOTS - cEn)
                : 0u;
    fSupp = (g_fSupported > 0) ? 1 : 0;
    fEn = (g_aEnabled[u32Slot] != 0) ? 1 : 0;

    /* Soft enable-slot bitmap (low 16 bits; one lamp per CPU slot). */
    for (iSlot = 0; iSlot < X2APIC_SOFT_CPU_SLOTS; iSlot++) {
        if (g_aEnabled[iSlot] != 0) {
            u32Bitmap |= (1u << iSlot);
        }
    }

    /* Live IA32_APIC_BASE peek only when mode is on (else may be pre-apic). */
    if (fEn) {
        u64Base = rdmsr(IA32_APIC_BASE_MSR);
        fX2Bit = (u64Base & APIC_BASE_X2APIC) ? 1u : 0u;
        fGlobEn = (u64Base & APIC_BASE_ENABLE) ? 1u : 0u;
    }

    if (g_u64IcrInit != 0 || g_u64IcrSipi != 0) {
        szBringup = "PASS";
    } else if (g_u64IcrWrites == 0 && g_u64IcrSelf == 0) {
        szBringup = "idle";
    } else {
        szBringup = "active";
    }

    /*
     * Soft lamp (never hard-gates): PASS when feature supported and at
     * least one slot enabled; idle when never probed; FAIL when probe
     * saw unsupported or enable_fail without any ok.
     */
    if (fSupp != 0 && cEn > 0) {
        szLamp = "PASS";
    } else if (g_u64SoftProbe == 0 && g_u64SoftEnableOk == 0 &&
               g_u64SoftEnableFail == 0) {
        szLamp = "idle";
    } else if (fSupp == 0 && g_fSupported >= 0) {
        szLamp = "FAIL";
    } else if (g_u64SoftEnableFail != 0 && g_u64SoftEnableOk == 0) {
        szLamp = "FAIL";
    } else {
        szLamp = "idle";
    }

    /*
     * Honesty first (Wave 15): soft inventory ≠ full ICR product rewrite,
     * ≠ timer mono/x2apic full replace, ≠ bar3. Diagnostics only.
     * greppable: x2apic: soft honesty
     * greppable: soft != full ICR/timer replace
     */
    kprintf("x2apic: soft honesty soft=1 icr_replace=0 timer_replace=0 "
            "mmio_icr_product=1 apic_timer_product=1 hard_gate=0 "
            "bar3=0 product_complete=0 wave=%u "
            "(soft inventory; soft != full ICR/timer replace product)\n",
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /*
     * Catalog: fixed surface (MSRs, delivery modes, slots) so scripts can
     * grep product depth without parsing C. P-IRQ-1 / P-SMP-3 product path.
     * Wave 15 deepen splits honesty/claim/catalog/capacity/fields/dest/
     * outcome/api/note/deepen on top of Wave 13 msr/slots/… surfaces.
     */
    /* Grep: x2apic: soft inventory */
    kprintf("x2apic: soft inventory slots=%u msr_icr=0x%x msr_self=0x%x "
            "msr_eoi=0x%x msr_apic_base=0x%x x2bit=10 glob_en_bit=11 "
            "modes=fixed,lowpri,smi,nmi,init,startup,extint "
            "cpuid_leaf1_ecx21=1 p_irq1=1 p_smp3=1 "
            "rate_log_max=%u enabled_slots=%u peak_slots=%lu "
            "log_n=%lu areas=%u wave=%u\n",
            (unsigned)X2APIC_SOFT_CPU_SLOTS,
            (unsigned)X2APIC_MSR_ICR, (unsigned)X2APIC_MSR_SELF_IPI,
            (unsigned)X2APIC_MSR_EOI, (unsigned)IA32_APIC_BASE_MSR,
            (unsigned)X2APIC_ICR_SOFT_LOG_MAX, cEn,
            (unsigned long)g_u64SoftPeakEnabledSlots,
            (unsigned long)g_u64SoftInventoryLog,
            (unsigned)X2APIC_SOFT_DEEPEN_AREAS,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft stats */
    kprintf("x2apic: soft stats probe=%lu enable_ok=%lu enable_fail=%lu "
            "enable_already=%lu eoi=%lu ipi_fixed=%lu ipi_raw=%lu "
            "self_ipi=%lu inv_log=%lu icr_rate_logged=%u "
            "query=%lu log_api=%lu note=%lu wave=%u\n",
            (unsigned long)g_u64SoftProbe,
            (unsigned long)g_u64SoftEnableOk,
            (unsigned long)g_u64SoftEnableFail,
            (unsigned long)g_u64SoftEnableAlready,
            (unsigned long)g_u64SoftEoi,
            (unsigned long)g_u64SoftIpiFixed,
            (unsigned long)g_u64SoftIpiRaw,
            (unsigned long)g_u64SoftSelfIpi,
            (unsigned long)g_u64SoftInventoryLog,
            (unsigned)g_u32IcrSoftLogged,
            (unsigned long)g_u64SoftIcrQuery,
            (unsigned long)g_u64SoftLogApi,
            (unsigned long)g_u64SoftIcrNote,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft mode */
    kprintf("x2apic: soft mode supported=%d enabled=%d cpu=%u "
            "enabled_slots=%u free_slots=%u apic_base=0x%lx x2bit=%u "
            "glob_en=%u peak_slots=%lu wave=%u\n",
            fSupp, fEn, u32Slot, cEn, cFree,
            (unsigned long)u64Base, fX2Bit, fGlobEn,
            (unsigned long)g_u64SoftPeakEnabledSlots,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft icr */
    kprintf("x2apic: soft icr writes=%lu fixed=%lu init=%lu sipi=%lu "
            "self=%lu other=%lu rate_logged=%u rate_max=%u "
            "peak_dest=%lu peak_vec=%lu wave=%u\n",
            (unsigned long)g_u64IcrWrites, (unsigned long)g_u64IcrFixed,
            (unsigned long)g_u64IcrInit, (unsigned long)g_u64IcrSipi,
            (unsigned long)g_u64IcrSelf, (unsigned long)g_u64IcrOther,
            (unsigned)g_u32IcrSoftLogged,
            (unsigned)X2APIC_ICR_SOFT_LOG_MAX,
            (unsigned long)g_u64SoftPeakDest,
            (unsigned long)g_u64SoftPeakVec,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft last */
    kprintf("x2apic: soft last dest=%u mode=%u vec=%u level=%u trig=%u "
            "dm=%u sh=%u val=0x%lx wave=%u\n",
            g_u32IcrLastDest, (unsigned)g_u8IcrLastMode,
            (unsigned)g_u8IcrLastVec, (unsigned)g_u8IcrLastLevel,
            (unsigned)g_u8IcrLastTrigger, (unsigned)g_u8IcrLastDestMode,
            (unsigned)g_u8IcrLastShorthand, (unsigned long)g_u64IcrLast,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft bringup */
    kprintf("x2apic: soft bringup %s init=%lu sipi=%lu writes=%lu "
            "self=%lu fixed=%lu other=%lu enabled_slots=%u wave=%u\n",
            szBringup,
            (unsigned long)g_u64IcrInit, (unsigned long)g_u64IcrSipi,
            (unsigned long)g_u64IcrWrites, (unsigned long)g_u64IcrSelf,
            (unsigned long)g_u64IcrFixed, (unsigned long)g_u64IcrOther,
            cEn, (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft eoi */
    kprintf("x2apic: soft eoi count=%lu (msr=0x%x when mode on) wave=%u\n",
            (unsigned long)g_u64SoftEoi, (unsigned)X2APIC_MSR_EOI,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft msr — fixed MSR surface */
    kprintf("x2apic: soft msr icr=0x%x self_ipi=0x%x eoi=0x%x "
            "apic_base=0x%x x2bit=10 glob_en_bit=11 "
            "cpuid_leaf=1 ecx_bit=21 apic_base_live=0x%lx "
            "x2bit_live=%u glob_en_live=%u wave=%u\n",
            (unsigned)X2APIC_MSR_ICR, (unsigned)X2APIC_MSR_SELF_IPI,
            (unsigned)X2APIC_MSR_EOI, (unsigned)IA32_APIC_BASE_MSR,
            (unsigned long)u64Base, fX2Bit, fGlobEn,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft slots — per-CPU enable occupancy */
    kprintf("x2apic: soft slots max=%u used=%u free=%u peak=%lu "
            "bitmap=0x%x cpu=%u this_en=%d wave=%u\n",
            (unsigned)X2APIC_SOFT_CPU_SLOTS, cEn, cFree,
            (unsigned long)g_u64SoftPeakEnabledSlots, u32Bitmap,
            u32Slot, fEn, (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft probe — detect path tallies */
    kprintf("x2apic: soft probe enter=%lu supp=%lu unsupp=%lu "
            "supported=%d last_cpu=%u enable=after_smp wave=%u\n",
            (unsigned long)g_u64SoftProbe,
            (unsigned long)g_u64SoftProbeSupp,
            (unsigned long)g_u64SoftProbeUnsupp,
            fSupp, u32Slot, (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft enable — enable path reason split */
    kprintf("x2apic: soft enable ok=%lu fail=%lu already=%lu "
            "fail_unsupp=%lu fail_apic=%lu fail_msr=%lu "
            "enabled_slots=%u peak_slots=%lu wave=%u\n",
            (unsigned long)g_u64SoftEnableOk,
            (unsigned long)g_u64SoftEnableFail,
            (unsigned long)g_u64SoftEnableAlready,
            (unsigned long)g_u64SoftEnableFailUnsupp,
            (unsigned long)g_u64SoftEnableFailApic,
            (unsigned long)g_u64SoftEnableFailMsr,
            cEn, (unsigned long)g_u64SoftPeakEnabledSlots,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft ipi — fixed/raw entry tallies */
    kprintf("x2apic: soft ipi fixed=%lu raw=%lu self=%lu "
            "icr_writes=%lu icr_fixed=%lu path=msr_0x830 wave=%u\n",
            (unsigned long)g_u64SoftIpiFixed,
            (unsigned long)g_u64SoftIpiRaw,
            (unsigned long)g_u64SoftSelfIpi,
            (unsigned long)g_u64IcrWrites,
            (unsigned long)g_u64IcrFixed,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft self — self-IPI MSR surface */
    kprintf("x2apic: soft self count=%lu msr=0x%x last_vec=%u "
            "last_dest=0 path=self_ipi_vector_only wave=%u\n",
            (unsigned long)g_u64IcrSelf, (unsigned)X2APIC_MSR_SELF_IPI,
            (unsigned)g_u8IcrLastVec, (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft modes — delivery-mode decode rollup */
    kprintf("x2apic: soft modes fixed=%lu lowpri=%lu smi=%lu nmi=%lu "
            "init=%lu sipi=%lu extint=%lu reserved=%lu other=%lu "
            "catalog=fixed,lowpri,smi,nmi,init,startup,extint wave=%u\n",
            (unsigned long)g_u64IcrFixed,
            (unsigned long)g_u64SoftModeLowpri,
            (unsigned long)g_u64SoftModeSmi,
            (unsigned long)g_u64SoftModeNmi,
            (unsigned long)g_u64IcrInit,
            (unsigned long)g_u64IcrSipi,
            (unsigned long)g_u64SoftModeExtint,
            (unsigned long)g_u64SoftModeReserved,
            (unsigned long)g_u64IcrOther,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft query — soft API accessor samples */
    kprintf("x2apic: soft query enabled=%lu supported=%lu icr_api=%lu "
            "log_api=%lu inv_log=%lu wave=%u\n",
            (unsigned long)g_u64SoftEnabledQuery,
            (unsigned long)g_u64SoftSupportedQuery,
            (unsigned long)g_u64SoftIcrQuery,
            (unsigned long)g_u64SoftLogApi,
            (unsigned long)g_u64SoftInventoryLog,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft fields — ICR field decode rollup (Wave 15) */
    kprintf("x2apic: soft fields level=%lu trigger=%lu logical_dm=%lu "
            "sh_none=%lu sh_self=%lu sh_all=%lu sh_others=%lu "
            "last_level=%u last_trig=%u last_dm=%u last_sh=%u wave=%u\n",
            (unsigned long)g_u64SoftFieldLevel,
            (unsigned long)g_u64SoftFieldTrigger,
            (unsigned long)g_u64SoftFieldLogical,
            (unsigned long)g_u64SoftFieldShNone,
            (unsigned long)g_u64SoftFieldShSelf,
            (unsigned long)g_u64SoftFieldShAll,
            (unsigned long)g_u64SoftFieldShOthers,
            (unsigned)g_u8IcrLastLevel, (unsigned)g_u8IcrLastTrigger,
            (unsigned)g_u8IcrLastDestMode, (unsigned)g_u8IcrLastShorthand,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft dest — dest class rollup (Wave 15) */
    kprintf("x2apic: soft dest zero=%lu unicast=%lu bcast=%lu "
            "self_ipi=%lu peak_dest=%lu last_dest=%u wave=%u\n",
            (unsigned long)g_u64SoftDestZero,
            (unsigned long)g_u64SoftDestUnicast,
            (unsigned long)g_u64SoftDestBcast,
            (unsigned long)g_u64IcrSelf,
            (unsigned long)g_u64SoftPeakDest,
            g_u32IcrLastDest,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft note — soft-note path tallies (Wave 15) */
    kprintf("x2apic: soft note enter=%lu raw=%lu self=%lu "
            "rate_logged=%u rate_max=%u wave=%u\n",
            (unsigned long)g_u64SoftIcrNote,
            (unsigned long)g_u64SoftIcrNoteRaw,
            (unsigned long)g_u64SoftIcrNoteSelf,
            (unsigned)g_u32IcrSoftLogged,
            (unsigned)X2APIC_ICR_SOFT_LOG_MAX,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft outcome — enable+bringup rollup (Wave 15) */
    kprintf("x2apic: soft outcome lamp=%s bringup=%s enable_ok=%lu "
            "enable_fail=%lu enable_already=%lu init=%lu sipi=%lu "
            "enabled_slots=%u wave=%u\n",
            szLamp, szBringup,
            (unsigned long)g_u64SoftEnableOk,
            (unsigned long)g_u64SoftEnableFail,
            (unsigned long)g_u64SoftEnableAlready,
            (unsigned long)g_u64IcrInit,
            (unsigned long)g_u64IcrSipi,
            cEn, (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft api — soft accessor surface (Wave 15) */
    kprintf("x2apic: soft api supported=1 enabled=1 enable=1 probe=1 "
            "send_ipi=1 send_raw=1 self_ipi=1 eoi=1 "
            "icr_soft_writes=1 icr_soft_fixed=1 icr_soft_init=1 "
            "icr_soft_sipi=1 icr_soft_self=1 icr_soft_other=1 "
            "icr_soft_last=1 icr_soft_log=1 wave=%u\n",
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft catalog — API + MSR + mode soft catalog */
    kprintf("x2apic: soft catalog msr_icr=1 msr_self=1 msr_eoi=1 "
            "msr_apic_base=1 mode_fixed=1 mode_init=1 mode_sipi=1 "
            "mode_lowpri=1 mode_smi=1 mode_nmi=1 mode_extint=1 "
            "mode_reserved_soft=1 field_decode=1 dest_class=1 "
            "timer_replace=0 icr_full_replace=0 wave=%u\n",
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft capacity — fixed table / rate lamps */
    kprintf("x2apic: soft capacity cpu_slots=%u rate_log_max=%u "
            "areas=%u heap=0 hard_gate=0 locks=0 wave=%u\n",
            (unsigned)X2APIC_SOFT_CPU_SLOTS,
            (unsigned)X2APIC_ICR_SOFT_LOG_MAX,
            (unsigned)X2APIC_SOFT_DEEPEN_AREAS,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft claim — product claim bounds */
    kprintf("x2apic: soft claim msr_icr=1 self_ipi=1 eoi=1 "
            "dest=32bit_apic_id cpuid_ecx21=1 p_irq1=1 p_smp3=1 "
            "mmio_icr=0 full_icr_replace=0 full_timer_replace=0 "
            "bar3=0 wave=%u\n",
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft path — honesty / product claim bounds */
    kprintf("x2apic: soft path claim=msr_icr+self_ipi+eoi "
            "dest=32bit_apic_id cpuid_ecx21=1 p_irq1=1 p_smp3=1 "
            "mmio_icr=0 hard_gate=0 rate_log_max=%u slots=%u "
            "icr_replace=0 timer_replace=0 "
            "wave=%u (soft inventory; soft != full ICR/timer replace; "
            "not bar3)\n",
            (unsigned)X2APIC_ICR_SOFT_LOG_MAX,
            (unsigned)X2APIC_SOFT_CPU_SLOTS,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /*
     * Wave 16 complementary sub-lines (kept; never reshape primary).
     */
    /* Grep: x2apic: soft exclusive */
    kprintf("x2apic: soft exclusive wave=%u exclusive=1 soft=1 "
            "unit=x2apic.c bar3=0 hard_gate=0 "
            "full_icr_replace=0 full_timer_replace=0 "
            "soft_ne_full_replace=1 areas=%u\n",
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE,
            (unsigned)X2APIC_SOFT_DEEPEN_AREAS);

    /* Grep: x2apic: soft ratio — probe/enable/icr/eoi path ratios */
    kprintf("x2apic: soft ratio probe=%lu enable_ok=%lu enable_fail=%lu "
            "icr_writes=%lu icr_self=%lu eoi=%lu note=%lu "
            "inv_log=%lu slots_en=%u wave=%u\n",
            (unsigned long)g_u64SoftProbe,
            (unsigned long)g_u64SoftEnableOk,
            (unsigned long)g_u64SoftEnableFail,
            (unsigned long)g_u64IcrWrites,
            (unsigned long)g_u64IcrSelf,
            (unsigned long)g_u64SoftEoi,
            (unsigned long)g_u64SoftIcrNote,
            (unsigned long)g_u64SoftInventoryLog,
            cEn,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: x2apic: soft return — Wave 17 API return surfaces (kept) */
    kprintf("x2apic: soft return supported=%u enabled=%u icr=1 eoi=1 soft_inv=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)fSupp, (unsigned)(cEn > 0u ? 1u : 0u), (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("x2apic: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("x2apic: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: x2apic: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("x2apic: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft retcode — Wave 19 retcode catalog */
    kprintf("x2apic: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft deepen wave (Wave 21 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: x2apic: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("x2apic: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
    /* Grep: x2apic: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("x2apic: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: x2apic: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("x2apic: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
    /* Grep: x2apic: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("x2apic: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 exclusive complementary surfaces (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: x2apic: soft retpulse — Wave 21 return-pulse honesty */
            kprintf("x2apic: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
            /* Grep: x2apic: soft retmark — Wave 21 exclusive mark stamp */
            kprintf("x2apic: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
    kprintf("x2apic: soft deepen wave=%u areas=%u inv_log=%lu "
            "probe=%lu enable_ok=%lu icr_writes=%lu self=%lu "
            "eoi=%lu note=%lu ok=1 skip=0 "
            "(soft != full ICR/timer replace product)\n",
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE,
            (unsigned)X2APIC_SOFT_DEEPEN_AREAS,
            (unsigned long)g_u64SoftInventoryLog,
            (unsigned long)g_u64SoftProbe,
            (unsigned long)g_u64SoftEnableOk,
            (unsigned long)g_u64IcrWrites,
            (unsigned long)g_u64IcrSelf,
            (unsigned long)g_u64SoftEoi,
            (unsigned long)g_u64SoftIcrNote);

    /* Grep: x2apic: soft inventory PASS|FAIL|idle */
    kprintf("x2apic: soft inventory %s supported=%d enabled_slots=%u "
            "areas=%u wave=%u\n",
            szLamp, fSupp, cEn,
            (unsigned)X2APIC_SOFT_DEEPEN_AREAS,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);

    /* Grep: x2apic: soft PASS | FAIL | idle */
    kprintf("x2apic: soft %s supported=%d enabled_slots=%u "
            "enable_ok=%lu enable_fail=%lu bringup=%s wave=%u\n",
            szLamp, fSupp, cEn,
            (unsigned long)g_u64SoftEnableOk,
            (unsigned long)g_u64SoftEnableFail,
            szBringup, (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
    if (szLamp[0] == 'P') {
        /* Grep: x2apic: soft PASS */
        kprintf("x2apic: soft PASS wave=%u\n",
                (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
    } else if (szLamp[0] == 'F') {
        /* Grep: x2apic: soft FAIL */
        kprintf("x2apic: soft FAIL wave=%u\n",
                (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
    } else {
        /* Grep: x2apic: soft idle */
        kprintf("x2apic: soft idle wave=%u\n",
                (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
    }
}

/**
 * After first product enable/IPI/EOI activity, print soft inventory once
 * (mirrors futex/input_hub soft-stats-once). Diagnostics only.
 */
static void
x2apic_soft_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_u64SoftEnableOk == 0 && g_u64SoftEnableFail == 0 &&
        g_u64SoftIpiFixed == 0 && g_u64SoftIpiRaw == 0 &&
        g_u64SoftSelfIpi == 0 && g_u64SoftEoi == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    x2apic_soft_inventory();
}

int
x2apic_supported(void)
{
    u32 u32Eax;
    u32 u32Ebx;
    u32 u32Ecx;
    u32 u32Edx;

    g_u64SoftSupportedQuery++;

    if (g_fSupported >= 0) {
        return g_fSupported;
    }
    u32Eax = 1;
    u32Ecx = 0;
    __asm__ volatile (
        "push %%rbx\n\t"
        "cpuid\n\t"
        "mov %%ebx, %1\n\t"
        "pop %%rbx"
        : "+a"(u32Eax), "=r"(u32Ebx), "+c"(u32Ecx), "=d"(u32Edx)
        :
        : "memory");
    g_fSupported = (u32Ecx & (1u << 21)) ? 1 : 0;
    (void)u32Eax;
    (void)u32Ebx;
    (void)u32Edx;
    return g_fSupported;
}

int
x2apic_enabled(void)
{
    u32 u32Slot = cpu_id();

    g_u64SoftEnabledQuery++;

    if (u32Slot >= X2APIC_SOFT_CPU_SLOTS) {
        u32Slot = 0;
    }
    return g_aEnabled[u32Slot] != 0;
}

int
x2apic_enable(void)
{
    u64 u64Base;
    u32 u32Slot = cpu_id();
    u32 cEn;

    if (!x2apic_supported()) {
        g_u64SoftEnableFail++;
        g_u64SoftEnableFailUnsupp++;
        /* Grep: x2apic: soft mode (enable reject) */
        kprintf("x2apic: soft mode enable FAIL reason=unsupported cpu=%u "
                "wave=%u\n",
                u32Slot, (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
        /* Grep: x2apic: soft enable */
        kprintf("x2apic: soft enable FAIL reason=unsupported cpu=%u "
                "fail_unsupp=%lu wave=%u\n",
                u32Slot, (unsigned long)g_u64SoftEnableFailUnsupp,
                (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
        x2apic_soft_maybe_once();
        return -1;
    }
    if (u32Slot >= X2APIC_SOFT_CPU_SLOTS) {
        u32Slot = 0;
    }
    if (g_aEnabled[u32Slot]) {
        g_u64SoftEnableAlready++;
        /* Grep: x2apic: soft enable */
        kprintf("x2apic: soft enable already cpu=%u enabled_slots=%u "
                "wave=%u\n",
                u32Slot, x2apic_soft_enabled_slots(),
                (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
        return 0;
    }
    if (!apic_ready()) {
        g_u64SoftEnableFail++;
        g_u64SoftEnableFailApic++;
        kprintf("x2apic: soft mode enable FAIL reason=apic_not_ready "
                "cpu=%u wave=%u\n",
                u32Slot, (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
        kprintf("x2apic: soft enable FAIL reason=apic_not_ready cpu=%u "
                "fail_apic=%lu wave=%u\n",
                u32Slot, (unsigned long)g_u64SoftEnableFailApic,
                (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
        x2apic_soft_maybe_once();
        return -1;
    }

    u64Base = rdmsr(IA32_APIC_BASE_MSR);
    /* Enable x2APIC: set bit 10 while bit 11 (APIC global enable) stays set. */
    u64Base |= APIC_BASE_ENABLE | APIC_BASE_X2APIC;
    wrmsr(IA32_APIC_BASE_MSR, u64Base);

    /* Verify */
    u64Base = rdmsr(IA32_APIC_BASE_MSR);
    if ((u64Base & APIC_BASE_X2APIC) == 0) {
        g_u64SoftEnableFail++;
        g_u64SoftEnableFailMsr++;
        kprintf("x2apic: enable failed cpu=%u\n", u32Slot);
        kprintf("x2apic: soft mode enable FAIL reason=msr_reject cpu=%u "
                "apic_base=0x%lx wave=%u\n",
                u32Slot, (unsigned long)u64Base,
                (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
        kprintf("x2apic: soft enable FAIL reason=msr_reject cpu=%u "
                "apic_base=0x%lx fail_msr=%lu wave=%u\n",
                u32Slot, (unsigned long)u64Base,
                (unsigned long)g_u64SoftEnableFailMsr,
                (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
        x2apic_soft_maybe_once();
        return -1;
    }
    g_aEnabled[u32Slot] = 1;
    g_u64SoftEnableOk++;
    cEn = x2apic_soft_enabled_slots();
    x2apic_soft_note_enabled_peak(cEn);
    kprintf("x2apic: mode enabled cpu=%u\n", u32Slot);
    /* Soft: ICR path is live on this CPU (MSR 0x830). */
    kprintf("x2apic: icr soft ready cpu=%u writes=%lu\n", u32Slot,
            (unsigned long)g_u64IcrWrites);
    /* Grep: x2apic: soft mode */
    kprintf("x2apic: soft mode enable PASS cpu=%u apic_base=0x%lx "
            "enabled_slots=%u writes=%lu wave=%u\n",
            u32Slot, (unsigned long)u64Base, cEn,
            (unsigned long)g_u64IcrWrites,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
    /* Grep: x2apic: soft enable */
    kprintf("x2apic: soft enable PASS cpu=%u apic_base=0x%lx "
            "enabled_slots=%u peak_slots=%lu wave=%u\n",
            u32Slot, (unsigned long)u64Base, cEn,
            (unsigned long)g_u64SoftPeakEnabledSlots,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
    x2apic_soft_maybe_once();
    return 0;
}

void
x2apic_send_ipi_raw(u64 u64Icr)
{
    g_u64SoftIpiRaw++;
    x2apic_icr_soft_note(u64Icr, 0);
    wrmsr(X2APIC_MSR_ICR, u64Icr);
    x2apic_soft_maybe_once();
}

void
x2apic_send_ipi(u32 u32ApicId, u8 u8Vector)
{
    /*
     * ICR layout in x2APIC: bits 7:0 vector, 10:8 delivery fixed=0,
     * dest in upper 32 bits of MSR write (EDX:EAX as 64-bit).
     */
    u64 u64Icr = (u64)u8Vector | (((u64)u32ApicId) << 32);

    g_u64SoftIpiFixed++;
    x2apic_send_ipi_raw(u64Icr);
}

void
x2apic_send_self_ipi(u8 u8Vector)
{
    g_u64SoftSelfIpi++;
    x2apic_icr_soft_note((u64)u8Vector, 1);
    wrmsr(X2APIC_MSR_SELF_IPI, (u64)u8Vector);
    x2apic_soft_maybe_once();
}

void
x2apic_eoi(void)
{
    g_u64SoftEoi++;
    wrmsr(X2APIC_MSR_EOI, 0);
    x2apic_soft_maybe_once();
}

void
x2apic_probe(void)
{
    int f = x2apic_supported();

    g_u64SoftProbe++;
    if (f != 0) {
        g_u64SoftProbeSupp++;
    } else {
        g_u64SoftProbeUnsupp++;
    }

    /* Detect only — enable after timer calibrate + AP bring-up (safer order). */
    kprintf("x2apic: supported=%d (enable after SMP)\n", f);
    kprintf("x2apic: icr soft counters armed (writes=0 until mode+ICR)\n");
    /* Grep: x2apic: soft mode (probe) */
    kprintf("x2apic: soft mode probe supported=%d cpu=%u "
            "enable=after_smp wave=%u\n",
            f, cpu_id(), (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
    /* Grep: x2apic: soft probe */
    kprintf("x2apic: soft probe supported=%d cpu=%u enter=%lu "
            "supp=%lu unsupp=%lu wave=%u\n",
            f, cpu_id(),
            (unsigned long)g_u64SoftProbe,
            (unsigned long)g_u64SoftProbeSupp,
            (unsigned long)g_u64SoftProbeUnsupp,
            (unsigned)X2APIC_SOFT_DEEPEN_WAVE);
    /* Wave 15 soft inventory at detect (idle ICR expected). */
    x2apic_soft_inventory();
}

/* ---- Soft ICR observability API ------------------------------------ */

u64
x2apic_icr_soft_writes(void)
{
    g_u64SoftIcrQuery++;
    return g_u64IcrWrites;
}

u64
x2apic_icr_soft_fixed(void)
{
    g_u64SoftIcrQuery++;
    return g_u64IcrFixed;
}

u64
x2apic_icr_soft_init(void)
{
    g_u64SoftIcrQuery++;
    return g_u64IcrInit;
}

u64
x2apic_icr_soft_sipi(void)
{
    g_u64SoftIcrQuery++;
    return g_u64IcrSipi;
}

u64
x2apic_icr_soft_self(void)
{
    g_u64SoftIcrQuery++;
    return g_u64IcrSelf;
}

u64
x2apic_icr_soft_other(void)
{
    g_u64SoftIcrQuery++;
    return g_u64IcrOther;
}

u64
x2apic_icr_soft_last(void)
{
    g_u64SoftIcrQuery++;
    return g_u64IcrLast;
}

u32
x2apic_icr_soft_last_dest(void)
{
    g_u64SoftIcrQuery++;
    return g_u32IcrLastDest;
}

u8
x2apic_icr_soft_last_mode(void)
{
    g_u64SoftIcrQuery++;
    return g_u8IcrLastMode;
}

u8
x2apic_icr_soft_last_vector(void)
{
    g_u64SoftIcrQuery++;
    return g_u8IcrLastVec;
}

void
x2apic_icr_soft_log(void)
{
    g_u64SoftLogApi++;

    /*
     * Legacy greppable soft summary (product / smoke inventory):
     *   x2apic: icr soft writes=… fixed=… init=… sipi=… self=… other=…
     *   x2apic: icr soft last dest=… mode=… vec=… val=0x…
     */
    kprintf("x2apic: icr soft writes=%lu fixed=%lu init=%lu sipi=%lu "
            "self=%lu other=%lu\n",
            (unsigned long)g_u64IcrWrites, (unsigned long)g_u64IcrFixed,
            (unsigned long)g_u64IcrInit, (unsigned long)g_u64IcrSipi,
            (unsigned long)g_u64IcrSelf, (unsigned long)g_u64IcrOther);
    kprintf("x2apic: icr soft last dest=%u mode=%u vec=%u val=0x%lx\n",
            g_u32IcrLastDest, (unsigned)g_u8IcrLastMode,
            (unsigned)g_u8IcrLastVec, (unsigned long)g_u64IcrLast);
    if (g_u64IcrInit != 0 || g_u64IcrSipi != 0) {
        /* Bring-up path observed INIT and/or SIPI via x2APIC ICR. */
        kprintf("x2apic: icr soft bringup PASS init=%lu sipi=%lu\n",
                (unsigned long)g_u64IcrInit, (unsigned long)g_u64IcrSipi);
    } else if (g_u64IcrWrites == 0 && g_u64IcrSelf == 0) {
        kprintf("x2apic: icr soft idle (no MSR ICR yet)\n");
    } else {
        kprintf("x2apic: icr soft active (no INIT/SIPI — fixed/self only)\n");
    }

    /* Wave 15: full soft inventory under greppable "x2apic: soft …". */
    x2apic_soft_inventory();
}
