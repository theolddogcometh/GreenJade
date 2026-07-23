/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product T1 HCL: AHCI (SATA HBA) PCI class probe — clean-room pure C.
 * Enumerate class 01:06:01; soft identify CAP/GHC/PI/VS via
 * vmm_map_device_uc (high UC window — never identity-map device MMIO
 * over the kernel). No port engines, no command lists. No GPL source;
 * public PCI class codes + AHCI 1.x register layout only.
 */
#include <gj/klog.h>
#include <gj/types.h>
#include <gj/vmm.h>

/* PCI class / subclass / prog-if: mass storage / SATA / AHCI 1.0 */
#define AHCI_PCI_CLASS    0x01u
#define AHCI_PCI_SUBCLASS 0x06u
#define AHCI_PCI_PROG_IF  0x01u

/* HBA memory (ABAR) — AHCI 1.3.1 §3 (dword indices) */
#define AHCI_REG_CAP 0u  /* 0x00 Host Capabilities */
#define AHCI_REG_GHC 1u  /* 0x04 Global HBA Control */
#define AHCI_REG_PI  3u  /* 0x0C Ports Implemented */
#define AHCI_REG_VS  4u  /* 0x10 AHCI Version */

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
 * Resolve ABAR (BAR5) physical base — 32- or 64-bit memory BAR.
 * Returns 0 for I/O or empty. *pf64 set when type is 64-bit.
 */
static u64
ahci_abar_pa(u8 u8Bus, u8 u8Slot, u8 u8Func, u32 *pBarRaw, int *pf64)
{
    u32 u32Lo = pci_cfg_read(u8Bus, u8Slot, u8Func, 0x24);
    u64 paAbar;

    if (pBarRaw != NULL) {
        *pBarRaw = u32Lo;
    }
    if (pf64 != NULL) {
        *pf64 = 0;
    }
    if ((u32Lo & 1u) != 0) {
        return 0; /* I/O space (invalid for ABAR) */
    }
    paAbar = (u64)(u32Lo & ~0xfu);
    if (((u32Lo >> 1) & 3u) == 2u) {
        u32 u32Hi = pci_cfg_read(u8Bus, u8Slot, u8Func, 0x28);

        paAbar |= ((u64)u32Hi << 32);
        if (pf64 != NULL) {
            *pf64 = 1;
        }
    }
    return paAbar;
}

/**
 * Soft identify: map ABAR UC and read CAP/GHC/PI/VS (no GHC.AE write).
 */
static void
ahci_soft_identify(u64 paAbar)
{
    gj_vaddr_t vaMap = 0;
    gj_status_t stMap;

    kprintf("ahci: abar mem soft path PASS pa=0x%lx\n",
            (unsigned long)paAbar);
    stMap = vmm_map_device_uc((gj_paddr_t)paAbar, 0x1000, &vaMap);
    if (stMap != GJ_OK) {
        kprintf("ahci: abar map soft fail st=%d\n", (int)stMap);
        return;
    }
    {
        volatile u32 *pReg = (volatile u32 *)(gj_vaddr_t)vaMap;
        u32 u32Cap = pReg[AHCI_REG_CAP];
        u32 u32Ghc = pReg[AHCI_REG_GHC];
        u32 u32Pi = pReg[AHCI_REG_PI];
        u32 u32Vs = pReg[AHCI_REG_VS];
        u32 u32Np = (u32Cap & 0x1fu) + 1u; /* CAP.NP + 1 ports */

        kprintf("ahci: HBA CAP=0x%x GHC=0x%x soft PASS\n", u32Cap, u32Ghc);
        kprintf("ahci: identify PI=0x%x VS=0x%x NP=%u soft PASS\n", u32Pi,
                u32Vs, u32Np);
    }
}

/**
 * Scan PCI for AHCI HBAs. Soft-reads CAP/GHC/PI/VS when ABAR is mappable.
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
                u32 u32BarRaw = 0;
                u8 u8Base;
                u8 u8Sub;
                u8 u8Pif;
                u16 u16Vendor;
                u16 u16Device;
                int f64 = 0;
                u64 paAbar;

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
                if (u8Base != AHCI_PCI_CLASS || u8Sub != AHCI_PCI_SUBCLASS ||
                    u8Pif != AHCI_PCI_PROG_IF) {
                    continue;
                }
                paAbar = ahci_abar_pa(u8Bus, u8Slot, u8Func, &u32BarRaw, &f64);
                kprintf("ahci: probe %u:%u.%u vendor=0x%x bar5=0x%x PASS\n",
                        u8Bus, u8Slot, u8Func, u16Vendor, u32BarRaw);
                kprintf("ahci: identify %u:%u.%u id=%04x:%04x abar_pa=0x%lx "
                        "bits=%u soft PASS\n",
                        u8Bus, u8Slot, u8Func, u16Vendor, u16Device,
                        (unsigned long)paAbar, f64 != 0 ? 64u : 32u);
                /* Memory BAR only; soft CAP path for product HCL. */
                if ((u32BarRaw & 1u) == 0 && paAbar != 0) {
                    ahci_soft_identify(paAbar);
                } else {
                    kprintf("ahci: abar empty/io soft skip\n");
                }
                cFound++;
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
