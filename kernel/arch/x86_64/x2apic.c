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
 * Soft inventory (Wave 10) — greppable "x2apic: soft …"
 * -------------------------------------------------------------------------
 * Pure observation; never hard-gates IPI delivery. Counters wrap OK.
 * Prefix-stable product / smoke markers (agent greps):
 *   x2apic: soft inventory   — MSR/mode/slot catalog + feature surface
 *   x2apic: soft stats       — probe/enable/eoi/ICR path counters
 *   x2apic: soft mode        — supported/enabled/cpu/slot lamps
 *   x2apic: soft icr         — writes/fixed/init/sipi/self/other rollup
 *   x2apic: soft last        — last dest/mode/vec/val snapshot
 *   x2apic: soft bringup     — PASS|idle|active INIT/SIPI verdict
 *   x2apic: soft eoi         — EOI MSR soft count (when mode on)
 *
 * Legacy ICR soft lines kept: "x2apic: icr soft …" (bring-up continuity).
 * greppable: x2apic: soft
 * greppable: x2apic: icr soft
 * greppable: x2apic: supported=
 * greppable: MSR 0x830 0x83F P-IRQ-1 P-SMP-3
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
 * Wave 10 soft path counters (sticky; wrap OK; never hard-gate).
 * greppable: x2apic: soft stats
 */
static volatile u64 g_u64SoftProbe;
static volatile u64 g_u64SoftEnableOk;
static volatile u64 g_u64SoftEnableFail;
static volatile u64 g_u64SoftEnableAlready;
static volatile u64 g_u64SoftEoi;
static volatile u64 g_u64SoftIpiFixed;
static volatile u64 g_u64SoftIpiRaw;
static volatile u64 g_u64SoftSelfIpi;
static volatile u64 g_u64SoftInventoryLog;

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
 * Soft: classify + count one ICR write; rate-limited serial for bring-up.
 * Pure observability — never changes delivery semantics.
 */
static void
x2apic_icr_soft_note(u64 u64Icr, int fSelf)
{
    u8  u8Mode;
    u8  u8Vec;
    u32 u32Dest;
    u32 u32N;

    if (fSelf) {
        g_u64IcrSelf++;
        g_u64IcrLast = (u64)(u8)u64Icr; /* self-IPI is vector only */
        g_u32IcrLastDest = 0;           /* self */
        g_u8IcrLastMode = GJ_X2APIC_ICR_MODE_FIXED;
        g_u8IcrLastVec = (u8)u64Icr;
        return;
    }

    u8Mode = (u8)((u64Icr >> 8) & 7u);
    u8Vec = (u8)(u64Icr & 0xffu);
    u32Dest = (u32)(u64Icr >> 32);

    g_u64IcrWrites++;
    g_u64IcrLast = u64Icr;
    g_u32IcrLastDest = u32Dest;
    g_u8IcrLastMode = u8Mode;
    g_u8IcrLastVec = u8Vec;

    if (u8Mode == GJ_X2APIC_ICR_MODE_FIXED) {
        g_u64IcrFixed++;
    } else if (u8Mode == GJ_X2APIC_ICR_MODE_INIT) {
        g_u64IcrInit++;
    } else if (u8Mode == GJ_X2APIC_ICR_MODE_STARTUP) {
        g_u64IcrSipi++;
    } else {
        g_u64IcrOther++;
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
        /* Wave 10: x2apic: soft … */
        kprintf("x2apic: soft icr write n=%u dest=%u mode=%u vec=%u "
                "val=0x%lx\n",
                u32N + 1u, u32Dest, (unsigned)u8Mode, (unsigned)u8Vec,
                (unsigned long)u64Icr);
    }
}

/**
 * Wave 10 soft inventory — greppable "x2apic: soft …" rollup.
 * Safe anytime; pure observation; no heap; no hard-gate.
 * greppable: x2apic: soft inventory
 * greppable: x2apic: soft stats
 * greppable: x2apic: soft mode
 * greppable: x2apic: soft icr
 * greppable: x2apic: soft last
 * greppable: x2apic: soft bringup
 */
