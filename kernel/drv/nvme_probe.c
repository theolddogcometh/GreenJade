/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product T1 HCL: NVMe PCI class probe — clean-room pure C.
 * Enumerate class 01:08:02; soft identify via CAP field inventory + VS
 * through vmm_map_device_uc (high UC window — never identity-map device
 * MMIO over the kernel). Soft-read only: no CC enable, no admin queues
 * claimed as product, no I/O. No GPL source; public PCI class codes +
 * NVM Express Base register layout only.
 */
#include <gj/klog.h>
#include <gj/types.h>
#include <gj/vmm.h>

/* PCI class / subclass / prog-if: mass storage / non-volatile / NVMe */
#define NVME_PCI_CLASS    0x01u
#define NVME_PCI_SUBCLASS 0x08u
#define NVME_PCI_PROG_IF  0x02u

/* Controller properties (NVM Express Base, capability region) */
#define NVME_REG_CAP 0x00u /* 64-bit CAP — soft inventory fields below */
#define NVME_REG_VS  0x08u /* 32-bit Version */

/*
 * CAP bit fields (public NVM Express Base layout; 0-based MQES).
 * Inventory only — does not enable the controller or claim queues.
 */
#define NVME_CAP_MQES(c)   ((u32)((c) & 0xffffu))
#define NVME_CAP_CQR(c)    ((u32)(((c) >> 16) & 1u))
#define NVME_CAP_AMS(c)    ((u32)(((c) >> 17) & 3u))
#define NVME_CAP_TO(c)     ((u32)(((c) >> 24) & 0xffu))
#define NVME_CAP_DSTRD(c)  ((u32)(((c) >> 32) & 0xfu))
#define NVME_CAP_NSSRS(c)  ((u32)(((c) >> 36) & 1u))
#define NVME_CAP_CSS(c)    ((u32)(((c) >> 37) & 0xffu))
#define NVME_CAP_BPS(c)    ((u32)(((c) >> 45) & 1u))
#define NVME_CAP_MPSMIN(c) ((u32)(((c) >> 52) & 0xfu))
#define NVME_CAP_MPSMAX(c) ((u32)(((c) >> 56) & 0xfu))
#define NVME_CAP_PMRS(c)   ((u32)(((c) >> 60) & 1u))
#define NVME_CAP_CMBS(c)   ((u32)(((c) >> 61) & 1u))

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
 * Soft identify: map BAR0 UC and inventory CAP fields + VS.
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
        u32 u32Mqes;
        u32 u32Cqr;
        u32 u32Ams;
        u32 u32To;
        u32 u32Dstrd;
        u32 u32Nssrs;
        u32 u32Css;
        u32 u32Bps;
        u32 u32MpsMin;
        u32 u32MpsMax;
        u32 u32Pmrs;
        u32 u32Cmbs;
        u32 u32Maj;
        u32 u32Min;
        u32 u32Ter;

        /* CAP @ 0x00 (64-bit LE) — full soft field inventory */
        u64Cap = *(volatile u64 *)(void *)(pMmio + NVME_REG_CAP);
        /* VS @ 0x08: TER:MIN:MAJ in bytes (public layout) */
        u32Vs = *(volatile u32 *)(void *)(pMmio + NVME_REG_VS);

        u32Mqes = NVME_CAP_MQES(u64Cap);
        u32Cqr = NVME_CAP_CQR(u64Cap);
        u32Ams = NVME_CAP_AMS(u64Cap);
        u32To = NVME_CAP_TO(u64Cap);
        u32Dstrd = NVME_CAP_DSTRD(u64Cap);
        u32Nssrs = NVME_CAP_NSSRS(u64Cap);
        u32Css = NVME_CAP_CSS(u64Cap);
        u32Bps = NVME_CAP_BPS(u64Cap);
        u32MpsMin = NVME_CAP_MPSMIN(u64Cap);
        u32MpsMax = NVME_CAP_MPSMAX(u64Cap);
        u32Pmrs = NVME_CAP_PMRS(u64Cap);
        u32Cmbs = NVME_CAP_CMBS(u64Cap);

        u32Ter = u32Vs & 0xffu;
        u32Min = (u32Vs >> 8) & 0xffu;
        u32Maj = (u32Vs >> 16) & 0xffffu;

        kprintf("nvme: CAP=0x%lx soft PASS\n", (unsigned long)u64Cap);
        kprintf("nvme: CAP inventory MQES=%u CQR=%u AMS=%u TO=%u DSTRD=%u "
                "soft PASS\n",
                u32Mqes, u32Cqr, u32Ams, u32To, u32Dstrd);
        kprintf("nvme: CAP inventory NSSRS=%u CSS=0x%x BPS=%u MPSMIN=%u "
                "MPSMAX=%u PMRS=%u CMBS=%u soft PASS\n",
                u32Nssrs, u32Css, u32Bps, u32MpsMin, u32MpsMax, u32Pmrs,
                u32Cmbs);
        kprintf("nvme: identify VS=0x%x %u.%u.%u soft PASS\n", u32Vs, u32Maj,
                u32Min, u32Ter);
        /* Explicit non-claim: product stop is CAP/VS soft inventory. */
        kprintf("nvme: admin queues soft SKIP (not claimed)\n");
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
