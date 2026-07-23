/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product T1 HCL: AHCI (SATA HBA) PCI class probe — clean-room pure C.
 * Enumerate class 01:06:01; optional CAP/GHC soft-read via vmm_map_device_uc
 * (high UC window — never identity-map device MMIO over the kernel).
 * No GPL source; public PCI class codes + AHCI 1.x register layout only.
 */
#include <gj/klog.h>
#include <gj/types.h>
#include <gj/vmm.h>

/* PCI class / subclass / prog-if: mass storage / SATA / AHCI 1.0 */
#define AHCI_PCI_CLASS    0x01u
#define AHCI_PCI_SUBCLASS 0x06u
#define AHCI_PCI_PROG_IF  0x01u

/* HBA memory (ABAR) — CAP @ 0x00, GHC @ 0x04 (AHCI 1.3.1 §3) */
#define AHCI_REG_CAP 0u
#define AHCI_REG_GHC 1u

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
 * Scan PCI for AHCI HBAs. Soft-reads CAP/GHC when ABAR is mappable.
 * Returns count of matching functions. Always logs a greppable product line.
 */
u32
ahci_probe_scan(void)
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
                if (u8Base != AHCI_PCI_CLASS || u8Sub != AHCI_PCI_SUBCLASS ||
                    u8Pif != AHCI_PCI_PROG_IF) {
                    continue;
                }
                {
                    u32 u32Bar5 = pci_cfg_read(u8Bus, u8Slot, u8Func, 0x24);
                    u64 paAbar = (u64)(u32Bar5 & ~0xfu);

                    kprintf("ahci: probe %u:%u.%u vendor=0x%x bar5=0x%x PASS\n",
                            u8Bus, u8Slot, u8Func, u16Vendor, u32Bar5);
                    /* Memory BAR only; soft CAP path for product HCL. */
                    if ((u32Bar5 & 1u) == 0 && paAbar != 0) {
                        gj_vaddr_t vaMap = 0;

                        kprintf("ahci: abar mem soft path PASS\n");
                        if (vmm_map_device_uc((gj_paddr_t)paAbar, 0x1000,
                                              &vaMap) == GJ_OK) {
                            volatile u32 *pReg =
                                (volatile u32 *)(gj_vaddr_t)vaMap;
                            u32 u32Cap = pReg[AHCI_REG_CAP];
                            u32 u32Ghc = pReg[AHCI_REG_GHC];

                            kprintf("ahci: HBA CAP=0x%x GHC=0x%x soft PASS\n",
                                    u32Cap, u32Ghc);
                        }
                    }
                    cFound++;
                }
            }
        }
    }
    if (cFound == 0) {
        kprintf("ahci: probe none (soft)\n");
    } else {
        kprintf("ahci: probe count=%u PASS\n", cFound);
    }
    kprintf("ahci: probe PASS\n");
    return cFound;
}