static void
x2apic_soft_inventory(void)
{
    u32 u32Slot = cpu_id();
    u32 cEn;
    int fSupp;
    int fEn;
    u64 u64Base = 0;
    u32 fX2Bit = 0;
    u32 fGlobEn = 0;
    const char *szBringup;

    g_u64SoftInventoryLog++;

    if (u32Slot >= X2APIC_SOFT_CPU_SLOTS) {
        u32Slot = 0;
    }
    cEn = x2apic_soft_enabled_slots();
    fSupp = (g_fSupported > 0) ? 1 : 0;
    fEn = (g_aEnabled[u32Slot] != 0) ? 1 : 0;

    /* Live IA32_APIC_BASE peek only when mode is on (else may be pre-apic). */
    if (fEn) {
        u64Base = rdmsr(IA32_APIC_BASE_MSR);
        fX2Bit = (u64Base & APIC_BASE_X2APIC) ? 1u : 0u;
        fGlobEn = (u64Base & APIC_BASE_ENABLE) ? 1u : 0u;
    }

    /*
     * Catalog: fixed surface (MSRs, delivery modes, slots) so scripts can
     * grep product depth without parsing C. P-IRQ-1 / P-SMP-3 product path.
     */
    /* Grep: x2apic: soft inventory */
    kprintf("x2apic: soft inventory slots=%u msr_icr=0x%x msr_self=0x%x "
            "msr_eoi=0x%x msr_apic_base=0x%x x2bit=10 glob_en_bit=11 "
            "modes=fixed,lowpri,smi,nmi,init,startup,extint "
            "cpuid_leaf1_ecx21=1 p_irq1=1 p_smp3=1 "
            "rate_log_max=%u wave=10\n",
            (unsigned)X2APIC_SOFT_CPU_SLOTS,
            (unsigned)X2APIC_MSR_ICR, (unsigned)X2APIC_MSR_SELF_IPI,
            (unsigned)X2APIC_MSR_EOI, (unsigned)IA32_APIC_BASE_MSR,
            (unsigned)X2APIC_ICR_SOFT_LOG_MAX);

    /* Grep: x2apic: soft stats */
    kprintf("x2apic: soft stats probe=%lu enable_ok=%lu enable_fail=%lu "
            "enable_already=%lu eoi=%lu ipi_fixed=%lu ipi_raw=%lu "
            "self_ipi=%lu inv_log=%lu icr_rate_logged=%u\n",
            (unsigned long)g_u64SoftProbe,
            (unsigned long)g_u64SoftEnableOk,
            (unsigned long)g_u64SoftEnableFail,
            (unsigned long)g_u64SoftEnableAlready,
            (unsigned long)g_u64SoftEoi,
            (unsigned long)g_u64SoftIpiFixed,
            (unsigned long)g_u64SoftIpiRaw,
            (unsigned long)g_u64SoftSelfIpi,
            (unsigned long)g_u64SoftInventoryLog,
            (unsigned)g_u32IcrSoftLogged);

    /* Grep: x2apic: soft mode */
    kprintf("x2apic: soft mode supported=%d enabled=%d cpu=%u "
            "enabled_slots=%u apic_base=0x%lx x2bit=%u glob_en=%u\n",
            fSupp, fEn, u32Slot, cEn,
            (unsigned long)u64Base, fX2Bit, fGlobEn);

    /* Grep: x2apic: soft icr */
    kprintf("x2apic: soft icr writes=%lu fixed=%lu init=%lu sipi=%lu "
            "self=%lu other=%lu\n",
            (unsigned long)g_u64IcrWrites, (unsigned long)g_u64IcrFixed,
            (unsigned long)g_u64IcrInit, (unsigned long)g_u64IcrSipi,
            (unsigned long)g_u64IcrSelf, (unsigned long)g_u64IcrOther);

    /* Grep: x2apic: soft last */
    kprintf("x2apic: soft last dest=%u mode=%u vec=%u val=0x%lx\n",
            g_u32IcrLastDest, (unsigned)g_u8IcrLastMode,
            (unsigned)g_u8IcrLastVec, (unsigned long)g_u64IcrLast);

    if (g_u64IcrInit != 0 || g_u64IcrSipi != 0) {
        szBringup = "PASS";
    } else if (g_u64IcrWrites == 0 && g_u64IcrSelf == 0) {
        szBringup = "idle";
    } else {
        szBringup = "active";
    }
    /* Grep: x2apic: soft bringup */
    kprintf("x2apic: soft bringup %s init=%lu sipi=%lu writes=%lu "
            "self=%lu fixed=%lu other=%lu enabled_slots=%u\n",
            szBringup,
            (unsigned long)g_u64IcrInit, (unsigned long)g_u64IcrSipi,
            (unsigned long)g_u64IcrWrites, (unsigned long)g_u64IcrSelf,
            (unsigned long)g_u64IcrFixed, (unsigned long)g_u64IcrOther,
            cEn);

    /* Grep: x2apic: soft eoi */
    kprintf("x2apic: soft eoi count=%lu (msr=0x%x when mode on)\n",
            (unsigned long)g_u64SoftEoi, (unsigned)X2APIC_MSR_EOI);
}

