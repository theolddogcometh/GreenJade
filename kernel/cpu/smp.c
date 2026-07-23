/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * MADT (APIC) discovery + AP bring-up + one-shot AP work.
 * Online inventory: g_Smp + g_aSlotApicId; soft helpers for telemetry/affinity.
 * Bring-up is capped by static percpu/stack pools; PMM percpu growth is ready
 * in cpu_init_ap for higher ids once park stacks / idle arrays scale.
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

/* One-shot AP work for smp_ap_run (drained on AP schedule path). */
static void (*volatile g_pfnApWork)(void *);
static void *volatile g_pApWorkArg;
static volatile u32 g_u32ApWorkSlot;
static volatile u32 g_u32ApWorkDone;

void
smp_ap_poll_work(void)
{
    void (*pfn)(void *) = g_pfnApWork;
    u32 u32Cpu = cpu_id();

    if (pfn != NULL && g_u32ApWorkSlot == u32Cpu) {
        void *pArg = g_pApWorkArg;

        g_pfnApWork = NULL;
        __asm__ volatile ("" ::: "memory");
        pfn(pArg);
        g_u32ApWorkDone = 1;
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
        kprintf("smp: AP percpu missing cpu=%u — HLT park\n", u32CpuIndex);
        g_u32ApReady = 1;
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }
    /* Match BSP: enable x2APIC on this CPU before MSR-based LAPIC access. */
    if (x2apic_supported()) {
        (void)x2apic_enable();
    }
    /* Local APIC timer so HLT wakes; does not drive global mono (BSP only). */
    apic_timer_start_local();
    __asm__ volatile ("sti");
    u64Ticks0 = apic_timer_ticks_cpu(u32CpuIndex);
    for (u32Spins = 0; u32Spins < 50000000u; u32Spins++) {
        u64Ticks1 = apic_timer_ticks_cpu(u32CpuIndex);
        if (u64Ticks1 > u64Ticks0) {
            break;
        }
        __asm__ volatile ("pause");
    }
    kprintf("cpu: AP id=%u timer_ticks=%lu\n", u32CpuIndex,
            (unsigned long)apic_timer_ticks_cpu(u32CpuIndex));

    /* Per-CPU idle before signalling ready (serialize vs next SIPI). */
    if (u32CpuIndex >= GJ_CPU_STATIC_MAX ||
        thread_init_ap_idle(u32CpuIndex) != 0) {
        kprintf("smp: AP idle create failed cpu=%u — HLT park\n", u32CpuIndex);
        g_u32ApReady = 1;
        for (;;) {
            smp_ap_poll_work();
            __asm__ volatile ("hlt");
        }
    }
    if (u32CpuIndex < GJ_CPU_STATIC_MAX) {
        g_aApSchedReady[u32CpuIndex] = 1;
    }
    g_u32ApReady = 1;
    /* Greppable: smp: AP schedule ready */
    kprintf("smp: AP schedule ready cpu=%u\n", u32CpuIndex);
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
        return;
    }
    /* de-dup */
    for (i = 0; i < g_Smp.u32NLocalApic; i++) {
        if (g_Smp.aApicId[i] == u32ApicId) {
            return;
        }
    }
    i = g_Smp.u32NLocalApic++;
    g_Smp.aApicId[i] = u32ApicId;
    g_Smp.aEnabled[i] = fEnabled ? 1 : 0;
    if (fX2) {
        g_Smp.fX2ApicIds = 1;
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
        kprintf("smp: MADT checksum bad\n");
        return;
    }
    g_Smp.fHasMadt = 1;
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
                return;
            }
        }
    }
}

