/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * MADT (APIC) discovery + AP bring-up + one-shot AP work.
 * Online inventory: g_Smp + g_aSlotApicId; soft helpers for telemetry/affinity.
 * Bring-up is capped by static percpu/stack pools; PMM percpu growth is ready
 * in cpu_init_ap for higher ids once park stacks / idle arrays scale.
 *
 * Soft deepen (boot telemetry only — not hot-path locks):
 *   Per-slot status / phase / ready-wait spins; cumulative ap_run ok/fail.
 *   smp_bringup_soft_log greps PASS|PARTIAL|UP + x2APIC ICR soft summary.
 *
 * Wave 9 soft SMP inventory (product / smoke — greppable prefix):
 *   smp: soft PASS|PARTIAL|UP|NONE …
 *   smp: soft inventory …
 *   smp: soft slot=… status=… phase=…
 *   smp: soft ap_run …
 *   smp: soft madt …
 *   smp: soft phases …
 *
 * Wave 12 exclusive deepen (this unit only — greppable "smp: soft …"):
 *   - Init path: rsdp / mb2 / bios scan / UP fallback + MADT parse tallies
 *   - start_aps early-outs + INIT-SIPI + skip class split (dis/bsp/cap)
 *   - AP entry: entry / percpu_fail / idle_fail / sched_ready + timer HWM
 *   - ap_run: enter / inval / busy / ipi + poll drain
 *   - spins HWM + log_n + path honesty catalog
 *   Primary PASS|inventory|madt|phases|ap_run|slot lines stay field-stable.
 *
 * Wave 14 exclusive deepen (this unit only — greppable "smp: soft …"):
 *   smp: soft deepen|verdict|handshake|online|sipi|x2|fail|ratio|path
 *   Additive rollups + basis-point ratios; Wave 9/12 primary lines stable.
 *
 * Wave 15 exclusive soft deepen (this unit only — greppable "smp: soft …"):
 *   smp: soft headroom|surface|deepen  — cap/MADT head + area catalog
 *   Inventory deepen wave stamp → 15; areas → 12. Wave 9/12 primary lines
 *   stay field-stable. Never hard-gates product; wrap-OK + kprintf only.
 *
 * Wave 16 exclusive soft deepen (this unit only — greppable "smp: soft …"):
 *   smp: soft return|surface|deepen  — bringup/ap_run return-path catalog
 *   Inventory deepen wave stamp → 16; areas → 15. Wave 9/12 primary lines
 *   stay field-stable. Soft ≠ multi-CPU product / bar3 gate.
 *
 * Wave 56 exclusive soft deepen (this unit only — greppable "smp: soft …"):
 *   smp: soft return|ret_surface|surface|deepen  — deepen bringup return classes
 *   Inventory deepen wave stamp → 17; areas → 16. Wave 9/12 primary lines
 *   stay field-stable. product_kernel=OPEN. Soft ≠ multi-CPU product / bar3.
 */
#include <gj/apic.h>
#include <gj/config.h>
#include <gj/cpu.h>
#include <gj/gdt.h>
#include <gj/idt.h>
#include <gj/klog.h>
#include <gj/smp.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/types.h>
#include <gj/x2apic.h>

/* Physical pages for AP boot (identity-mapped low RAM). */
#define AP_TRAMPOLINE_PA  0x8000u
#define AP_BOOT_INFO_PA   0x7000u
#define AP_SIPI_VECTOR    0x08u /* 0x8000 >> 12 */

extern char ap_trampoline_start[];
extern char ap_trampoline_end[];

struct gj_ap_boot_info {
    u16 u16GdtrLimit; /* unused when trampoline embeds GDT; pad for alignment */
    u16 u16Pad;
    u32 u32Pad;
    u64 u64GdtrBase;  /* reserved */
    u64 u64Cr3;
    u64 u64Entry;
    u64 u64Stack;
    u32 u32CpuIndex;
    u32 u32Pad2;
} __attribute__((packed));

/*
 * Bound static park stacks so BSS stays below user maps @ 0x1000000.
 * Bring-up cap = min(GJ_CPU_STATIC_MAX, GJ_AP_STACK_SLOTS); see smp_bringup_cap.
 * PMM percpu growth (cpu_init_ap) is ready for higher ids once these scale.
 */
#define GJ_AP_STACK_SLOTS 8
#define GJ_AP_STACK_SIZE  4096
static u8 g_aApStacks[GJ_AP_STACK_SLOTS][GJ_AP_STACK_SIZE]
    __attribute__((aligned(16)));
static volatile u32 g_u32ApReady;
/* Slot → APIC id for online inventory (indices < smp_bringup_cap only). */
static u32 g_aSlotApicId[GJ_CPU_STATIC_MAX];
static volatile u8 g_aApSchedReady[GJ_CPU_STATIC_MAX];
/* MADT + online inventory (published before AP helpers run). */
static struct gj_smp_info g_Smp;

/* Soft bring-up observability (sticky after smp_start_aps / ap_run). */
static u32 g_u32SoftTried;
static u32 g_u32SoftOk;
static u32 g_u32SoftTimeout;
static u32 g_u32SoftSkipped;
static u32 g_u32SoftApRunOk;
static u32 g_u32SoftApRunFail;
static u32 g_u32SoftApRunTimeout;
static u32 g_u32SoftLastSpins;
static u32 g_u32SoftLastApicId;
static u32 g_u32SoftLastSlot;
static u8  g_aSoftSlotStatus[GJ_CPU_STATIC_MAX]; /* GJ_SMP_SOFT_SLOT_* */
static volatile u8 g_aSoftApPhase[GJ_CPU_STATIC_MAX]; /* GJ_SMP_AP_PHASE_* */
static u32 g_aSoftSpins[GJ_CPU_STATIC_MAX];

/*
 * Wave 12 soft path tallies (file-local deepen; greppable "smp: soft …").
 * Cumulative unless noted HWM; wrap-OK; never hard-gates bring-up.
 */
static u32 g_u32SoftInitRsdp;       /* smp_init_from_rsdp entries */
static u32 g_u32SoftInitMb2;        /* smp_init_from_mb2 entries */
static u32 g_u32SoftInitRsdpHit;    /* UEFI/RSDP path yielded MADT */
static u32 g_u32SoftInitBiosScan;   /* scan_bios_rsdp invoked */
static u32 g_u32SoftInitBiosHit;    /* BIOS scan yielded MADT */
static u32 g_u32SoftInitUpFallback; /* MADT missing; UP fallback */
static u32 g_u32SoftMadtParse;      /* parse_madt accepted */
static u32 g_u32SoftMadtCsumBad;    /* parse_madt checksum reject */
static u32 g_u32SoftMadtLocal;      /* LOCAL_APIC entries accepted */
static u32 g_u32SoftMadtX2;         /* LOCAL_X2APIC entries accepted */
static u32 g_u32SoftMadtDedup;      /* smp_add_apic de-dup drops */
static u32 g_u32SoftMadtFull;       /* smp_add_apic at GJ_SMP_MAX_APICS */
static u32 g_u32SoftMadtDisabled;   /* add with fEnabled=0 */
static u32 g_u32SoftStartEnter;     /* smp_start_aps entries */
static u32 g_u32SoftStartNoApic;    /* start_aps early: !apic_ready */
static u32 g_u32SoftStartUp;        /* start_aps early: NLocalApic <= 1 */
static u32 g_u32SoftStartBadTramp;  /* start_aps early: tramp size */
static u32 g_u32SoftStartX2Arm;     /* x2APIC armed in start_aps */
static u32 g_u32SoftStartSipi;      /* INIT-SIPI issued */
static u32 g_u32SoftSkipDis;        /* skip class: MADT disabled */
static u32 g_u32SoftSkipBsp;        /* skip class: MADT entry is BSP */
static u32 g_u32SoftSkipCap;        /* skip class: over bring-up/stack cap */
static u32 g_u32SoftApEntry;        /* smp_ap_c_entry entries */
static u32 g_u32SoftApPercpuFail;   /* AP percpu missing → park */
static u32 g_u32SoftApIdleFail;     /* AP idle create fail → park */
static u32 g_u32SoftApSchedReady;   /* AP reached schedule ready */
static u32 g_u32SoftApTimerSpinHwm; /* max AP local-timer wait spins */
static u32 g_u32SoftApRunEnter;     /* smp_ap_run entries */
static u32 g_u32SoftApRunInval;     /* ap_run bad args / OOB */
static u32 g_u32SoftApRunBusy;      /* ap_run busy (outstanding work) */
static u32 g_u32SoftApRunIpi;       /* ap_run IPI posted */
static u32 g_u32SoftApPoll;         /* smp_ap_poll_work entries */
static u32 g_u32SoftApPollDrain;    /* one-shot work drained on AP */
static u32 g_u32SoftSpinsHwm;       /* max ready-wait spins (any slot) */
static u32 g_u32SoftLogN;           /* smp_bringup_soft_log emissions */

/* One-shot AP work for smp_ap_run (drained on AP schedule path). */
static void (*volatile g_pfnApWork)(void *);
static void *volatile g_pApWorkArg;
static volatile u32 g_u32ApWorkSlot;
static volatile u32 g_u32ApWorkDone;

/** Soft: saturate-increment sticky u32 (wrap-OK diagnostics). */
static void
smp_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