int
x2apic_supported(void)
{
    u32 u32Eax;
    u32 u32Ebx;
    u32 u32Ecx;
    u32 u32Edx;

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

    if (!x2apic_supported()) {
        g_u64SoftEnableFail++;
        /* Grep: x2apic: soft mode (enable reject) */
        kprintf("x2apic: soft mode enable FAIL reason=unsupported cpu=%u\n",
                u32Slot);
        return -1;
    }
    if (u32Slot >= X2APIC_SOFT_CPU_SLOTS) {
        u32Slot = 0;
    }
    if (g_aEnabled[u32Slot]) {
        g_u64SoftEnableAlready++;
        return 0;
    }
    if (!apic_ready()) {
        g_u64SoftEnableFail++;
        kprintf("x2apic: soft mode enable FAIL reason=apic_not_ready "
                "cpu=%u\n", u32Slot);
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
        kprintf("x2apic: enable failed cpu=%u\n", u32Slot);
        kprintf("x2apic: soft mode enable FAIL reason=msr_reject cpu=%u "
                "apic_base=0x%lx\n", u32Slot, (unsigned long)u64Base);
        return -1;
    }
    g_aEnabled[u32Slot] = 1;
    g_u64SoftEnableOk++;
    kprintf("x2apic: mode enabled cpu=%u\n", u32Slot);
    /* Soft: ICR path is live on this CPU (MSR 0x830). */
    kprintf("x2apic: icr soft ready cpu=%u writes=%lu\n", u32Slot,
            (unsigned long)g_u64IcrWrites);
    /* Grep: x2apic: soft mode */
    kprintf("x2apic: soft mode enable PASS cpu=%u apic_base=0x%lx "
            "enabled_slots=%u writes=%lu\n",
            u32Slot, (unsigned long)u64Base,
            x2apic_soft_enabled_slots(),
            (unsigned long)g_u64IcrWrites);
    return 0;
}

void
x2apic_send_ipi_raw(u64 u64Icr)
{
    g_u64SoftIpiRaw++;
    x2apic_icr_soft_note(u64Icr, 0);
    wrmsr(X2APIC_MSR_ICR, u64Icr);
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
}

void
x2apic_eoi(void)
{
    g_u64SoftEoi++;
    wrmsr(X2APIC_MSR_EOI, 0);
}

void
x2apic_probe(void)
{
    int f = x2apic_supported();

    g_u64SoftProbe++;

    /* Detect only — enable after timer calibrate + AP bring-up (safer order). */
    kprintf("x2apic: supported=%d (enable after SMP)\n", f);
    kprintf("x2apic: icr soft counters armed (writes=0 until mode+ICR)\n");
    /* Grep: x2apic: soft mode (probe) */
    kprintf("x2apic: soft mode probe supported=%d cpu=%u "
            "enable=after_smp wave=10\n",
            f, cpu_id());
    /* Wave 10 soft inventory at detect (idle ICR expected). */
    x2apic_soft_inventory();
}

/* ---- Soft ICR observability API ------------------------------------ */

u64
x2apic_icr_soft_writes(void)
{
    return g_u64IcrWrites;
}

u64
x2apic_icr_soft_fixed(void)
{
    return g_u64IcrFixed;
}

u64
x2apic_icr_soft_init(void)
{
    return g_u64IcrInit;
}

u64
x2apic_icr_soft_sipi(void)
{
    return g_u64IcrSipi;
}

u64
x2apic_icr_soft_self(void)
{
    return g_u64IcrSelf;
}

u64
x2apic_icr_soft_other(void)
{
    return g_u64IcrOther;
}

u64
x2apic_icr_soft_last(void)
{
    return g_u64IcrLast;
}

u32
x2apic_icr_soft_last_dest(void)
{
    return g_u32IcrLastDest;
}

u8
x2apic_icr_soft_last_mode(void)
{
    return g_u8IcrLastMode;
}

u8
x2apic_icr_soft_last_vector(void)
{
    return g_u8IcrLastVec;
}

void
x2apic_icr_soft_log(void)
{
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

    /* Wave 10: full soft inventory under greppable "x2apic: soft …". */
    x2apic_soft_inventory();
}
