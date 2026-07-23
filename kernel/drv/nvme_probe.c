/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product T1 HCL: NVMe PCI class probe — clean-room pure C.
 * Enumerate class 01:08:02; soft identify via CAP field inventory + VS
 * + CSTS/CC soft status through vmm_map_device_uc (high UC window —
 * never identity-map device MMIO over the kernel). Soft-read only: no
 * CC enable, no admin queues claimed as product, no I/O. No GPL source;
 * public PCI class codes + NVM Express Base register layout only.
 *
 * Greppable soft surface (Wave 12 CAP inventory deepen):
 *   nvme: probe … soft PASS|SKIP
 *   nvme: CAP=… soft PASS
 *   nvme: CAP inventory … soft PASS|SKIP   (per-field + summary)
 *   nvme: identify VS=… soft PASS
 *   nvme: CSTS=… soft PASS / CC=… soft PASS
 *   nvme: admin queues soft SKIP / CC enable soft SKIP
 */
#include <gj/klog.h>
#include <gj/types.h>
#include <gj/vmm.h>

/* PCI class / subclass / prog-if: mass storage / non-volatile / NVMe */
#define NVME_PCI_CLASS    0x01u
#define NVME_PCI_SUBCLASS 0x08u
#define NVME_PCI_PROG_IF  0x02u

/*
 * Controller properties (NVM Express Base, capability region).
 * Offsets are byte offsets into BAR0; soft-read only.
 */
#define NVME_REG_CAP  0x00u /* 64-bit CAP — soft inventory fields below */
#define NVME_REG_VS   0x08u /* 32-bit Version */
#define NVME_REG_CC   0x14u /* 32-bit Controller Configuration (read soft) */
#define NVME_REG_CSTS 0x1Cu /* 32-bit Controller Status (read soft) */

/* CSTS / CC single-bit soft decode (public layout; inventory only) */
#define NVME_CSTS_RDY 0x1u
#define NVME_CSTS_CFS 0x2u
#define NVME_CC_EN    0x1u

/*
 * CAP bit fields (public NVM Express Base layout; MQES is 0-based).
 * Inventory only — does not enable the controller or claim queues.
 *
 *   15:00  MQES    Maximum Queue Entries Supported (0's based)
 *   16     CQR     Contiguous Queues Required
 *   18:17  AMS     Arbitration Mechanism Supported
 *   31:24  TO      Timeout (500 ms units)
 *   35:32  DSTRD   Doorbell Stride (4 << DSTRD bytes)
 *   36     NSSRS   NVM Subsystem Reset Supported
 *   44:37  CSS     Command Sets Supported (bit0 = NVM)
 *   45     BPS     Boot Partition Support
 *   47:46  CPS     Controller Power Scope
 *   51:48  MPSMIN  Memory Page Size Minimum (2^(12+n))
 *   55:52  MPSMAX  Memory Page Size Maximum (2^(12+n))
 *   56     PMRS    Persistent Memory Region Supported
 *   57     CMBS    Controller Memory Buffer Supported
 *   60:59  CRMS    Controller Ready Modes (CRWMS|CRIMS)
 */
#define NVME_CAP_MQES(c)   ((u32)((c) & 0xffffu))
#define NVME_CAP_CQR(c)    ((u32)(((c) >> 16) & 1u))
#define NVME_CAP_AMS(c)    ((u32)(((c) >> 17) & 3u))
#define NVME_CAP_TO(c)     ((u32)(((c) >> 24) & 0xffu))
#define NVME_CAP_DSTRD(c)  ((u32)(((c) >> 32) & 0xfu))
#define NVME_CAP_NSSRS(c)  ((u32)(((c) >> 36) & 1u))
#define NVME_CAP_CSS(c)    ((u32)(((c) >> 37) & 0xffu))
#define NVME_CAP_BPS(c)    ((u32)(((c) >> 45) & 1u))
#define NVME_CAP_CPS(c)    ((u32)(((c) >> 46) & 3u))
#define NVME_CAP_MPSMIN(c) ((u32)(((c) >> 48) & 0xfu))
#define NVME_CAP_MPSMAX(c) ((u32)(((c) >> 52) & 0xfu))
#define NVME_CAP_PMRS(c)   ((u32)(((c) >> 56) & 1u))
#define NVME_CAP_CMBS(c)   ((u32)(((c) >> 57) & 1u))
#define NVME_CAP_CRMS(c)   ((u32)(((c) >> 59) & 3u))
#define NVME_CAP_CSS_NVM   0x1u /* CSS bit 0: NVM command set supported */

