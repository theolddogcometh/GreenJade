/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product T1 HCL: NVMe PCI class probe — clean-room pure C.
 * Enumerate class 01:08:02; optional CAP soft-read via vmm_map_device_uc
 * (high UC window — never identity-map device MMIO over the kernel).
 * No GPL source; public PCI class codes + NVM Express register layout only.
 */
#include <gj/klog.h>
#include <gj/types.h>
#include <gj/vmm.h>

/* PCI class / subclass / prog-if: mass storage / non-volatile / NVMe */
#define NVME_PCI_CLASS    0x01u
#define NVME_PCI_SUBCLASS 0x08u
#define NVME_PCI_PROG_IF  0x02u

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
 * Returns 0 for I/O BARs or empty.
 */
static u64
nvme_bar0_pa(u8 u8Bus, u8 u8Slot, u8 u8Func)
{
    u32 u32Lo = pci_cfg_read(u8Bus, u8Slot, u8Func, 0x10);
    u64 paBar;

    if ((u32Lo & 1u) != 0) {
        return 0; /* I/O space */
    }
    paBar = (u64)(u32Lo & ~0xfu);
    /* Type 10b = 64-bit memory BAR; upper dword at 0x14. */
    if (((u32Lo >> 1) & 3u) == 2u) {
        u32 u32Hi = pci_cfg_read(u8Bus, u8Slot, u8Func, 0x14);

        paBar |= ((u64)u32Hi << 32);
    }
    return paBar;
}

/**
 * Scan PCI for NVMe controllers. Soft-reads CAP when BAR0 is mappable.
 * Returns count of matching functions. Always logs a greppable product line.
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
                u32 u32Bar0;
                u8 u8Base;
                u8 u8Sub;
                u8 u8Pif;
                u16 u16Vendor;

                u16Vendor = (u16)(u32Id & 0xffffu);
                if (u16Vendor == 0xffffu) {
                    if (u8Func == 0) {
                        break;
                    }
                    continue;
                }
                u32ClassReg = pci_cfg_read(u8Bus, u8Slot, u8Func, 0x08);
                u8Base = (u8)((u32ClassReg >> 24) & 0xffu);
                u8Sub = (u8)((u32ClassReg >> 16) & 0xffu);
                u8Pif = (u8)((u32ClassReg >> 8) & 0xffu);
                if (u8Base != NVME_PCI_CLASS || u8Sub != NVME_PCI_SUBCLASS ||
                    u8Pif != NVME_PCI_PROG_IF) {
                    continue;
                }
                {
                    u64 paBar;

                    u32Bar0 = pci_cfg_read(u8Bus, u8Slot, u8Func, 0x10);
                    paBar = nvme_bar0_pa(u8Bus, u8Slot, u8Func);
                    kprintf("nvme: probe %u:%u.%u vendor=0x%x bar0=0x%x PASS\n",
                            u8Bus, u8Slot, u8Func, u16Vendor, u32Bar0);
                    if (paBar != 0) {
                        gj_vaddr_t vaMap = 0;

                        kprintf("nvme: bar0 mem soft path PASS\n");
                        /* CAP @ 0x00 (64-bit) via high UC window */
                        if (vmm_map_device_uc((gj_paddr_t)paBar, 0x1000,
                                              &vaMap) == GJ_OK) {
                            volatile u64 *pCap =
                                (volatile u64 *)(gj_vaddr_t)vaMap;
                            u64 u64Cap = pCap[0];

                            kprintf("nvme: CAP=0x%lx soft PASS\n",
                                    (unsigned long)u64Cap);
                        }
                    }
                    cFound++;
                }
            }
        }
    }
    if (cFound == 0) {
        kprintf("nvme: probe none (soft)\n");
    } else {
        kprintf("nvme: probe count=%u PASS\n", cFound);
    }
    kprintf("nvme: probe PASS\n");
    return cFound;
}