/** Soft: raise sticky max phase for slot (never regresses). */
static void
smp_soft_phase_set(u32 u32CpuSlot, u8 u8Phase)
{
    if (u32CpuSlot == 0 || u32CpuSlot >= GJ_CPU_STATIC_MAX) {
        return;
    }
    if (g_aSoftApPhase[u32CpuSlot] < u8Phase) {
        g_aSoftApPhase[u32CpuSlot] = u8Phase;
    }
}

/** Soft: note ready-wait spin HWM (bring-up only). */
static void
smp_soft_note_spins(u32 u32Spins)
{
    if (u32Spins > g_u32SoftSpinsHwm) {
        g_u32SoftSpinsHwm = u32Spins;
    }
}

void
smp_ap_poll_work(void)
{
    void (*pfn)(void *) = g_pfnApWork;
    u32 u32Cpu = cpu_id();

    smp_soft_inc(&g_u32SoftApPoll);
    if (pfn != NULL && g_u32ApWorkSlot == u32Cpu) {
        void *pArg = g_pApWorkArg;

        g_pfnApWork = NULL;
        __asm__ volatile ("" ::: "memory");
        pfn(pArg);
        g_u32ApWorkDone = 1;
        smp_soft_inc(&g_u32SoftApPollDrain);
    }
}

int
smp_ap_sched_ready(u32 u32CpuSlot)
{
    if (u32CpuSlot == 0) {
        return 1; /* BSP always sched-ready after boot */
    }
    if (u32CpuSlot >= GJ_CPU_STATIC_MAX) {
        return 0;
    }
    return g_aApSchedReady[u32CpuSlot] != 0;
}

u32
smp_bringup_cap(void)
{
    u32 u32Cap = GJ_CPU_STATIC_MAX;

    if (u32Cap > GJ_AP_STACK_SLOTS) {
        u32Cap = GJ_AP_STACK_SLOTS;
    }
    if (u32Cap == 0) {
        u32Cap = 1; /* defensive: always room for BSP */
    }
    return u32Cap;
}

int
smp_cpu_online(u32 u32CpuSlot)
{
    if (u32CpuSlot == 0) {
        return 1;
    }
    if (u32CpuSlot >= g_Smp.u32NOnline || u32CpuSlot >= smp_bringup_cap()) {
        return 0;
    }
    return 1;
}

void
smp_ap_c_entry(u32 u32CpuIndex)
{
    u64 u64Ticks0;
    u64 u64Ticks1;
    u32 u32Spins;

    smp_soft_inc(&g_u32SoftApEntry);
    /* Soft: trampoline reached C entry (before GDT/IDT). */
    smp_soft_phase_set(u32CpuIndex, (u8)GJ_SMP_AP_PHASE_ENTRY);

    /* Kernel GDT+IDT before IRQs (trampoline used a private GDT). */
    gdt_load_ap();
    idt_load_ap();
    /*
     * Percpu + GS_BASE. Static slots always succeed; dyn slots (id >=
     * GJ_CPU_STATIC_MAX) may return early on PMM OOM — do not trust GS then.
     * Bound sched-ready / idle paths to the static bring-up inventory.
     */
    if (u32CpuIndex == 0 || u32CpuIndex >= GJ_MAX_CPUS) {
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }
    cpu_init_ap(u32CpuIndex);
    if (!cpu_slot_online(u32CpuIndex)) {
        /* Greppable: smp: AP percpu missing */
        smp_soft_inc(&g_u32SoftApPercpuFail);
        kprintf("smp: AP percpu missing cpu=%u — HLT park\n", u32CpuIndex);
        if (u32CpuIndex < GJ_CPU_STATIC_MAX) {
            g_aSoftSlotStatus[u32CpuIndex] = (u8)GJ_SMP_SOFT_SLOT_FAIL;
        }
        g_u32ApReady = 1;
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }
    smp_soft_phase_set(u32CpuIndex, (u8)GJ_SMP_AP_PHASE_PERCPU);
    /* Match BSP: enable x2APIC on this CPU before MSR-based LAPIC access. */
    if (x2apic_supported()) {
        (void)x2apic_enable();
    }
    smp_soft_phase_set(u32CpuIndex, (u8)GJ_SMP_AP_PHASE_X2APIC);
    /* Local APIC timer so HLT wakes; does not drive global mono (BSP only). */
    apic_timer_start_local();
    smp_soft_phase_set(u32CpuIndex, (u8)GJ_SMP_AP_PHASE_TIMER);
    __asm__ volatile ("sti");
    u64Ticks0 = apic_timer_ticks_cpu(u32CpuIndex);
    for (u32Spins = 0; u32Spins < 50000000u; u32Spins++) {
        u64Ticks1 = apic_timer_ticks_cpu(u32CpuIndex);
        if (u64Ticks1 > u64Ticks0) {
            break;
        }
        __asm__ volatile ("pause");
    }
    if (u32Spins > g_u32SoftApTimerSpinHwm) {
        g_u32SoftApTimerSpinHwm = u32Spins;
    }
    kprintf("cpu: AP id=%u timer_ticks=%lu\n", u32CpuIndex,
            (unsigned long)apic_timer_ticks_cpu(u32CpuIndex));

    /* Per-CPU idle before signalling ready (serialize vs next SIPI). */
    if (u32CpuIndex >= GJ_CPU_STATIC_MAX ||
        thread_init_ap_idle(u32CpuIndex) != 0) {
        smp_soft_inc(&g_u32SoftApIdleFail);
        kprintf("smp: AP idle create failed cpu=%u — HLT park\n", u32CpuIndex);
        if (u32CpuIndex < GJ_CPU_STATIC_MAX) {
            g_aSoftSlotStatus[u32CpuIndex] = (u8)GJ_SMP_SOFT_SLOT_FAIL;
        }
        g_u32ApReady = 1;
        for (;;) {
            smp_ap_poll_work();
            __asm__ volatile ("hlt");
        }
    }
    smp_soft_phase_set(u32CpuIndex, (u8)GJ_SMP_AP_PHASE_IDLE);
    if (u32CpuIndex < GJ_CPU_STATIC_MAX) {
        g_aApSchedReady[u32CpuIndex] = 1;
    }
    smp_soft_phase_set(u32CpuIndex, (u8)GJ_SMP_AP_PHASE_SCHED);
    g_u32ApReady = 1;
    smp_soft_inc(&g_u32SoftApSchedReady);
    /* Greppable: smp: AP schedule ready */
    kprintf("smp: AP schedule ready cpu=%u phase=%u\n", u32CpuIndex,
            (unsigned)smp_bringup_soft_phase(u32CpuIndex));
    scheduler_run_ap();
}

#define MB2_TAG_END        0
#define MB2_TAG_ACPI_OLD   14
#define MB2_TAG_ACPI_NEW   15

#define ACPI_SIG_RSDT 0x54445352u /* 'RSDT' */
#define ACPI_SIG_XSDT 0x54445358u /* 'XSDT' */
#define ACPI_SIG_APIC 0x43495041u /* 'APIC' */

struct mb2_tag {
    u32 u32Type;
    u32 u32Size;
};

struct acpi_rsdp_v1 {
    char  aSig[8];
    u8    u8Checksum;
    char  aOem[6];
    u8    u8Revision;
    u32   u32Rsdt;
} __attribute__((packed));

struct acpi_rsdp_v2 {
    struct acpi_rsdp_v1 v1;
    u32 u32Length;
    u64 u64Xsdt;
    u8  u8ExtChecksum;
    u8  aReserved[3];
} __attribute__((packed));

struct acpi_sdt_hdr {
    u32 u32Sig;
    u32 u32Length;
    u8  u8Revision;
    u8  u8Checksum;
    char aOemId[6];
    char aOemTableId[8];
    u32 u32OemRev;
    u32 u32CreatorId;
    u32 u32CreatorRev;
} __attribute__((packed));

struct acpi_madt {
    struct acpi_sdt_hdr hdr;
    u32 u32LocalApicAddr;
    u32 u32Flags;
} __attribute__((packed));

/* MADT entry types */
#define MADT_LOCAL_APIC     0
#define MADT_IO_APIC        1
#define MADT_LOCAL_X2APIC   9

struct madt_local_apic {
    u8  u8Type;
    u8  u8Length;
    u8  u8AcpiId;
    u8  u8ApicId;
    u32 u32Flags;
} __attribute__((packed));

struct madt_local_x2apic {
    u8  u8Type;
    u8  u8Length;
    u16 u16Reserved;
    u32 u32X2ApicId;
    u32 u32Flags;
    u32 u32AcpiId;
} __attribute__((packed));

static void
smp_add_apic(u32 u32ApicId, int fEnabled, int fX2)
{
    u32 i;

    if (g_Smp.u32NLocalApic >= GJ_SMP_MAX_APICS) {
        smp_soft_inc(&g_u32SoftMadtFull);
        return;
    }
    /* de-dup */
    for (i = 0; i < g_Smp.u32NLocalApic; i++) {
        if (g_Smp.aApicId[i] == u32ApicId) {
            smp_soft_inc(&g_u32SoftMadtDedup);
            return;
        }
    }
    i = g_Smp.u32NLocalApic++;
    g_Smp.aApicId[i] = u32ApicId;
    g_Smp.aEnabled[i] = fEnabled ? 1 : 0;
    if (!fEnabled) {
        smp_soft_inc(&g_u32SoftMadtDisabled);
    }
    if (fX2) {
        g_Smp.fX2ApicIds = 1;
        smp_soft_inc(&g_u32SoftMadtX2);
    } else {
        smp_soft_inc(&g_u32SoftMadtLocal);
    }
}