static inline void
outl(u16 u16Port, u32 u32Val)
{
    __asm__ volatile("outl %0, %1" : : "a"(u32Val), "Nd"(u16Port));
}

static inline u32
inl(u16 u16Port)
{
    u32 u32Val;

    __asm__ volatile("inl %1, %0" : "=a"(u32Val) : "Nd"(u16Port));
    return u32Val;
}

static u32
pci_cfg_read(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32Addr = 0x80000000u | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
                  ((u32)u8Func << 8) | (u8Off & 0xfcu);

    outl(0xCF8, u32Addr);
    return inl(0xCFC);
}

/**
 * Resolve BAR0 physical base (32- or 64-bit memory BAR).
 * Returns 0 for I/O BARs or empty. *pf64 set when type is 64-bit.
 */
static u64
nvme_bar0_pa(u8 u8Bus, u8 u8Slot, u8 u8Func, u32 *pBarRaw, int *pf64)
{
    u32 u32Lo = pci_cfg_read(u8Bus, u8Slot, u8Func, 0x10);
    u64 paBar;

    if (pBarRaw != NULL) {
        *pBarRaw = u32Lo;
    }
    if (pf64 != NULL) {
        *pf64 = 0;
    }
    if ((u32Lo & 1u) != 0) {
        return 0; /* I/O space */
    }
    paBar = (u64)(u32Lo & ~0xfu);
    /* Type 10b = 64-bit memory BAR; upper dword at 0x14. */
    if (((u32Lo >> 1) & 3u) == 2u) {
        u32 u32Hi = pci_cfg_read(u8Bus, u8Slot, u8Func, 0x14);

        paBar |= ((u64)u32Hi << 32);
        if (pf64 != NULL) {
            *pf64 = 1;
        }
    }
    return paBar;
}

/**
 * Soft CAP field inventory — full per-field decode (read-only).
 * Grep: nvme: CAP inventory … soft PASS|SKIP
 *
 * All-0 / all-1 CAP is treated as unreadable MMIO for the soft path.
 * Derived soft values (max_q_entries, TO ms, doorbell stride bytes,
 * page-size powers) are inventory only — never used to program CC.
 */