static void
smp_finish_madt_log(void)
{
    if (g_Smp.u32NLocalApic == 0) {
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
    kprintf("smp: init_from_rsdp pa=0x%lx\n", (unsigned long)u64Rsdp);
    memset(&g_Smp, 0, sizeof(g_Smp));
    g_Smp.u32NOnline = 1;
    g_Smp.u32BspApicId = 0;

    if (u64Rsdp != 0) {
        from_rsdp((const void *)(gj_vaddr_t)u64Rsdp, 36);
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

    if (pfn == NULL || u32CpuSlot == 0 || u32CpuSlot >= g_Smp.u32NOnline ||
        u32CpuSlot >= u32Cap || u32CpuSlot >= GJ_CPU_STATIC_MAX) {
        return -1;
    }
    if (g_pfnApWork != NULL) {
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

    for (u32Spins = 0; u32Spins < 100000000u && !g_u32ApWorkDone; u32Spins++) {
        __asm__ volatile ("pause");
    }
    if (!g_u32ApWorkDone) {
        g_pfnApWork = NULL;
        kprintf("smp: ap_run slot=%u timeout\n", u32CpuSlot);
        return -1;
    }
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
    u32 cbTramp;
    struct gj_ap_boot_info *pInfo;
    u8 *pTramp;
    u64 u64Cr3;

    g_aSlotApicId[0] = u32Bsp;
    g_pfnApWork = NULL;
    g_u32ApWorkDone = 1;

    if (!apic_ready()) {
        kprintf("smp: start_aps skipped (no APIC)\n");
        return;
    }
    if (g_Smp.u32NLocalApic <= 1) {
        kprintf("smp: start_aps UP (no APs in MADT)\n");
        return;
    }

    cbTramp = (u32)(ap_trampoline_end - ap_trampoline_start);
    if (cbTramp == 0 || cbTramp > 0x1000u) {
        kprintf("smp: bad trampoline size %u\n", cbTramp);
        return;
    }

    pTramp = (u8 *)(gj_vaddr_t)AP_TRAMPOLINE_PA;
    pInfo = (struct gj_ap_boot_info *)(gj_vaddr_t)AP_BOOT_INFO_PA;
    memset(pTramp, 0, 0x1000);
    memcpy(pTramp, ap_trampoline_start, cbTramp);

    __asm__ volatile ("mov %%cr3, %0" : "=r"(u64Cr3));

    /*
     * Boot only into the static percpu/stack pools (smp_bringup_cap).
     * PMM-backed percpu growth in cpu_init_ap covers higher slots once
     * AP park stacks / idle arrays scale past GJ_CPU_STATIC_MAX.
     */
    for (i = 0; i < g_Smp.u32NLocalApic && u32CpuSlot < smp_bringup_cap();
         i++) {
        u32 u32ApicId;
        u32 u32Spins;

        if (!g_Smp.aEnabled[i] || g_Smp.aApicId[i] == u32Bsp) {
            continue;
        }
        /* Park-stack array is the tightest static bound after bringup_cap. */
        if (u32CpuSlot >= GJ_AP_STACK_SLOTS ||
            u32CpuSlot >= GJ_CPU_STATIC_MAX) {
            break;
        }
        u32ApicId = g_Smp.aApicId[i];
        u32Tried++;

        memset(pInfo, 0, sizeof(*pInfo));
        pInfo->u64Cr3 = u64Cr3;
        pInfo->u64Entry = (u64)(gj_vaddr_t)smp_ap_c_entry;
        pInfo->u64Stack =
            ((u64)(gj_vaddr_t)(g_aApStacks[u32CpuSlot] + GJ_AP_STACK_SIZE)) &
            ~0xfull;
        pInfo->u32CpuIndex = u32CpuSlot;
        g_aSlotApicId[u32CpuSlot] = u32ApicId;
        g_u32ApReady = 0;

        kprintf("smp: starting AP apic_id=%u slot=%u stack=0x%lx\n",
                u32ApicId, u32CpuSlot, (unsigned long)pInfo->u64Stack);
        apic_send_init_sipi(u32ApicId, AP_SIPI_VECTOR);

        for (u32Spins = 0; u32Spins < 50000000u && !g_u32ApReady; u32Spins++) {
            __asm__ volatile ("pause");
        }
        if (g_u32ApReady) {
            u32Ok++;
            if (g_Smp.u32NOnline < GJ_SMP_MAX_APICS) {
                g_Smp.u32NOnline++;
            }
            u32CpuSlot++;
            kprintf("smp: AP apic_id=%u online ok\n", u32ApicId);
        } else {
            kprintf("smp: AP apic_id=%u timeout\n", u32ApicId);
        }
    }
    /*
     * Greppable inventory summary (soft):
     *   smp: start_aps tried=… ok=… online=… cpu_online=…
     *   smp: inventory …
     */
    kprintf("smp: start_aps tried=%u ok=%u online=%u cpu_online=%u\n",
            u32Tried, u32Ok, g_Smp.u32NOnline, cpu_online_count());
    kprintf("smp: inventory detected=%u online=%u bringup_cap=%u "
            "cpu_online=%u dyn_percpu=%u\n",
            smp_cpu_count_detected(), g_Smp.u32NOnline, smp_bringup_cap(),
            cpu_online_count(), cpu_dyn_percpu_count());
}
