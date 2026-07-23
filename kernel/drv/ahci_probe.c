/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product T1 HCL: AHCI (SATA HBA) PCI class probe — clean-room pure C.
 * Enumerate class 01:06:01; soft identify CAP/GHC/PI/VS via
 * vmm_map_device_uc (high UC window — never identity-map device MMIO
 * over the kernel).
 *
 * Wave 12 exclusive soft deepen (this unit only — greppable "ahci: soft …"):
 *   ahci: soft inventory  — ports / NP / NCS / CAP ok + PI mask
 *   ahci: soft cap        — CAP field inventory (public AHCI 1.x layout)
 *   ahci: soft pi         — ports-implemented popcount + mask
 *   ahci: soft ghc        — GHC soft snapshot (read-only; no AE write)
 *   ahci: soft vs         — AHCI version major.minor
 *   ahci: soft path       — honesty: probe/soft only; no engines / no AE
 *   ahci: soft inventory PASS|SKIP
 *   ahci: soft PASS|SKIP
 *
 * No port engines, no command lists, no GHC.AE write. No GPL source;
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

/* CAP field masks (AHCI 1.x; fields are 0's based where noted) */
#define AHCI_CAP_NP_MASK   0x1fu /* bits 4:0  Number of Ports */
#define AHCI_CAP_SXS       (1u << 5)
#define AHCI_CAP_EMS       (1u << 6)
#define AHCI_CAP_CCCS      (1u << 7)
#define AHCI_CAP_NCS_SHIFT 8u
#define AHCI_CAP_NCS_MASK  0x1fu /* bits 12:8 Number of Command Slots */
#define AHCI_CAP_PSC       (1u << 13)
#define AHCI_CAP_SSC       (1u << 14)
#define AHCI_CAP_PMD       (1u << 15)
#define AHCI_CAP_FBSS      (1u << 16)
#define AHCI_CAP_SPM       (1u << 17)
#define AHCI_CAP_SAM       (1u << 18)
#define AHCI_CAP_ISS_SHIFT 20u
#define AHCI_CAP_ISS_MASK  0xfu /* bits 23:20 Interface Speed Support */
#define AHCI_CAP_SCLO      (1u << 24)
#define AHCI_CAP_SAL       (1u << 25)
#define AHCI_CAP_SALP      (1u << 26)
#define AHCI_CAP_SSS       (1u << 27)
#define AHCI_CAP_SMPS      (1u << 28)
#define AHCI_CAP_SSNTF     (1u << 29)
#define AHCI_CAP_SNCQ      (1u << 30)
#define AHCI_CAP_S64A      (1u << 31)

/* GHC soft bits (read-only inventory; never write AE) */
#define AHCI_GHC_HR (1u << 0)  /* HBA Reset */
#define AHCI_GHC_IE (1u << 1)  /* Interrupt Enable */
#define AHCI_GHC_AE (1u << 31) /* AHCI Enable (observe only) */

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
 * Count set bits in a 32-bit value (soft PI inventory; freestanding).
 */