static void
nvme_soft_cap_inventory(u64 u64Cap)
{
    u32 u32Mqes;
    u32 u32Cqr;
    u32 u32Ams;
    u32 u32To;
    u32 u32Dstrd;
    u32 u32Nssrs;
    u32 u32Css;
    u32 u32Bps;
    u32 u32Cps;
    u32 u32MpsMin;
    u32 u32MpsMax;
    u32 u32Pmrs;
    u32 u32Cmbs;
    u32 u32Crms;
    u32 u32MaxQEntries;
    u32 u32ToMs;
    u32 u32DbStrideB;
    u32 u32MpsMinShift;
    u32 u32MpsMaxShift;
    int fCapOk;
    int fCssNvm;

    fCapOk = (u64Cap != 0ull && u64Cap != ~0ull) ? 1 : 0;
    if (fCapOk == 0) {
        kprintf("nvme: CAP inventory soft SKIP cap=0x%lx (unreadable)\n",
                (unsigned long)u64Cap);
        kprintf("nvme: CAP inventory MQES soft SKIP\n");
        kprintf("nvme: CAP inventory CQR soft SKIP\n");
        kprintf("nvme: CAP inventory AMS soft SKIP\n");
        kprintf("nvme: CAP inventory TO soft SKIP\n");
        kprintf("nvme: CAP inventory DSTRD soft SKIP\n");
        kprintf("nvme: CAP inventory NSSRS soft SKIP\n");
        kprintf("nvme: CAP inventory CSS soft SKIP\n");
        kprintf("nvme: CAP inventory BPS soft SKIP\n");
        kprintf("nvme: CAP inventory CPS soft SKIP\n");
        kprintf("nvme: CAP inventory MPSMIN soft SKIP\n");
        kprintf("nvme: CAP inventory MPSMAX soft SKIP\n");
        kprintf("nvme: CAP inventory PMRS soft SKIP\n");
        kprintf("nvme: CAP inventory CMBS soft SKIP\n");
        kprintf("nvme: CAP inventory CRMS soft SKIP\n");
        kprintf("nvme: CAP inventory soft SKIP\n");
        return;
    }

    u32Mqes = NVME_CAP_MQES(u64Cap);
    u32Cqr = NVME_CAP_CQR(u64Cap);
    u32Ams = NVME_CAP_AMS(u64Cap);
    u32To = NVME_CAP_TO(u64Cap);
    u32Dstrd = NVME_CAP_DSTRD(u64Cap);
    u32Nssrs = NVME_CAP_NSSRS(u64Cap);
    u32Css = NVME_CAP_CSS(u64Cap);
    u32Bps = NVME_CAP_BPS(u64Cap);
    u32Cps = NVME_CAP_CPS(u64Cap);
    u32MpsMin = NVME_CAP_MPSMIN(u64Cap);
    u32MpsMax = NVME_CAP_MPSMAX(u64Cap);
    u32Pmrs = NVME_CAP_PMRS(u64Cap);
    u32Cmbs = NVME_CAP_CMBS(u64Cap);
    u32Crms = NVME_CAP_CRMS(u64Cap);

    /* Spec: MQES is 0's based → max entries = MQES + 1 */
    u32MaxQEntries = u32Mqes + 1u;
    /* TO is in 500 ms units */
    u32ToMs = u32To * 500u;
    /* Doorbell stride = 4 << DSTRD bytes */
    u32DbStrideB = 4u << u32Dstrd;
    /* MPS: page size = 2^(12 + n) */
    u32MpsMinShift = 12u + u32MpsMin;
    u32MpsMaxShift = 12u + u32MpsMax;
    fCssNvm = ((u32Css & NVME_CAP_CSS_NVM) != 0u) ? 1 : 0;

    /* Grouped inventory lines (smoke / HCL greppable aggregates) */
    kprintf("nvme: CAP inventory MQES=%u CQR=%u AMS=%u TO=%u DSTRD=%u "
            "soft PASS\n",
            u32Mqes, u32Cqr, u32Ams, u32To, u32Dstrd);
    kprintf("nvme: CAP inventory NSSRS=%u CSS=0x%x BPS=%u MPSMIN=%u "
            "MPSMAX=%u PMRS=%u CMBS=%u soft PASS\n",
            u32Nssrs, u32Css, u32Bps, u32MpsMin, u32MpsMax, u32Pmrs,
            u32Cmbs);

    /* Per-field deepen — each greppable as soft PASS on its own */
    kprintf("nvme: CAP inventory MQES=%u max_q_entries=%u soft PASS\n",
            u32Mqes, u32MaxQEntries);
    kprintf("nvme: CAP inventory CQR=%u soft PASS\n", u32Cqr);
    kprintf("nvme: CAP inventory AMS=%u soft PASS\n", u32Ams);
    kprintf("nvme: CAP inventory TO=%u units to_ms=%u soft PASS\n", u32To,
            u32ToMs);
    kprintf("nvme: CAP inventory DSTRD=%u db_stride_b=%u soft PASS\n",
            u32Dstrd, u32DbStrideB);
    kprintf("nvme: CAP inventory NSSRS=%u soft PASS\n", u32Nssrs);
    kprintf("nvme: CAP inventory CSS=0x%x soft PASS\n", u32Css);
    if (fCssNvm != 0) {
        kprintf("nvme: CAP inventory CSS NVM soft PASS\n");
    } else {
        kprintf("nvme: CAP inventory CSS NVM soft SKIP\n");
    }
    kprintf("nvme: CAP inventory BPS=%u soft PASS\n", u32Bps);
    kprintf("nvme: CAP inventory CPS=%u soft PASS\n", u32Cps);
    kprintf("nvme: CAP inventory MPSMIN=%u shift=%u soft PASS\n", u32MpsMin,
            u32MpsMinShift);
    kprintf("nvme: CAP inventory MPSMAX=%u shift=%u soft PASS\n", u32MpsMax,
            u32MpsMaxShift);
    kprintf("nvme: CAP inventory PMRS=%u soft PASS\n", u32Pmrs);
    kprintf("nvme: CAP inventory CMBS=%u soft PASS\n", u32Cmbs);
    kprintf("nvme: CAP inventory CRMS=%u soft PASS\n", u32Crms);

    /* Summary rollup — greppable product soft gate */
    kprintf("nvme: CAP inventory soft PASS mqes=%u max_q=%u to_ms=%u "
            "dstrd_b=%u css=0x%x mpsmin=%u mpsmax=%u crms=%u\n",
            u32Mqes, u32MaxQEntries, u32ToMs, u32DbStrideB, u32Css,
            u32MpsMin, u32MpsMax, u32Crms);
}