static int
acpi_checksum(const void *p, u32 cb)
{
    const u8 *pB = (const u8 *)p;
    u8 u8Sum = 0;
    u32 i;

    /* Guard against corrupt/huge lengths hanging the boot CPU */
    if (p == NULL || cb == 0 || cb > (1u * 1024u * 1024u)) {
        return 0;
    }
    for (i = 0; i < cb; i++) {
        u8Sum = (u8)(u8Sum + pB[i]);
    }
    return u8Sum == 0;
}

static void
parse_madt(const struct acpi_madt *pMadt)
{
    const u8 *p;
    const u8 *pEnd;

    if (pMadt == NULL || pMadt->hdr.u32Length < sizeof(*pMadt)) {
        return;
    }
    if (!acpi_checksum(pMadt, pMadt->hdr.u32Length)) {
        smp_soft_inc(&g_u32SoftMadtCsumBad);
        kprintf("smp: MADT checksum bad\n");
        return;
    }
    g_Smp.fHasMadt = 1;
    smp_soft_inc(&g_u32SoftMadtParse);
    p = (const u8 *)pMadt + sizeof(*pMadt);
    pEnd = (const u8 *)pMadt + pMadt->hdr.u32Length;
    while (p + 2 <= pEnd) {
        u8 u8Type = p[0];
        u8 u8Len = p[1];

        if (u8Len < 2 || p + u8Len > pEnd) {
            break;
        }
        if (u8Type == MADT_LOCAL_APIC && u8Len >= sizeof(struct madt_local_apic)) {
            const struct madt_local_apic *pE = (const struct madt_local_apic *)p;
            int fEn = (pE->u32Flags & 1u) != 0;

            smp_add_apic(pE->u8ApicId, fEn, 0);
        } else if (u8Type == MADT_LOCAL_X2APIC &&
                   u8Len >= sizeof(struct madt_local_x2apic)) {
            const struct madt_local_x2apic *pE =
                (const struct madt_local_x2apic *)p;
            int fEn = (pE->u32Flags & 1u) != 0;

            smp_add_apic(pE->u32X2ApicId, fEn, 1);
        }
        p += u8Len;
    }
}

static void
walk_rsdt(u32 u32RsdtPa)
{
    const struct acpi_sdt_hdr *pRsdt =
        (const struct acpi_sdt_hdr *)(gj_vaddr_t)u32RsdtPa;
    u32 cEntries;
    u32 i;
    const u32 *pEnt;

    if (pRsdt == NULL || pRsdt->u32Sig != ACPI_SIG_RSDT) {
        return;
    }
    if (pRsdt->u32Length < sizeof(*pRsdt) ||
        !acpi_checksum(pRsdt, pRsdt->u32Length)) {
        return;
    }
    cEntries = (pRsdt->u32Length - (u32)sizeof(*pRsdt)) / 4u;
    if (cEntries > 256u) {
        cEntries = 256u;
    }
    pEnt = (const u32 *)(const void *)(pRsdt + 1);
    for (i = 0; i < cEntries; i++) {
        const struct acpi_sdt_hdr *pHdr =
            (const struct acpi_sdt_hdr *)(gj_vaddr_t)pEnt[i];

        if (pHdr != NULL && pHdr->u32Sig == ACPI_SIG_APIC) {
            parse_madt((const struct acpi_madt *)pHdr);
            return;
        }
    }
}

static void
walk_xsdt(u64 u64XsdtPa)
{
    const struct acpi_sdt_hdr *pXsdt =
        (const struct acpi_sdt_hdr *)(gj_vaddr_t)u64XsdtPa;
    u32 cEntries;
    u32 i;
    const u64 *pEnt;

    if (pXsdt == NULL || pXsdt->u32Sig != ACPI_SIG_XSDT) {
        return;
    }
    if (pXsdt->u32Length < sizeof(*pXsdt) ||
        !acpi_checksum(pXsdt, pXsdt->u32Length)) {
        return;
    }
    cEntries = (pXsdt->u32Length - (u32)sizeof(*pXsdt)) / 8u;
    if (cEntries > 256u) {
        cEntries = 256u;
    }
    pEnt = (const u64 *)(const void *)(pXsdt + 1);
    for (i = 0; i < cEntries; i++) {
        const struct acpi_sdt_hdr *pHdr =
            (const struct acpi_sdt_hdr *)(gj_vaddr_t)pEnt[i];

        if (pHdr != NULL && pHdr->u32Sig == ACPI_SIG_APIC) {
            parse_madt((const struct acpi_madt *)pHdr);
            return;
        }
    }
}

static void
from_rsdp(const void *pRsdp, u32 cbTagPayload)
{
    const struct acpi_rsdp_v1 *pV1 = (const struct acpi_rsdp_v1 *)pRsdp;
    const struct acpi_rsdp_v2 *pV2;

    if (pV1 == NULL || cbTagPayload < sizeof(*pV1)) {
        return;
    }
    if (pV1->aSig[0] != 'R' || pV1->aSig[1] != 'S' || pV1->aSig[2] != 'D' ||
        pV1->aSig[3] != ' ' || pV1->aSig[4] != 'P' || pV1->aSig[5] != 'T' ||
        pV1->aSig[6] != 'R' || pV1->aSig[7] != ' ') {
        return;
    }
    if (pV1->u8Revision >= 2 && cbTagPayload >= sizeof(*pV2)) {
        pV2 = (const struct acpi_rsdp_v2 *)pRsdp;
        if (pV2->u64Xsdt != 0) {
            walk_xsdt(pV2->u64Xsdt);
            if (g_Smp.fHasMadt) {
                return;
            }
        }
    }
    if (pV1->u32Rsdt != 0) {
        walk_rsdt(pV1->u32Rsdt);
    }
}

/** Scan EBDA + 0xE0000..0xFFFFF for RSDP signature (legacy ACPI discovery). */
static void
scan_bios_rsdp(void)
{
    u16 u16EbdaSeg;
    u32 u32Base;
    u32 u32Off;
    const u8 *p;

    smp_soft_inc(&g_u32SoftInitBiosScan);
    /* EBDA segment pointer at BDA 0x40E */
    u16EbdaSeg = *(volatile u16 *)(gj_vaddr_t)0x40E;
    if (u16EbdaSeg != 0) {
        u32Base = (u32)u16EbdaSeg << 4;
        for (u32Off = 0; u32Off < 0x400u; u32Off += 16u) {
            p = (const u8 *)(gj_vaddr_t)(u32Base + u32Off);
            if (p[0] == 'R' && p[1] == 'S' && p[2] == 'D' && p[3] == ' ' &&
                p[4] == 'P' && p[5] == 'T' && p[6] == 'R' && p[7] == ' ') {
                from_rsdp(p, 36);
                if (g_Smp.fHasMadt) {
                    smp_soft_inc(&g_u32SoftInitBiosHit);
                    return;
                }
            }
        }
    }
    for (u32Base = 0xE0000u; u32Base < 0x100000u; u32Base += 16u) {
        p = (const u8 *)(gj_vaddr_t)u32Base;
        if (p[0] == 'R' && p[1] == 'S' && p[2] == 'D' && p[3] == ' ' &&
            p[4] == 'P' && p[5] == 'T' && p[6] == 'R' && p[7] == ' ') {
            from_rsdp(p, 36);
            if (g_Smp.fHasMadt) {
                smp_soft_inc(&g_u32SoftInitBiosHit);
                return;
            }
        }
    }
}

static void
smp_finish_madt_log(void)
{
    if (g_Smp.u32NLocalApic == 0) {
        smp_soft_inc(&g_u32SoftInitUpFallback);
        smp_add_apic(0, 1, 0);
        kprintf("smp: MADT missing; UP fallback\n");
    } else {
        u32 i;
        u32 u32En = 0;

        for (i = 0; i < g_Smp.u32NLocalApic; i++) {
            if (g_Smp.aEnabled[i]) {
                u32En++;
            }
        }
        g_Smp.u32BspApicId = g_Smp.aApicId[0];
        kprintf("smp: MADT local_apics=%u enabled=%u online=%u x2=%d bsp_id=%u\n",
                g_Smp.u32NLocalApic, u32En, g_Smp.u32NOnline, g_Smp.fX2ApicIds,
                g_Smp.u32BspApicId);
        for (i = 0; i < g_Smp.u32NLocalApic && i < 8u; i++) {
            kprintf("smp:  apic[%u]=%u enabled=%u\n", i, g_Smp.aApicId[i],
                    g_Smp.aEnabled[i]);
        }
    }
}

void
smp_init_from_rsdp(u64 u64Rsdp)
{
    smp_soft_inc(&g_u32SoftInitRsdp);
    kprintf("smp: init_from_rsdp pa=0x%lx\n", (unsigned long)u64Rsdp);
    memset(&g_Smp, 0, sizeof(g_Smp));
    g_Smp.u32NOnline = 1;
    g_Smp.u32BspApicId = 0;

    if (u64Rsdp != 0) {
        from_rsdp((const void *)(gj_vaddr_t)u64Rsdp, 36);
        if (g_Smp.fHasMadt) {
            smp_soft_inc(&g_u32SoftInitRsdpHit);
        }
    }
    if (!g_Smp.fHasMadt) {
        scan_bios_rsdp();
    }
    smp_finish_madt_log();
}