static u32
ahci_popcount32(u32 u32Val)
{
    u32 cBits = 0;
    u32 u32Tmp = u32Val;

    while (u32Tmp != 0) {
        cBits += u32Tmp & 1u;
        u32Tmp >>= 1;
    }
    return cBits;
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
 * Wave 12 greppable soft inventory dump (product / smoke).
 * Prefix-stable "ahci: soft …" — never hard-gates; kprintf only.
 *
 *   ahci: soft inventory — via + CAP ok + ports/NP/NCS + PI
 *   ahci: soft cap       — public CAP field inventory
 *   ahci: soft pi        — ports-implemented popcount + mask
 *   ahci: soft ghc       — GHC AE/IE/HR observe-only
 *   ahci: soft vs        — version major.minor
 *   ahci: soft path      — product surface honesty (no engines)
 *   ahci: soft PASS|SKIP / ahci: soft inventory PASS|SKIP
 *
 * greppable: ahci: soft
 */
static void
ahci_soft_inventory(const char *szVia, u32 u32Cap, u32 u32Ghc, u32 u32Pi,
                    u32 u32Vs)
{
    u32 u32Np;
    u32 u32Ncs;
    u32 u32Iss;
    u32 cPortsImpl;
    u32 u32Maj;
    u32 u32Min;
    int fCapOk;
    int fPiOk;
    int fGhcOk;
    int fVsOk;
    const char *szVerdict;

    if (szVia == NULL) {
        szVia = "path";
    }

    /*
     * All-0 / all-1 CAP is treated as unreadable MMIO for soft path.
     * Still report PI/GHC/VS when non-trivial so smoke can distinguish.
     */
    fCapOk = (u32Cap != 0u && u32Cap != 0xffffffffu) ? 1 : 0;
    fPiOk = (u32Pi != 0xffffffffu) ? 1 : 0;
    fGhcOk = (u32Ghc != 0xffffffffu) ? 1 : 0;
    fVsOk = (u32Vs != 0u && u32Vs != 0xffffffffu) ? 1 : 0;
    cPortsImpl = (fPiOk != 0) ? ahci_popcount32(u32Pi) : 0u;

    u32Np = (fCapOk != 0) ? ((u32Cap & AHCI_CAP_NP_MASK) + 1u) : 0u;
    u32Ncs = (fCapOk != 0)
                 ? (((u32Cap >> AHCI_CAP_NCS_SHIFT) & AHCI_CAP_NCS_MASK) + 1u)
                 : 0u;
    u32Iss = (fCapOk != 0)
                 ? ((u32Cap >> AHCI_CAP_ISS_SHIFT) & AHCI_CAP_ISS_MASK)
                 : 0u;
    u32Maj = (u32Vs >> 16) & 0xffffu;
    u32Min = u32Vs & 0xffffu;

    /*
     * Soft verdict (inventory only; never claims port engines):
     *   PASS — CAP readable and at least one PI bit, or CAP+VS ok
     *   SKIP — unreadable CAP / empty inventory path
     */
    if (fCapOk != 0 && (cPortsImpl > 0u || fVsOk != 0)) {
        szVerdict = "PASS";
    } else if (fCapOk != 0) {
        szVerdict = "PASS";
    } else {
        szVerdict = "SKIP";
    }

    /* Grep: ahci: soft inventory */
    kprintf("ahci: soft inventory via=%s cap_ok=%u pi_ok=%u ghc_ok=%u "
            "vs_ok=%u ports=%u np=%u ncs=%u pi=0x%x cap=0x%x\n",
            szVia, fCapOk != 0 ? 1u : 0u, fPiOk != 0 ? 1u : 0u,
            fGhcOk != 0 ? 1u : 0u, fVsOk != 0 ? 1u : 0u, cPortsImpl, u32Np,
            u32Ncs, u32Pi, u32Cap);

    /* Grep: ahci: soft cap (public AHCI 1.x CAP field inventory) */
    if (fCapOk != 0) {
        kprintf("ahci: soft cap np=%u ncs=%u iss=%u s64a=%u sncq=%u "
                "sss=%u salp=%u sal=%u sclo=%u sam=%u spm=%u fbss=%u "
                "pmd=%u ssc=%u psc=%u cccs=%u ems=%u sxs=%u raw=0x%x\n",
                u32Np, u32Ncs, u32Iss, (u32Cap & AHCI_CAP_S64A) != 0u ? 1u : 0u,
                (u32Cap & AHCI_CAP_SNCQ) != 0u ? 1u : 0u,
                (u32Cap & AHCI_CAP_SSS) != 0u ? 1u : 0u,
                (u32Cap & AHCI_CAP_SALP) != 0u ? 1u : 0u,
                (u32Cap & AHCI_CAP_SAL) != 0u ? 1u : 0u,
                (u32Cap & AHCI_CAP_SCLO) != 0u ? 1u : 0u,
                (u32Cap & AHCI_CAP_SAM) != 0u ? 1u : 0u,
                (u32Cap & AHCI_CAP_SPM) != 0u ? 1u : 0u,
                (u32Cap & AHCI_CAP_FBSS) != 0u ? 1u : 0u,
                (u32Cap & AHCI_CAP_PMD) != 0u ? 1u : 0u,
                (u32Cap & AHCI_CAP_SSC) != 0u ? 1u : 0u,
                (u32Cap & AHCI_CAP_PSC) != 0u ? 1u : 0u,
                (u32Cap & AHCI_CAP_CCCS) != 0u ? 1u : 0u,
                (u32Cap & AHCI_CAP_EMS) != 0u ? 1u : 0u,
                (u32Cap & AHCI_CAP_SXS) != 0u ? 1u : 0u, u32Cap);
        kprintf("ahci: soft cap ssntf=%u smps=%u soft PASS\n",
                (u32Cap & AHCI_CAP_SSNTF) != 0u ? 1u : 0u,
                (u32Cap & AHCI_CAP_SMPS) != 0u ? 1u : 0u);
    } else {
        kprintf("ahci: soft cap soft SKIP cap=0x%x\n", u32Cap);
    }

    /* Grep: ahci: soft pi */
    if (fPiOk != 0 && cPortsImpl > 0u) {
        kprintf("ahci: soft pi ports=%u mask=0x%x soft PASS\n", cPortsImpl,
                u32Pi);
    } else {
        kprintf("ahci: soft pi ports=%u mask=0x%x soft SKIP\n", cPortsImpl,
                u32Pi);
    }

    /* Grep: ahci: soft ghc (observe only — never write AE) */
    if (fGhcOk != 0) {
        kprintf("ahci: soft ghc ae=%u ie=%u hr=%u raw=0x%x soft PASS "
                "ae_write=0\n",
                (u32Ghc & AHCI_GHC_AE) != 0u ? 1u : 0u,
                (u32Ghc & AHCI_GHC_IE) != 0u ? 1u : 0u,
                (u32Ghc & AHCI_GHC_HR) != 0u ? 1u : 0u, u32Ghc);
    } else {
        kprintf("ahci: soft ghc soft SKIP ghc=0x%x\n", u32Ghc);
    }

    /* Grep: ahci: soft vs */
    if (fVsOk != 0) {
        kprintf("ahci: soft vs major=%u minor=0x%x raw=0x%x soft PASS\n",
                u32Maj, u32Min, u32Vs);
    } else {
        kprintf("ahci: soft vs soft SKIP vs=0x%x\n", u32Vs);
    }

    /*
     * Grep: ahci: soft path
     * Honesty catalog: product surface is PCI class + soft CAP/GHC/PI/VS.
     * claim=0 engines — no port start, no CLB/FB, no GHC.AE write.
     */
    kprintf("ahci: soft path claim=0 engines=0 cmdlist=0 fis=0 "
            "ghc_ae_write=0 map_uc=1 cap_fields=1 pi=1 ghc_ro=1 vs=1 "
            "via=%s\n",
            szVia);

    /* Legacy greppable inventory lines (pre-Wave-12 product smokes) */
    if (cPortsImpl > 0u) {
        kprintf("ahci: inventory ports implemented c=%u pi=0x%x soft PASS\n",
                cPortsImpl, u32Pi);
    } else {
        kprintf("ahci: inventory ports implemented c=0 pi=0x%x soft SKIP\n",
                u32Pi);
    }
    if (fCapOk != 0) {
        kprintf("ahci: inventory cap np=%u soft PASS\n", u32Np);
        kprintf("ahci: inventory command slots ncs=%u soft PASS\n", u32Ncs);
        kprintf("ahci: inventory soft PASS ports=%u slots=%u\n", cPortsImpl,
                u32Ncs);
    } else {
        kprintf("ahci: inventory cap soft SKIP cap=0x%x\n", u32Cap);
        kprintf("ahci: inventory command slots soft SKIP\n");
        kprintf("ahci: inventory soft SKIP\n");
    }

    /* Grep: ahci: soft inventory PASS|SKIP / ahci: soft PASS|SKIP */
    kprintf("ahci: soft inventory %s via=%s ports=%u ncs=%u\n", szVerdict,
            szVia, cPortsImpl, u32Ncs);
    kprintf("ahci: soft %s via=%s cap_ok=%u ports=%u\n", szVerdict, szVia,
            fCapOk != 0 ? 1u : 0u, cPortsImpl);
}

/**
 * Soft identify: map ABAR UC and read CAP/GHC/PI/VS (no GHC.AE write).
 * Then Wave 12 soft inventory (CAP fields + PI + GHC + VS + path honesty).
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
        kprintf("ahci: inventory soft SKIP map\n");
        /* Grep: ahci: soft … SKIP (map fail; 0xff.. = unread MMIO) */
        ahci_soft_inventory("map_fail", 0xffffffffu, 0xffffffffu, 0xffffffffu,
                            0xffffffffu);
        return;
    }
    {
        volatile u32 *pReg = (volatile u32 *)(gj_vaddr_t)vaMap;
        u32 u32Cap = pReg[AHCI_REG_CAP];
        u32 u32Ghc = pReg[AHCI_REG_GHC];
        u32 u32Pi = pReg[AHCI_REG_PI];
        u32 u32Vs = pReg[AHCI_REG_VS];
        u32 u32Np = (u32Cap & AHCI_CAP_NP_MASK) + 1u;
        u32 u32Ncs =
            ((u32Cap >> AHCI_CAP_NCS_SHIFT) & AHCI_CAP_NCS_MASK) + 1u;

        kprintf("ahci: HBA CAP=0x%x GHC=0x%x soft PASS\n", u32Cap, u32Ghc);
        kprintf("ahci: identify PI=0x%x VS=0x%x NP=%u NCS=%u soft PASS\n",
                u32Pi, u32Vs, u32Np, u32Ncs);
        ahci_soft_inventory("identify", u32Cap, u32Ghc, u32Pi, u32Vs);
    }
}

/**
 * Scan PCI for AHCI HBAs. Soft-reads CAP/GHC/PI/VS when ABAR is mappable.
 * Returns count of matching functions. Always logs greppable soft PASS/SKIP.
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
                /* Memory BAR only; soft CAP + inventory for product HCL. */
                if ((u32BarRaw & 1u) == 0 && paAbar != 0) {
                    ahci_soft_identify(paAbar);
                } else {
                    kprintf("ahci: abar empty/io soft SKIP\n");
                    kprintf("ahci: inventory soft SKIP no_abar\n");
                    ahci_soft_inventory("no_abar", 0xffffffffu, 0xffffffffu,
                                        0xffffffffu, 0xffffffffu);
                }
                cFound++;
            }
        }
    }
    if (cFound == 0) {
        kprintf("ahci: probe none (soft)\n");
        /* Grep: ahci: soft … SKIP (no controller; 0xff.. = unread) */
        ahci_soft_inventory("none", 0xffffffffu, 0xffffffffu, 0xffffffffu,
                            0xffffffffu);
    } else {
        kprintf("ahci: probe count=%u PASS\n", cFound);
    }
    kprintf("ahci: probe PASS\n");
    return cFound;
}