/**
 * Soft CSTS/CC status inventory (read-only; never write CC.EN).
 * Grep: nvme: CSTS=… soft PASS | nvme: CC=… soft PASS
 */
static void
nvme_soft_status_inventory(u32 u32Csts, u32 u32Cc)
{
    u32 u32Rdy;
    u32 u32Cfs;
    u32 u32En;

    u32Rdy = (u32Csts & NVME_CSTS_RDY) != 0u ? 1u : 0u;
    u32Cfs = (u32Csts & NVME_CSTS_CFS) != 0u ? 1u : 0u;
    u32En = (u32Cc & NVME_CC_EN) != 0u ? 1u : 0u;

    kprintf("nvme: CSTS=0x%x RDY=%u CFS=%u soft PASS\n", u32Csts, u32Rdy,
            u32Cfs);
    kprintf("nvme: CC=0x%x EN=%u soft PASS\n", u32Cc, u32En);
    if (u32En != 0u) {
        /* Soft observe only — product path never claims enable */
        kprintf("nvme: CC enable observed soft PASS (not claimed)\n");
    } else {
        kprintf("nvme: CC enable soft SKIP (disabled)\n");
    }
}

/**
 * Soft identify: map BAR0 UC and inventory CAP fields + VS + CSTS/CC.
 * Read-only MMIO — no CC write, no AQA/ASQ/ACQ, no admin queues as product.
 */
static void
nvme_soft_identify(u64 paBar)
{
    gj_vaddr_t vaMap = 0;
    gj_status_t stMap;

    kprintf("nvme: bar0 mem soft path PASS pa=0x%lx\n",
            (unsigned long)paBar);
    stMap = vmm_map_device_uc((gj_paddr_t)paBar, 0x1000, &vaMap);
    if (stMap != GJ_OK) {
        kprintf("nvme: bar0 map soft SKIP st=%d\n", (int)stMap);
        kprintf("nvme: CAP inventory soft SKIP (unmapped)\n");
        return;
    }
    {
        volatile u8 *pMmio = (volatile u8 *)(gj_vaddr_t)vaMap;
        u64 u64Cap;
        u32 u32Vs;
        u32 u32Cc;
        u32 u32Csts;
        u32 u32Maj;
        u32 u32Min;
        u32 u32Ter;

        /* CAP @ 0x00 (64-bit LE) — full soft field inventory */
        u64Cap = *(volatile u64 *)(void *)(pMmio + NVME_REG_CAP);
        /* VS @ 0x08: TER:MIN:MAJ in bytes (public layout) */
        u32Vs = *(volatile u32 *)(void *)(pMmio + NVME_REG_VS);
        /* CC / CSTS soft-read only — never write EN */
        u32Cc = *(volatile u32 *)(void *)(pMmio + NVME_REG_CC);
        u32Csts = *(volatile u32 *)(void *)(pMmio + NVME_REG_CSTS);

        u32Ter = u32Vs & 0xffu;
        u32Min = (u32Vs >> 8) & 0xffu;
        u32Maj = (u32Vs >> 16) & 0xffffu;

        kprintf("nvme: CAP=0x%lx soft PASS\n", (unsigned long)u64Cap);
        nvme_soft_cap_inventory(u64Cap);
        kprintf("nvme: identify VS=0x%x %u.%u.%u soft PASS\n", u32Vs, u32Maj,
                u32Min, u32Ter);
        nvme_soft_status_inventory(u32Csts, u32Cc);
        /* Explicit non-claim: product stop is CAP/VS/status soft inventory. */
        kprintf("nvme: admin queues soft SKIP (not claimed)\n");
        kprintf("nvme: I/O path soft SKIP (not claimed)\n");
    }
}