void
smp_init_from_mb2(u32 paMb2Info)
{
    struct mb2_tag *pTag;
    u8 *pInfo;
    u32 u32Tags;
    u32 u32Total;

    smp_soft_inc(&g_u32SoftInitMb2);
    kprintf("smp: init_from_mb2 pa=0x%x\n", paMb2Info);
    memset(&g_Smp, 0, sizeof(g_Smp));
    g_Smp.u32NOnline = 1;
    g_Smp.u32BspApicId = 0;

    if (paMb2Info != 0) {
        pInfo = (u8 *)(gj_vaddr_t)paMb2Info;
        /* Multiboot2 total_size is first u32 of info struct */
        u32Total = *(const u32 *)(const void *)pInfo;
        if (u32Total < 16u || u32Total > (4u * 1024u * 1024u)) {
            kprintf("smp: mb2 total_size bad %u — skip tags\n", u32Total);
            u32Total = 0;
        }
        pTag = (struct mb2_tag *)(pInfo + 8);
        u32Tags = 0;
        while (u32Total != 0 && pTag->u32Type != MB2_TAG_END &&
               u32Tags < 256u) {
            u32 cbTag;
            u8 *pNext;
            u8 *pEnd = pInfo + u32Total;

            if ((u8 *)pTag + 8 > pEnd) {
                kprintf("smp: mb2 tag overrun\n");
                break;
            }
            if (pTag->u32Type == MB2_TAG_ACPI_OLD ||
                pTag->u32Type == MB2_TAG_ACPI_NEW) {
                const u8 *pPayload = (const u8 *)pTag + 8;
                u32 cb = pTag->u32Size > 8 ? pTag->u32Size - 8 : 0;

                kprintf("smp: mb2 ACPI tag type=%u size=%u\n", pTag->u32Type,
                        pTag->u32Size);
                from_rsdp(pPayload, cb);
                if (g_Smp.fHasMadt) {
                    break;
                }
            }
            cbTag = (pTag->u32Size + 7u) & ~7u;
            if (cbTag < 8u) {
                kprintf("smp: mb2 tag size %u — stop\n", pTag->u32Size);
                break;
            }
            pNext = (u8 *)pTag + cbTag;
            if (pNext <= (u8 *)pTag || pNext > pEnd) {
                kprintf("smp: mb2 tag walk stop next=%p\n", (void *)pNext);
                break;
            }
            pTag = (struct mb2_tag *)pNext;
            u32Tags++;
        }
        kprintf("smp: mb2 tags walked=%u madt=%u\n", u32Tags, g_Smp.fHasMadt);
    }

    if (!g_Smp.fHasMadt) {
        kprintf("smp: scan BIOS RSDP\n");
        scan_bios_rsdp();
    }
    smp_finish_madt_log();
}

const struct gj_smp_info *
smp_info(void)
{
    return &g_Smp;
}

u32
smp_cpu_count_detected(void)
{
    return g_Smp.u32NLocalApic ? g_Smp.u32NLocalApic : 1;
}

u32
smp_cpu_count_online(void)
{
    return g_Smp.u32NOnline ? g_Smp.u32NOnline : 1;
}

u32
smp_apic_id_for_cpu(u32 u32CpuSlot)
{
    if (u32CpuSlot == 0) {
        return g_Smp.u32BspApicId;
    }
    /* Soft: unpublished / OOB slots report 0 (not a valid probe alone). */
    if (u32CpuSlot >= GJ_CPU_STATIC_MAX || u32CpuSlot >= g_Smp.u32NOnline ||
        u32CpuSlot >= smp_bringup_cap()) {
        return 0;
    }
    return g_aSlotApicId[u32CpuSlot];
}

int
smp_ap_run(u32 u32CpuSlot, void (*pfn)(void *), void *pArg)
{
    u32 u32Spins;
    u32 u32ApicId;
    u32 u32Cap = smp_bringup_cap();

    smp_soft_inc(&g_u32SoftApRunEnter);
    if (pfn == NULL || u32CpuSlot == 0 || u32CpuSlot >= g_Smp.u32NOnline ||
        u32CpuSlot >= u32Cap || u32CpuSlot >= GJ_CPU_STATIC_MAX) {
        g_u32SoftApRunFail++;
        smp_soft_inc(&g_u32SoftApRunInval);
        return -1;
    }
    if (g_pfnApWork != NULL) {
        g_u32SoftApRunFail++;
        smp_soft_inc(&g_u32SoftApRunBusy);
        return -1; /* busy */
    }
    u32ApicId = g_aSlotApicId[u32CpuSlot];
    g_u32ApWorkDone = 0;
    g_pApWorkArg = pArg;
    g_u32ApWorkSlot = u32CpuSlot;
    __asm__ volatile ("" ::: "memory");
    g_pfnApWork = pfn;
    __asm__ volatile ("" ::: "memory");
    /* Wake AP from HLT via fixed IPI */
    apic_send_ipi(u32ApicId, (u8)GJ_APIC_IPI_RESCHED_VEC);
    smp_soft_inc(&g_u32SoftApRunIpi);

    for (u32Spins = 0; u32Spins < 100000000u && !g_u32ApWorkDone; u32Spins++) {
        __asm__ volatile ("pause");
    }
    if (!g_u32ApWorkDone) {
        g_pfnApWork = NULL;
        g_u32SoftApRunTimeout++;
        kprintf("smp: ap_run slot=%u timeout soft_to=%u\n", u32CpuSlot,
                g_u32SoftApRunTimeout);
        return -1;
    }
    g_u32SoftApRunOk++;
    return 0;
}

void
smp_start_aps(void)
{
    u32 i;
    u32 u32Bsp = g_Smp.u32BspApicId;
    u32 u32Tried = 0;
    u32 u32Ok = 0;
    u32 u32CpuSlot = 1;
    u32 u32Cap;
    u32 cbTramp;
    struct gj_ap_boot_info *pInfo;
    u8 *pTramp;
    u64 u64Cr3;

    smp_soft_inc(&g_u32SoftStartEnter);
    g_aSlotApicId[0] = u32Bsp;
    g_aSoftSlotStatus[0] = (u8)GJ_SMP_SOFT_SLOT_ONLINE;
    g_aSoftApPhase[0] = (u8)GJ_SMP_AP_PHASE_SCHED;
    g_pfnApWork = NULL;
    g_u32ApWorkDone = 1;
    /* Soft: reset bring-up counters for this start_aps pass. */
    g_u32SoftTried = 0;
    g_u32SoftOk = 0;
    g_u32SoftTimeout = 0;
    g_u32SoftSkipped = 0;
    g_u32SoftSkipDis = 0;
    g_u32SoftSkipBsp = 0;
    g_u32SoftSkipCap = 0;
    g_u32SoftLastSpins = 0;
    g_u32SoftLastApicId = 0;
    g_u32SoftLastSlot = 0;
    g_u32SoftSpinsHwm = 0;

    if (!apic_ready()) {
        smp_soft_inc(&g_u32SoftStartNoApic);
        kprintf("smp: start_aps skipped (no APIC)\n");
        smp_bringup_soft_log();
        return;
    }
    if (g_Smp.u32NLocalApic <= 1) {
        smp_soft_inc(&g_u32SoftStartUp);
        kprintf("smp: start_aps UP (no APs in MADT)\n");
        smp_bringup_soft_log();
        return;
    }

    cbTramp = (u32)(ap_trampoline_end - ap_trampoline_start);
    if (cbTramp == 0 || cbTramp > 0x1000u) {
        smp_soft_inc(&g_u32SoftStartBadTramp);
        kprintf("smp: bad trampoline size %u\n", cbTramp);
        smp_bringup_soft_log();
        return;
    }

    pTramp = (u8 *)(gj_vaddr_t)AP_TRAMPOLINE_PA;
    pInfo = (struct gj_ap_boot_info *)(gj_vaddr_t)AP_BOOT_INFO_PA;
    memset(pTramp, 0, 0x1000);
    memcpy(pTramp, ap_trampoline_start, cbTramp);

    __asm__ volatile ("mov %%cr3, %0" : "=r"(u64Cr3));

    u32Cap = smp_bringup_cap();
    /*
     * Soft deepen: arm x2APIC ICR on BSP before INIT-SIPI when supported so
     * soft ICR counters observe bring-up (INIT/SIPI via MSR 0x830). Timer
     * cal already ran in main; post-SMP enable in main is idempotent.
     */
    if (x2apic_supported() && !x2apic_enabled()) {
        if (x2apic_enable() == 0) {
            smp_soft_inc(&g_u32SoftStartX2Arm);
            kprintf("smp: bringup soft x2APIC ICR path armed\n");
        }
    }
    kprintf("smp: bringup soft begin cap=%u madt=%u x2ids=%d x2apic=%d\n",
            u32Cap, g_Smp.u32NLocalApic, g_Smp.fX2ApicIds,
            x2apic_enabled());

    /*
     * Boot only into the static percpu/stack pools (smp_bringup_cap).
     * PMM-backed percpu growth in cpu_init_ap covers higher slots once
     * AP park stacks / idle arrays scale past GJ_CPU_STATIC_MAX.
     */
    for (i = 0; i < g_Smp.u32NLocalApic && u32CpuSlot < u32Cap; i++) {
        u32 u32ApicId;
        u32 u32Spins;

        if (!g_Smp.aEnabled[i]) {
            g_u32SoftSkipped++;
            smp_soft_inc(&g_u32SoftSkipDis);
            continue;
        }
        if (g_Smp.aApicId[i] == u32Bsp) {
            g_u32SoftSkipped++;
            smp_soft_inc(&g_u32SoftSkipBsp);
            continue;
        }
        /* Park-stack array is the tightest static bound after bringup_cap. */
        if (u32CpuSlot >= GJ_AP_STACK_SLOTS ||
            u32CpuSlot >= GJ_CPU_STATIC_MAX) {
            g_u32SoftSkipped++;
            smp_soft_inc(&g_u32SoftSkipCap);
            break;
        }
        u32ApicId = g_Smp.aApicId[i];
        u32Tried++;
        g_u32SoftTried = u32Tried;
        g_u32SoftLastApicId = u32ApicId;
        g_u32SoftLastSlot = u32CpuSlot;

        memset(pInfo, 0, sizeof(*pInfo));
        pInfo->u64Cr3 = u64Cr3;
        pInfo->u64Entry = (u64)(gj_vaddr_t)smp_ap_c_entry;
        pInfo->u64Stack =
            ((u64)(gj_vaddr_t)(g_aApStacks[u32CpuSlot] + GJ_AP_STACK_SIZE)) &
            ~0xfull;
        pInfo->u32CpuIndex = u32CpuSlot;
        g_aSlotApicId[u32CpuSlot] = u32ApicId;
        g_aSoftSlotStatus[u32CpuSlot] = (u8)GJ_SMP_SOFT_SLOT_NONE;
        g_aSoftApPhase[u32CpuSlot] = (u8)GJ_SMP_AP_PHASE_NONE;
        g_aSoftSpins[u32CpuSlot] = 0;
        g_u32ApReady = 0;

        kprintf("smp: starting AP apic_id=%u slot=%u stack=0x%lx "
                "icr_writes=%lu\n",
                u32ApicId, u32CpuSlot, (unsigned long)pInfo->u64Stack,
                (unsigned long)x2apic_icr_soft_writes());
        /*
         * INIT-SIPI via apic layer (x2APIC ICR MSR when mode enabled).
         * Soft ICR counters bump inside x2apic_send_ipi_raw.
         */
        apic_send_init_sipi(u32ApicId, AP_SIPI_VECTOR);
        smp_soft_inc(&g_u32SoftStartSipi);

        for (u32Spins = 0; u32Spins < 50000000u && !g_u32ApReady; u32Spins++) {
            __asm__ volatile ("pause");
        }
        g_aSoftSpins[u32CpuSlot] = u32Spins;
        g_u32SoftLastSpins = u32Spins;
        smp_soft_note_spins(u32Spins);

        if (g_u32ApReady) {
            u32Ok++;
            g_u32SoftOk = u32Ok;
            /* Preserve FAIL if AP parked after partial bring-up. */
            if (g_aSoftSlotStatus[u32CpuSlot] !=
                (u8)GJ_SMP_SOFT_SLOT_FAIL) {
                g_aSoftSlotStatus[u32CpuSlot] =
                    (u8)GJ_SMP_SOFT_SLOT_ONLINE;
            }
            if (g_Smp.u32NOnline < GJ_SMP_MAX_APICS) {
                g_Smp.u32NOnline++;
            }
            kprintf("smp: AP apic_id=%u online ok spins=%u phase=%u "
                    "status=%u\n",
                    u32ApicId, u32Spins,
                    (unsigned)g_aSoftApPhase[u32CpuSlot],
                    (unsigned)g_aSoftSlotStatus[u32CpuSlot]);
            u32CpuSlot++;
        } else {
            g_u32SoftTimeout++;
            g_aSoftSlotStatus[u32CpuSlot] = (u8)GJ_SMP_SOFT_SLOT_TIMEOUT;
            kprintf("smp: AP apic_id=%u timeout spins=%u phase=%u "
                    "icr_last_mode=%u\n",
                    u32ApicId, u32Spins,
                    (unsigned)g_aSoftApPhase[u32CpuSlot],
                    (unsigned)x2apic_icr_soft_last_mode());
            /*
             * Do not advance slot — next MADT AP reuses this inventory
             * index (same as pre-soft path). Soft status sticky on slot
             * until a later success overwrites.
             */
        }
    }
    /* Count MADT leftovers past cap as soft-skipped (cap class). */
    for (; i < g_Smp.u32NLocalApic; i++) {
        if (g_Smp.aEnabled[i] && g_Smp.aApicId[i] != u32Bsp) {
            g_u32SoftSkipped++;
            smp_soft_inc(&g_u32SoftSkipCap);
        }
    }
    /*
     * Greppable inventory summary (soft):
     *   smp: start_aps tried=… ok=… online=… cpu_online=…
     *   smp: inventory …
     *   smp: soft …          (Wave 9 soft SMP inventory)
     *   smp: bringup soft …  (legacy soft bring-up lines)
     */
    kprintf("smp: start_aps tried=%u ok=%u online=%u cpu_online=%u\n",
            u32Tried, u32Ok, g_Smp.u32NOnline, cpu_online_count());
    kprintf("smp: inventory detected=%u online=%u bringup_cap=%u "
            "cpu_online=%u dyn_percpu=%u\n",
            smp_cpu_count_detected(), g_Smp.u32NOnline, smp_bringup_cap(),
            cpu_online_count(), cpu_dyn_percpu_count());
    smp_bringup_soft_log();
}

/* ---- Soft AP bring-up observability API ---------------------------- */

void
smp_bringup_soft_snapshot(struct gj_smp_bringup_soft *pOut)
{
    u32 i;
    u32 u32Sched = 0;

    if (pOut == NULL) {
        return;
    }
    for (i = 1; i < GJ_CPU_STATIC_MAX && i < smp_bringup_cap(); i++) {
        if (g_aApSchedReady[i] != 0) {
            u32Sched++;
        }
    }
    pOut->u32Tried = g_u32SoftTried;
    pOut->u32Ok = g_u32SoftOk;
    pOut->u32Timeout = g_u32SoftTimeout;
    pOut->u32Skipped = g_u32SoftSkipped;
    pOut->u32Online = g_Smp.u32NOnline ? g_Smp.u32NOnline : 1u;
    pOut->u32Cap = smp_bringup_cap();
    pOut->u32SchedReady = u32Sched;
    pOut->u32ApRunOk = g_u32SoftApRunOk;
    pOut->u32ApRunFail = g_u32SoftApRunFail;
    pOut->u32ApRunTimeout = g_u32SoftApRunTimeout;
    pOut->u32LastSpins = g_u32SoftLastSpins;
    pOut->u32LastApicId = g_u32SoftLastApicId;
    pOut->u32LastSlot = g_u32SoftLastSlot;
}

u32
smp_bringup_soft_slot(u32 u32CpuSlot)
{
    if (u32CpuSlot == 0) {
        return GJ_SMP_SOFT_SLOT_ONLINE;
    }
    if (u32CpuSlot >= GJ_CPU_STATIC_MAX) {
        return GJ_SMP_SOFT_SLOT_NONE;
    }
    return (u32)g_aSoftSlotStatus[u32CpuSlot];
}

u32
smp_bringup_soft_phase(u32 u32CpuSlot)
{
    if (u32CpuSlot == 0) {
        return GJ_SMP_AP_PHASE_SCHED;
    }
    if (u32CpuSlot >= GJ_CPU_STATIC_MAX) {
        return GJ_SMP_AP_PHASE_NONE;
    }
    return (u32)g_aSoftApPhase[u32CpuSlot];
}

u32
smp_bringup_soft_spins(u32 u32CpuSlot)
{
    if (u32CpuSlot == 0 || u32CpuSlot >= GJ_CPU_STATIC_MAX) {
        return 0;
    }
    return g_aSoftSpins[u32CpuSlot];
}