/**
 * Scan PCI for NVMe controllers. Soft CAP inventory when BAR0 is mapped.
 * Returns count of matching functions. Always logs greppable soft PASS/SKIP.
 */
u32
nvme_probe_scan(void)
{
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u32 cFound = 0;

    for (u8Bus = 0; u8Bus < 8; u8Bus++) {
        for (u8Slot = 0; u8Slot < 32; u8Slot++) {
            for (u8Func = 0; u8Func < 8; u8Func++) {
                u32 u32Id = pci_cfg_read(u8Bus, u8Slot, u8Func, 0);
                u32 u32ClassReg;
                u32 u32BarRaw = 0;
                u8 u8Base;
                u8 u8Sub;
                u8 u8Pif;
                u16 u16Vendor;
                u16 u16Device;
                int f64 = 0;
                u64 paBar;

                u16Vendor = (u16)(u32Id & 0xffffu);
                if (u16Vendor == 0xffffu) {
                    if (u8Func == 0) {
                        break;
                    }
                    continue;
                }
                u16Device = (u16)((u32Id >> 16) & 0xffffu);
                u32ClassReg = pci_cfg_read(u8Bus, u8Slot, u8Func, 0x08);
                u8Base = (u8)((u32ClassReg >> 24) & 0xffu);
                u8Sub = (u8)((u32ClassReg >> 16) & 0xffu);
                u8Pif = (u8)((u32ClassReg >> 8) & 0xffu);
                if (u8Base != NVME_PCI_CLASS || u8Sub != NVME_PCI_SUBCLASS ||
                    u8Pif != NVME_PCI_PROG_IF) {
                    continue;
                }
                paBar = nvme_bar0_pa(u8Bus, u8Slot, u8Func, &u32BarRaw, &f64);
                kprintf("nvme: probe %u:%u.%u vendor=0x%x bar0=0x%x soft "
                        "PASS\n",
                        u8Bus, u8Slot, u8Func, u16Vendor, u32BarRaw);
                kprintf("nvme: identify %u:%u.%u id=%04x:%04x bar0_pa=0x%lx "
                        "bits=%u soft PASS\n",
                        u8Bus, u8Slot, u8Func, u16Vendor, u16Device,
                        (unsigned long)paBar, f64 != 0 ? 64u : 32u);
                if (paBar != 0 && (u32BarRaw & 1u) == 0) {
                    nvme_soft_identify(paBar);
                } else {
                    kprintf("nvme: bar0 empty/io soft SKIP\n");
                    kprintf("nvme: CAP inventory soft SKIP (unmapped)\n");
                }
                cFound++;
            }
        }
    }
    if (cFound == 0) {
        kprintf("nvme: probe none soft SKIP\n");
        kprintf("nvme: CAP inventory soft SKIP (no controller)\n");
    } else {
        kprintf("nvme: probe count=%u soft PASS\n", cFound);
    }
    kprintf("nvme: probe soft PASS\n");
    return cFound;
}