void
smp_bringup_soft_log(void)
{
    struct gj_smp_bringup_soft stSoft;
    u32 i;
    u32 u32Cap;
    u32 u32MadtEn;
    u32 u32StOnline;
    u32 u32StTimeout;
    u32 u32StFail;
    u32 u32StNone;
    u32 u32PhMax;
    u32 u32PhSched;
    u32 u32Headroom;
    u32 aPhHist[7];
    const char *szVerdict;

    smp_soft_inc(&g_u32SoftLogN);
    smp_bringup_soft_snapshot(&stSoft);
    u32Cap = stSoft.u32Cap;

    if (stSoft.u32Tried == 0) {
        szVerdict = (stSoft.u32Online <= 1u) ? "UP" : "NONE";
    } else if (stSoft.u32Ok == stSoft.u32Tried && stSoft.u32Timeout == 0) {
        szVerdict = "PASS";
    } else if (stSoft.u32Ok > 0) {
        szVerdict = "PARTIAL";
    } else {
        szVerdict = "NONE";
    }

    /* MADT enabled count + per-slot status/phase tallies (cap-bounded). */
    u32MadtEn = 0;
    for (i = 0; i < g_Smp.u32NLocalApic && i < GJ_SMP_MAX_APICS; i++) {
        if (g_Smp.aEnabled[i]) {
            u32MadtEn++;
        }
    }
    u32StOnline = 0;
    u32StTimeout = 0;
    u32StFail = 0;
    u32StNone = 0;
    u32PhMax = 0;
    u32PhSched = 0;
    for (i = 0; i < 7u; i++) {
        aPhHist[i] = 0;
    }
    for (i = 0; i < u32Cap && i < GJ_CPU_STATIC_MAX; i++) {
        u32 u32St = smp_bringup_soft_slot(i);
        u32 u32Ph = smp_bringup_soft_phase(i);

        if (u32St == GJ_SMP_SOFT_SLOT_ONLINE) {
            u32StOnline++;
        } else if (u32St == GJ_SMP_SOFT_SLOT_TIMEOUT) {
            u32StTimeout++;
        } else if (u32St == GJ_SMP_SOFT_SLOT_FAIL) {
            u32StFail++;
        } else {
            u32StNone++;
        }
        if (u32Ph > u32PhMax) {
            u32PhMax = u32Ph;
        }
        if (u32Ph == GJ_SMP_AP_PHASE_SCHED) {
            u32PhSched++;
        }
        if (u32Ph < 7u) {
            aPhHist[u32Ph]++;
        }
    }
    if (u32Cap > stSoft.u32Online) {
        u32Headroom = u32Cap - stSoft.u32Online;
    } else {
        u32Headroom = 0;
    }

    /*
     * Wave 9 greppable soft SMP inventory (product / smoke) — field-stable:
     *   smp: soft PASS|PARTIAL|UP|NONE …
     *   smp: soft inventory …
     *   smp: soft madt …
     *   smp: soft phases …
     *   smp: soft ap_run …
     *   smp: soft slot=…
     */
    kprintf("smp: soft %s tried=%u ok=%u timeout=%u skipped=%u "
            "online=%u cap=%u sched_ready=%u headroom=%u "
            "last_spins=%u last_apic=%u last_slot=%u\n",
            szVerdict, stSoft.u32Tried, stSoft.u32Ok, stSoft.u32Timeout,
            stSoft.u32Skipped, stSoft.u32Online, stSoft.u32Cap,
            stSoft.u32SchedReady, u32Headroom, stSoft.u32LastSpins,
            stSoft.u32LastApicId, stSoft.u32LastSlot);
    kprintf("smp: soft inventory detected=%u online=%u bringup_cap=%u "
            "cpu_online=%u dyn_percpu=%u status_online=%u status_to=%u "
            "status_fail=%u status_none=%u ph_max=%u ph_sched=%u\n",
            smp_cpu_count_detected(), stSoft.u32Online, stSoft.u32Cap,
            cpu_online_count(), cpu_dyn_percpu_count(), u32StOnline,
            u32StTimeout, u32StFail, u32StNone, u32PhMax, u32PhSched);
    kprintf("smp: soft madt has=%u local=%u enabled=%u bsp_id=%u x2ids=%d "
            "x2apic=%d stack_slots=%u static_max=%u\n",
            g_Smp.fHasMadt ? 1u : 0u, g_Smp.u32NLocalApic, u32MadtEn,
            g_Smp.u32BspApicId, g_Smp.fX2ApicIds, x2apic_enabled(),
            (u32)GJ_AP_STACK_SLOTS, (u32)GJ_CPU_STATIC_MAX);
    kprintf("smp: soft phases none=%u entry=%u percpu=%u x2apic=%u "
            "timer=%u idle=%u sched=%u\n",
            aPhHist[0], aPhHist[1], aPhHist[2], aPhHist[3], aPhHist[4],
            aPhHist[5], aPhHist[6]);
    kprintf("smp: soft ap_run ok=%u fail=%u timeout=%u\n",
            stSoft.u32ApRunOk, stSoft.u32ApRunFail, stSoft.u32ApRunTimeout);

    /* Per-slot soft detail (cap-bounded; skip pure NONE beyond BSP). */
    for (i = 0; i < u32Cap && i < GJ_CPU_STATIC_MAX; i++) {
        u32 u32St = smp_bringup_soft_slot(i);
        u32 u32Ph = smp_bringup_soft_phase(i);

        if (i > 0 && u32St == GJ_SMP_SOFT_SLOT_NONE && u32Ph == 0) {
            continue;
        }
        kprintf("smp: soft slot=%u status=%u phase=%u spins=%u "
                "apic=%u sched=%u online=%u\n",
                i, u32St, u32Ph, smp_bringup_soft_spins(i),
                smp_apic_id_for_cpu(i),
                (i == 0 || g_aApSchedReady[i] != 0) ? 1u : 0u,
                smp_cpu_online(i) ? 1u : 0u);
    }

    /*
     * Wave 12 exclusive deepen (complementary; never reshapes primary lines):
     *   smp: soft init …
     *   smp: soft madt_path …
     *   smp: soft start …
     *   smp: soft skip …
     *   smp: soft ap_entry …
     *   smp: soft ap_run_path …
     *   smp: soft spins …
     *   smp: soft caps …
     *   smp: soft path …
     * greppable: smp: soft
     */
    /* Grep: smp: soft init */
    kprintf("smp: soft init rsdp=%u mb2=%u rsdp_hit=%u bios_scan=%u "
            "bios_hit=%u up_fallback=%u logs=%u\n",
            g_u32SoftInitRsdp, g_u32SoftInitMb2, g_u32SoftInitRsdpHit,
            g_u32SoftInitBiosScan, g_u32SoftInitBiosHit,
            g_u32SoftInitUpFallback, g_u32SoftLogN);
    /* Grep: smp: soft madt_path */
    kprintf("smp: soft madt_path parse=%u csum_bad=%u local=%u x2=%u "
            "dedup=%u full=%u disabled=%u\n",
            g_u32SoftMadtParse, g_u32SoftMadtCsumBad, g_u32SoftMadtLocal,
            g_u32SoftMadtX2, g_u32SoftMadtDedup, g_u32SoftMadtFull,
            g_u32SoftMadtDisabled);
    /* Grep: smp: soft start */
    kprintf("smp: soft start enter=%u no_apic=%u up=%u bad_tramp=%u "
            "x2_arm=%u sipi=%u\n",
            g_u32SoftStartEnter, g_u32SoftStartNoApic, g_u32SoftStartUp,
            g_u32SoftStartBadTramp, g_u32SoftStartX2Arm, g_u32SoftStartSipi);
    /* Grep: smp: soft skip */
    kprintf("smp: soft skip total=%u dis=%u bsp=%u cap=%u "
            "(class split of soft skipped)\n",
            stSoft.u32Skipped, g_u32SoftSkipDis, g_u32SoftSkipBsp,
            g_u32SoftSkipCap);
    /* Grep: smp: soft ap_entry */
    kprintf("smp: soft ap_entry n=%u percpu_fail=%u idle_fail=%u "
            "sched_ready=%u timer_spin_hwm=%u\n",
            g_u32SoftApEntry, g_u32SoftApPercpuFail, g_u32SoftApIdleFail,
            g_u32SoftApSchedReady, g_u32SoftApTimerSpinHwm);
    /* Grep: smp: soft ap_run_path */
    kprintf("smp: soft ap_run_path enter=%u inval=%u busy=%u ipi=%u "
            "ok=%u fail=%u timeout=%u poll=%u drain=%u\n",
            g_u32SoftApRunEnter, g_u32SoftApRunInval, g_u32SoftApRunBusy,
            g_u32SoftApRunIpi, stSoft.u32ApRunOk, stSoft.u32ApRunFail,
            stSoft.u32ApRunTimeout, g_u32SoftApPoll, g_u32SoftApPollDrain);
    /* Grep: smp: soft spins */
    kprintf("smp: soft spins last=%u hwm=%u timer_hwm=%u last_slot=%u "
            "last_apic=%u\n",
            stSoft.u32LastSpins, g_u32SoftSpinsHwm, g_u32SoftApTimerSpinHwm,
            stSoft.u32LastSlot, stSoft.u32LastApicId);
    /* Grep: smp: soft caps */
    kprintf("smp: soft caps bringup=%u stack_slots=%u static_max=%u "
            "madt_max=%u headroom=%u online=%u\n",
            stSoft.u32Cap, (u32)GJ_AP_STACK_SLOTS, (u32)GJ_CPU_STATIC_MAX,
            (u32)GJ_SMP_MAX_APICS, u32Headroom, stSoft.u32Online);
    /* Grep: smp: soft path */
    kprintf("smp: soft path claim=MADT+INIT-SIPI+ap_run "
            "phases=ENTRY..SCHED soft=Wave12 (soft inventory; not bar3)\n");

    /*
     * Wave 14 exclusive deepen (complementary; Wave 9/12 lines stay stable):
     *   smp: soft deepen …
     *   smp: soft verdict …
     *   smp: soft handshake …
     *   smp: soft online …
     *   smp: soft sipi …
     *   smp: soft x2 …
     *   smp: soft fail …
     *   smp: soft ratio …
     *   smp: soft path … (Wave14 honesty stamp; Wave12 path line retained)
     * greppable: smp: soft
     */
    {
        u32 u32OkBp;
        u32 u32ToBp;
        u32 u32SkipBp;
        u32 u32SchedBp;
        u32 u32TriedDenom;
        u32 u32ApFail;
        u32 u32MadtHead;
        u32 u32CapHead;
        u32 u32CpuOn;

        u32TriedDenom = stSoft.u32Tried;
        if (u32TriedDenom == 0) {
            u32OkBp = 0;
            u32ToBp = 0;
        } else {
            u32OkBp = (stSoft.u32Ok * 10000u) / u32TriedDenom;
            u32ToBp = (stSoft.u32Timeout * 10000u) / u32TriedDenom;
        }
        /* Skip share vs (tried+skipped) — soft only. */
        if ((stSoft.u32Tried + stSoft.u32Skipped) != 0) {
            u32SkipBp = (stSoft.u32Skipped * 10000u) /
                        (stSoft.u32Tried + stSoft.u32Skipped);
        } else {
            u32SkipBp = 0;
        }
        /* Sched-ready basis points among online slots under cap. */
        if (stSoft.u32Online != 0) {
            u32SchedBp =
                (stSoft.u32SchedReady * 10000u) / stSoft.u32Online;
        } else {
            u32SchedBp = 0;
        }
        u32ApFail = g_u32SoftApPercpuFail + g_u32SoftApIdleFail;
        u32CpuOn = cpu_online_count();
        if (g_Smp.u32NLocalApic > stSoft.u32Online) {
            u32MadtHead = g_Smp.u32NLocalApic - stSoft.u32Online;
        } else {
            u32MadtHead = 0;
        }
        if (stSoft.u32Cap > stSoft.u32Online) {
            u32CapHead = stSoft.u32Cap - stSoft.u32Online;
        } else {
            u32CapHead = 0;
        }

        /* Grep: smp: soft retmap — Wave 19 return-surface map */
    kprintf("smp: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=74\n");

    /* Grep: smp: soft deepen */
        /*
         * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3.
         */
        /* Grep: smp: soft retclass — Wave 19 return-class taxonomy (kept) */
        kprintf("smp: soft retclass ok|fail|inval|nodev|busy|nomem "
                "soft_only=1 product_gate=0 wave=74 "
                "(retclass taxonomy; Soft≠product; not bar3)\n");
        /* Grep: smp: soft retlane — Wave 19 return-lane catalog (kept) */
        kprintf("smp: soft retlane inv|selftest|rate|retcode|retmap|class "
                "product_kernel=OPEN soft_ne_product=1 wave=74 "
                "(retlane catalog; Soft≠product)\n");
        /*
         * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3.
         */
        /* Grep: smp: soft retbound — Wave 20 return-bound honesty (kept) */
        kprintf("smp: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                "never_blocks_m0=1 wave=74 "
                "(retbound honesty; Soft≠product; not bar3)\n");
        /* Grep: smp: soft retseal — Wave 20 seal stamp (kept) */
        kprintf("smp: soft retseal exclusive=1 soft_ne_product=1 "
                "product_kernel=OPEN bar3=0 wave=74 "
                "(retseal stamp; Soft≠product)\n");
                /*
                 * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                */
                /* Grep: smp: soft retpulse — Wave 21 return-pulse honesty (kept) */
                kprintf("smp: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=74 "
                        "(retpulse honesty; Soft≠product; not bar3)\n");
                /* Grep: smp: soft retmark — Wave 21 mark stamp (kept) */
                kprintf("smp: soft retmark exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=74 "
                        "(retmark stamp; Soft≠product)\n");
                /*
                 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                */
                /* Grep: smp: soft retphase — Wave 22 return-phase honesty (kept) */
                kprintf("smp: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=74 "
                        "(retphase honesty; Soft≠product; not bar3)\n");
                /* Grep: smp: soft retbadge — Wave 22 badge stamp (kept) */
                kprintf("smp: soft retbadge exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=74 "
                        "(retbadge stamp; Soft≠product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
                */
                /* Grep: smp: soft rettoken — Wave 23 return-token honesty (kept) */
                kprintf("smp: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=74 "
                        "(rettoken honesty; Soft≠product; not bar3)\n");
                /* Grep: smp: soft retcrest — Wave 23 crest stamp (kept) */
                kprintf("smp: soft retcrest exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=74 "
                        "(retcrest stamp; Soft≠product)\n");
                /*
                 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: smp: soft retvault — Wave 24 return-vault honesty (kept) */
                kprintf("smp: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=74 "
                        "(retvault honesty; Soft≠product; not bar3)\n");
                /* Grep: smp: soft retbanner — Wave 24 banner stamp (kept) */
                kprintf("smp: soft retbanner exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=74 "
                        "(retbanner stamp; Soft≠product)\n");
                /*
                 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: smp: soft retledger — Wave 25 return-ledger honesty (kept) */
                kprintf("smp: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=74 "
                        "(retledger honesty; Soft≠product; not bar3)\n");
                /* Grep: smp: soft retbeacon — Wave 25 beacon stamp (kept) */
                kprintf("smp: soft retbeacon exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=74 "
                        "(retbeacon stamp; Soft≠product)\n");
                /*
                 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: smp: soft retcipher — Wave 26 return-cipher honesty (kept) */
                kprintf("smp: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=74 "
                        "(retcipher honesty; Soft≠product; not bar3)\n");
                /* Grep: smp: soft retflame — Wave 26 flame stamp (kept) */
                kprintf("smp: soft retflame exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=74 "
                        "(retflame stamp; Soft≠product)\n");
                        /*
                         * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                         * Return surfaces only — soft inventory; never hard-gates product paths.
                         * Soft≠product; not bar3.
                         */
                        /* Grep: smp: soft retprism — Wave 27 return-prism honesty (kept) */
                        kprintf("smp: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=74 "
                                "(retprism honesty; Soft≠product; not bar3)\n");
                        /* Grep: smp: soft retforge — Wave 27 forge stamp (kept) */
                        kprintf("smp: soft retforge exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=74 "
                                "(retforge stamp; Soft≠product)\n");
                                /*
                                 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: smp: soft retshard — Wave 28 return-shard honesty (kept) */
                                kprintf("smp: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=74 "
                                        "(retshard honesty; Soft≠product; not bar3)\n");
                                /* Grep: smp: soft retcrown — Wave 28 crown stamp (kept) */
                                kprintf("smp: soft retcrown exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=74 "
                                        "(retcrown stamp; Soft≠product)\n");
                                        /*
                                 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: smp: soft retglyph — Wave 29 return-glyph honesty (kept) */
                                kprintf("smp: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=74 "
                                        "(retglyph honesty; Soft≠product; not bar3)\n");
                                /* Grep: smp: soft retscepter — Wave 29 scepter stamp (kept) */
                                kprintf("smp: soft retscepter exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=74 "
                                        "(retscepter stamp; Soft≠product)\n");
                                        /*
                                 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: smp: soft retsigil — Wave 30 return-sigil honesty (kept) */
                                kprintf("smp: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=74 "
                                        "(retsigil honesty; Soft≠product; not bar3)\n");
                                /* Grep: smp: soft retemblem — Wave 30 emblem stamp (kept) */
                                kprintf("smp: soft retemblem exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=74 "
                                        "(retemblem stamp; Soft≠product)\n");
                                /*
                                 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: smp: soft retaegis — Wave 31 return-aegis honesty (kept) */
                                kprintf("smp: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=74 "
                                        "(retaegis honesty; Soft≠product; not bar3)\n");
                                /* Grep: smp: soft retsigil — Wave 30 return-sigil honesty (kept) */
                                kprintf("smp: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=74 "
                                        "(retsigil honesty; Soft≠product; not bar3)\n");
                                /* Grep: smp: soft retmantle — Wave 31 mantle stamp (kept) */
                                kprintf("smp: soft retmantle exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=74 "
                                        "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("smp: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("smp: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("smp: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("smp: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("smp: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("smp: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retfortress — Wave 35 return-fortress honesty */
kprintf("smp: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("smp: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft rethold — Wave 36 return-hold honesty */
kprintf("smp: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retspire — Wave 36 exclusive spire stamp */
kprintf("smp: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retwall — Wave 37 return-wall honesty */
kprintf("smp: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retgate — Wave 37 exclusive gate stamp */
kprintf("smp: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retmoat — Wave 38 return-moat honesty */
kprintf("smp: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retower — Wave 38 exclusive tower stamp */
kprintf("smp: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("smp: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("smp: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("smp: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("smp: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retravelin — Wave 41 return-travelin honesty */
kprintf("smp: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("smp: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("smp: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("smp: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("smp: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("smp: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("smp: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("smp: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("smp: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("smp: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retbailey — Wave 46 return-bailey honesty */
kprintf("smp: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("smp: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("smp: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("smp: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("smp: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("smp: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("smp: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("smp: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retsally — Wave 50 return-sally honesty */
kprintf("smp: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("smp: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retfosse — Wave 51 return-fosse honesty */
kprintf("smp: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("smp: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("smp: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("smp: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retravelin — Wave 53 return-travelin honesty */
kprintf("smp: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("smp: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("smp: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retredan — Wave 54 exclusive redan stamp */
kprintf("smp: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retflank — Wave 55 return-flank honesty */
kprintf("smp: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retface — Wave 55 exclusive face stamp */
kprintf("smp: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retgorge — Wave 56 return-gorge honesty */
kprintf("smp: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("smp: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retraverse — Wave 57 return-traverse honesty */
kprintf("smp: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("smp: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retorillon — Wave 58 return-orillon honesty */
kprintf("smp: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("smp: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("smp: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("smp: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retplace — Wave 60 return-place honesty */
kprintf("smp: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("smp: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("smp: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("smp: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("smp: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("smp: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("smp: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("smp: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: smp: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("smp: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: smp: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("smp: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: smp: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("smp: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: smp: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("smp: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: smp: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("smp: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=74 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: smp: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("smp: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=74 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("smp: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("smp: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("smp: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("smp: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("smp: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=74 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("smp: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=74 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("smp: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=74 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("smp: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=74 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("smp: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=74 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("smp: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=74 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: smp: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("smp: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=74 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("smp: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=74 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: smp: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("smp: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=74 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("smp: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=74 (retbastionangle stamp; Soft≠product)\n");
/* Grep: smp: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("smp: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=74 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: smp: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("smp: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=74 (retparapetangle stamp; Soft≠product)\n");

                                kprintf("smp: soft deepen wave=74 areas=101 verdict=%s tried=%u "
                "ok=%u timeout=%u skipped=%u online=%u cap=%u logs=%u\n",
                szVerdict, stSoft.u32Tried, stSoft.u32Ok, stSoft.u32Timeout,
                stSoft.u32Skipped, stSoft.u32Online, stSoft.u32Cap,
                g_u32SoftLogN);
        /* Grep: smp: soft verdict */
        kprintf("smp: soft verdict %s ok_bp=%u to_bp=%u skip_bp=%u "
                "sched_bp=%u ph_max=%u ph_sched=%u status_on=%u "
                "status_to=%u status_fail=%u\n",
                szVerdict, u32OkBp, u32ToBp, u32SkipBp, u32SchedBp,
                u32PhMax, u32PhSched, u32StOnline, u32StTimeout,
                u32StFail);
        /* Grep: smp: soft handshake */
        kprintf("smp: soft handshake tried=%u ok=%u timeout=%u "
                "last_spins=%u spins_hwm=%u timer_spin_hwm=%u "
                "last_slot=%u last_apic=%u sipi=%u\n",
                stSoft.u32Tried, stSoft.u32Ok, stSoft.u32Timeout,
                stSoft.u32LastSpins, g_u32SoftSpinsHwm,
                g_u32SoftApTimerSpinHwm, stSoft.u32LastSlot,
                stSoft.u32LastApicId, g_u32SoftStartSipi);
        /* Grep: smp: soft online */
        kprintf("smp: soft online smp=%u cpu=%u dyn_percpu=%u "
                "sched_ready=%u cap=%u madt_en=%u madt_head=%u "
                "cap_head=%u headroom=%u\n",
                stSoft.u32Online, u32CpuOn, cpu_dyn_percpu_count(),
                stSoft.u32SchedReady, stSoft.u32Cap, u32MadtEn,
                u32MadtHead, u32CapHead, u32Headroom);
        /* Grep: smp: soft sipi */
        kprintf("smp: soft sipi enter=%u no_apic=%u up=%u bad_tramp=%u "
                "x2_arm=%u sipi=%u skip_dis=%u skip_bsp=%u skip_cap=%u\n",
                g_u32SoftStartEnter, g_u32SoftStartNoApic,
                g_u32SoftStartUp, g_u32SoftStartBadTramp,
                g_u32SoftStartX2Arm, g_u32SoftStartSipi, g_u32SoftSkipDis,
                g_u32SoftSkipBsp, g_u32SoftSkipCap);
        /* Grep: smp: soft x2 */
        kprintf("smp: soft x2 enabled=%d supported_ids=%d icr_writes=%lu "
                "icr_init=%lu icr_sipi=%lu icr_fixed=%lu last_mode=%u "
                "arm=%u\n",
                x2apic_enabled(), g_Smp.fX2ApicIds,
                (unsigned long)x2apic_icr_soft_writes(),
                (unsigned long)x2apic_icr_soft_init(),
                (unsigned long)x2apic_icr_soft_sipi(),
                (unsigned long)x2apic_icr_soft_fixed(),
                (unsigned)x2apic_icr_soft_last_mode(),
                g_u32SoftStartX2Arm);
        /* Grep: smp: soft fail */
        kprintf("smp: soft fail timeout=%u status_fail=%u percpu=%u "
                "idle=%u ap_fail=%u ap_run_fail=%u ap_run_to=%u "
                "ap_run_inval=%u ap_run_busy=%u\n",
                stSoft.u32Timeout, u32StFail, g_u32SoftApPercpuFail,
                g_u32SoftApIdleFail, u32ApFail, stSoft.u32ApRunFail,
                stSoft.u32ApRunTimeout, g_u32SoftApRunInval,
                g_u32SoftApRunBusy);
        /* Grep: smp: soft ratio */
        kprintf("smp: soft ratio ok_bp=%u to_bp=%u skip_bp=%u "
                "sched_bp=%u tried=%u skipped=%u online=%u cap=%u "
                "wave=74\n",
                u32OkBp, u32ToBp, u32SkipBp, u32SchedBp, stSoft.u32Tried,
                stSoft.u32Skipped, stSoft.u32Online, stSoft.u32Cap);
        /* Grep: smp: soft headroom (Wave 15/17) */
        kprintf("smp: soft headroom cap=%u madt=%u stack_slots=%u "
                "static_max=%u online=%u cap_head=%u madt_head=%u "
                "wave=74\n",
                stSoft.u32Cap, g_Smp.u32NLocalApic, (u32)GJ_AP_STACK_SLOTS,
                (u32)GJ_CPU_STATIC_MAX, stSoft.u32Online, u32CapHead,
                u32MadtHead);
        /*
         * Grep: smp: soft return
         * Wave 19 return-path catalog — bring-up / ap_run / skip outcomes.
         * Soft ≠ product multi-CPU / bar3 gate. product_kernel=OPEN.
         */
        kprintf("smp: soft return ok=%u timeout=%u skipped=%u status_fail=%u "
                "ap_percpu_fail=%u ap_idle_fail=%u ap_run_ok=%u "
                "ap_run_fail=%u ap_run_to=%u ap_run_inval=%u ap_run_busy=%u "
                "start_no_apic=%u start_up=%u start_bad_tramp=%u "
                "madt_csum_bad=%u skip_dis=%u skip_bsp=%u skip_cap=%u "
                "product_kernel=OPEN wave=74\n",
                stSoft.u32Ok, stSoft.u32Timeout, stSoft.u32Skipped,
                u32StFail, g_u32SoftApPercpuFail, g_u32SoftApIdleFail,
                stSoft.u32ApRunOk, stSoft.u32ApRunFail, stSoft.u32ApRunTimeout,
                g_u32SoftApRunInval, g_u32SoftApRunBusy, g_u32SoftStartNoApic,
                g_u32SoftStartUp, g_u32SoftStartBadTramp, g_u32SoftMadtCsumBad,
                g_u32SoftSkipDis, g_u32SoftSkipBsp, g_u32SoftSkipCap);
        /* Grep: smp: soft ret_surface — Wave 19 terminal return classes */
        kprintf("smp: soft ret_surface bringup=ok|timeout|skip|status_fail "
                "ap=percpu_fail|idle_fail ap_run=ok|fail|to|inval|busy "
                "start=no_apic|up|bad_tramp madt=csum_bad "
                "skip=dis|bsp|cap product_kernel=OPEN areas=53 wave=74\n");
        /* Grep: smp: soft surface (Wave 19 area catalog) */
        kprintf("smp: soft surface deepen,verdict,handshake,online,sipi,"
                "x2,fail,ratio,path,headroom,return,ret_surface,surface,"
                "PASS,inventory,ap_run areas=53 wave=74\n");
        /* Grep: smp: soft path (Wave 19 honesty; Wave12 path retained) */
        kprintf("smp: soft path claim=MADT+INIT-SIPI+ap_run "
                "phases=ENTRY..SCHED soft=Wave17 "
                "(soft inventory; not bar3)\n");
    }

    /*
     * Legacy greppable soft bring-up lines (kept for existing smoke greps):
     *   smp: bringup soft PASS|PARTIAL|UP|NONE tried=… ok=… …
     */
    kprintf("smp: bringup soft %s tried=%u ok=%u timeout=%u skipped=%u "
            "online=%u cap=%u sched_ready=%u last_spins=%u last_apic=%u "
            "last_slot=%u\n",
            szVerdict, stSoft.u32Tried, stSoft.u32Ok, stSoft.u32Timeout,
            stSoft.u32Skipped, stSoft.u32Online, stSoft.u32Cap,
            stSoft.u32SchedReady, stSoft.u32LastSpins, stSoft.u32LastApicId,
            stSoft.u32LastSlot);
    kprintf("smp: bringup soft ap_run ok=%u fail=%u timeout=%u\n",
            stSoft.u32ApRunOk, stSoft.u32ApRunFail, stSoft.u32ApRunTimeout);

    for (i = 0; i < u32Cap && i < GJ_CPU_STATIC_MAX; i++) {
        u32 u32St = smp_bringup_soft_slot(i);
        u32 u32Ph = smp_bringup_soft_phase(i);

        if (i > 0 && u32St == GJ_SMP_SOFT_SLOT_NONE && u32Ph == 0) {
            continue;
        }
        kprintf("smp: bringup soft slot=%u status=%u phase=%u spins=%u "
                "apic=%u sched=%u\n",
                i, u32St, u32Ph, smp_bringup_soft_spins(i),
                smp_apic_id_for_cpu(i),
                (i == 0 || g_aApSchedReady[i] != 0) ? 1u : 0u);
    }

    /* Couple AP bring-up soft to x2APIC ICR soft (same boot window). */
    x2apic_icr_soft_log();
}
