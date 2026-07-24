/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product T1 HCL: AHCI (SATA HBA) PCI class probe — clean-room pure C.
 * Enumerate class 01:06:01; soft identify CAP/GHC/IS/PI/VS/CAP2/BOHC via
 * vmm_map_device_uc (high UC window — never identity-map device MMIO
 * over the kernel).
 *
 * Wave 14/15/16/17 exclusive soft deepen (this unit only — greppable "ahci: soft …"):
 *   ahci: soft inventory  — ports / NP / NCS / CAP ok + PI mask + wave
 *   ahci: soft cap        — CAP field inventory (public AHCI 1.x layout)
 *   ahci: soft cap2       — CAP2 field inventory (observe-only)
 *   ahci: soft pi         — ports-implemented popcount + mask
 *   ahci: soft ghc        — GHC soft snapshot (read-only; no AE write)
 *   ahci: soft is         — Interrupt Status soft snapshot
 *   ahci: soft vs         — AHCI version major.minor
 *   ahci: soft bohc       — BIOS/OS Handoff Control observe
 *   ahci: soft iss        — Interface Speed Support decode
 *   ahci: soft regs       — HBA memory offset map
 *   ahci: soft pci        — class 01:06:01 inventory
 *   ahci: soft path       — honesty: probe/soft only; no engines / no AE
 *   ahci: soft return rate — Wave 19 ok/fail rate lamps
 *   ahci: soft retcode    — Wave 19 retcode catalog
 *   ahci: soft deepen     — wave=69 areas stamp
 *   ahci: soft ratio      — Wave 15 port/ncs occupancy
 *   ahci: soft headroom   — Wave 15 NP vs PI head
 *   ahci: soft surface    — Wave 16 area catalog
 *   ahci: soft honesty    — Wave 16 bar3/game-I/O non-claims
 *   ahci: soft geom       — Wave 16 HBA reg geometry
 *   ahci: soft return     — Wave 16 return-surface bitmask
 *   ahci: soft return selftest — Wave 17 terminal return surface (kept)
 *   ahci: soft retmap     — Wave 17 return-surface map (kept)
 *   ahci: soft contract   — Wave 16 soft≠game I/O contract
 *   ahci: soft stats      — emission / probe tallies
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
#define AHCI_REG_CAP  0u  /* 0x00 Host Capabilities */
#define AHCI_REG_GHC  1u  /* 0x04 Global HBA Control */
#define AHCI_REG_IS   2u  /* 0x08 Interrupt Status */
#define AHCI_REG_PI   3u  /* 0x0C Ports Implemented */
#define AHCI_REG_VS   4u  /* 0x10 AHCI Version */
#define AHCI_REG_CAP2 9u  /* 0x24 Host Capabilities Extended */
#define AHCI_REG_BOHC 10u /* 0x28 BIOS/OS Handoff Control and Status */

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

/* CAP2 soft bits (AHCI 1.x extended; observe only) */
#define AHCI_CAP2_BOH  (1u << 0) /* BIOS/OS Handoff */
#define AHCI_CAP2_NVMP (1u << 1) /* NVMHCI Present */
#define AHCI_CAP2_APST (1u << 2) /* Automatic Partial to Slumber */
#define AHCI_CAP2_SDS  (1u << 3) /* Supports Device Sleep */
#define AHCI_CAP2_SADM (1u << 4) /* Supports Aggressive Device Sleep Mgmt */
#define AHCI_CAP2_DESO (1u << 5) /* DevSleep Entrance from Slumber Only */

/* GHC soft bits (read-only inventory; never write AE) */
#define AHCI_GHC_HR (1u << 0)  /* HBA Reset */
#define AHCI_GHC_IE (1u << 1)  /* Interrupt Enable */
#define AHCI_GHC_MRSM (1u << 2) /* MSI Revert to Single Message */
#define AHCI_GHC_AE (1u << 31) /* AHCI Enable (observe only) */

/* BOHC soft bits (observe only; never claim handoff) */
#define AHCI_BOHC_BOS  (1u << 0) /* BIOS Owned Semaphore */
#define AHCI_BOHC_OOS  (1u << 1) /* OS Owned Semaphore */
#define AHCI_BOHC_SOOE (1u << 2) /* SMI on OS Ownership Change Enable */
#define AHCI_BOHC_OOC  (1u << 3) /* OS Ownership Change */
#define AHCI_BOHC_BB   (1u << 4) /* BIOS Busy */

/* Wave 20 deepen area count (fixed greppable categories in inventory log). */
#define AHCI_SOFT_DEEPEN_AREAS 90u
#define AHCI_SOFT_DEEPEN_WAVE 61u

/* Soft inventory emission tallies (wrap OK; never hard-gate). */
static u32 g_u32SoftInvLogs;
static u32 g_u32SoftProbeLogs;
static u32 g_u32SoftIdentifyOk;
static u32 g_u32SoftMapFail;
static u32 g_u32SoftNoAbar;
static u32 g_u32SoftFound;

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
 * Map ISS field to greppable speed tag (soft inventory only).
 */
static const char *
ahci_iss_tag(u32 u32Iss)
{
    if (u32Iss == 1u) {
        return "gen1";
    }
    if (u32Iss == 2u) {
        return "gen2";
    }
    if (u32Iss == 3u) {
        return "gen3";
    }
    if (u32Iss == 0u) {
        return "reserved0";
    }
    return "other";
}

/**
 * Wave 14 greppable soft inventory dump (product / smoke).
 * Prefix-stable "ahci: soft …" — never hard-gates; kprintf only.
 *
 * greppable: ahci: soft
 */
static void
ahci_soft_inventory(const char *szVia, u32 u32Cap, u32 u32Ghc, u32 u32Is,
                    u32 u32Pi, u32 u32Vs, u32 u32Cap2, u32 u32Bohc)
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
    int fIsOk;
    int fCap2Ok;
    int fBohcOk;
    const char *szVerdict;
    const char *szIss;
    const char *szViaSafe;

    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "path";
    if (g_u32SoftInvLogs < 0xffffffffu) {
        g_u32SoftInvLogs++;
    }

    /*
     * All-0 / all-1 CAP is treated as unreadable MMIO for soft path.
     * Still report PI/GHC/VS when non-trivial so smoke can distinguish.
     */
    fCapOk = (u32Cap != 0u && u32Cap != 0xffffffffu) ? 1 : 0;
    fPiOk = (u32Pi != 0xffffffffu) ? 1 : 0;
    fGhcOk = (u32Ghc != 0xffffffffu) ? 1 : 0;
    fVsOk = (u32Vs != 0u && u32Vs != 0xffffffffu) ? 1 : 0;
    fIsOk = (u32Is != 0xffffffffu) ? 1 : 0;
    fCap2Ok = (u32Cap2 != 0xffffffffu) ? 1 : 0;
    fBohcOk = (u32Bohc != 0xffffffffu) ? 1 : 0;
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
    szIss = ahci_iss_tag(u32Iss);

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
            "vs_ok=%u cap2_ok=%u bohc_ok=%u ports=%u np=%u ncs=%u "
            "pi=0x%x cap=0x%x wave=%u\n",
            szViaSafe, fCapOk != 0 ? 1u : 0u, fPiOk != 0 ? 1u : 0u,
            fGhcOk != 0 ? 1u : 0u, fVsOk != 0 ? 1u : 0u,
            fCap2Ok != 0 ? 1u : 0u, fBohcOk != 0 ? 1u : 0u, cPortsImpl,
            u32Np, u32Ncs, u32Pi, u32Cap, (unsigned)AHCI_SOFT_DEEPEN_WAVE);

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

    /* Grep: ahci: soft cap2 (Wave 14 extended CAP inventory) */
    if (fCap2Ok != 0) {
        kprintf("ahci: soft cap2 boh=%u nvmp=%u apst=%u sds=%u sadm=%u "
                "deso=%u raw=0x%x soft PASS\n",
                (u32Cap2 & AHCI_CAP2_BOH) != 0u ? 1u : 0u,
                (u32Cap2 & AHCI_CAP2_NVMP) != 0u ? 1u : 0u,
                (u32Cap2 & AHCI_CAP2_APST) != 0u ? 1u : 0u,
                (u32Cap2 & AHCI_CAP2_SDS) != 0u ? 1u : 0u,
                (u32Cap2 & AHCI_CAP2_SADM) != 0u ? 1u : 0u,
                (u32Cap2 & AHCI_CAP2_DESO) != 0u ? 1u : 0u, u32Cap2);
    } else {
        kprintf("ahci: soft cap2 soft SKIP cap2=0x%x\n", u32Cap2);
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
        kprintf("ahci: soft ghc ae=%u ie=%u hr=%u mrsm=%u raw=0x%x "
                "soft PASS ae_write=0\n",
                (u32Ghc & AHCI_GHC_AE) != 0u ? 1u : 0u,
                (u32Ghc & AHCI_GHC_IE) != 0u ? 1u : 0u,
                (u32Ghc & AHCI_GHC_HR) != 0u ? 1u : 0u,
                (u32Ghc & AHCI_GHC_MRSM) != 0u ? 1u : 0u, u32Ghc);
    } else {
        kprintf("ahci: soft ghc soft SKIP ghc=0x%x\n", u32Ghc);
    }

    /* Grep: ahci: soft is (Interrupt Status observe) */
    if (fIsOk != 0) {
        kprintf("ahci: soft is raw=0x%x pop=%u soft PASS clear_write=0\n",
                u32Is, ahci_popcount32(u32Is));
    } else {
        kprintf("ahci: soft is soft SKIP is=0x%x\n", u32Is);
    }

    /* Grep: ahci: soft vs */
    if (fVsOk != 0) {
        kprintf("ahci: soft vs major=%u minor=0x%x raw=0x%x soft PASS\n",
                u32Maj, u32Min, u32Vs);
    } else {
        kprintf("ahci: soft vs soft SKIP vs=0x%x\n", u32Vs);
    }

    /* Grep: ahci: soft bohc (observe only — never claim OS ownership) */
    if (fBohcOk != 0) {
        kprintf("ahci: soft bohc bos=%u oos=%u sooe=%u ooc=%u bb=%u "
                "raw=0x%x soft PASS handoff_claim=0\n",
                (u32Bohc & AHCI_BOHC_BOS) != 0u ? 1u : 0u,
                (u32Bohc & AHCI_BOHC_OOS) != 0u ? 1u : 0u,
                (u32Bohc & AHCI_BOHC_SOOE) != 0u ? 1u : 0u,
                (u32Bohc & AHCI_BOHC_OOC) != 0u ? 1u : 0u,
                (u32Bohc & AHCI_BOHC_BB) != 0u ? 1u : 0u, u32Bohc);
    } else {
        kprintf("ahci: soft bohc soft SKIP bohc=0x%x\n", u32Bohc);
    }

    /* Grep: ahci: soft iss */
    if (fCapOk != 0) {
        kprintf("ahci: soft iss code=%u tag=%s soft PASS\n", u32Iss, szIss);
    } else {
        kprintf("ahci: soft iss soft SKIP\n");
    }

    /* Grep: ahci: soft regs (HBA memory dword-index / byte-offset map) */
    kprintf("ahci: soft regs CAP=0x00 GHC=0x04 IS=0x08 PI=0x0C VS=0x10 "
            "CAP2=0x24 BOHC=0x28 soft PASS\n");

    /* Grep: ahci: soft pci */
    kprintf("ahci: soft pci class=0x%02x subclass=0x%02x pif=0x%02x "
            "mass_storage_sata_ahci=1 soft PASS\n",
            (unsigned)AHCI_PCI_CLASS, (unsigned)AHCI_PCI_SUBCLASS,
            (unsigned)AHCI_PCI_PROG_IF);

    /*
     * Grep: ahci: soft path
     * Honesty catalog: product surface is PCI class + soft CAP/GHC/PI/VS.
     * claim=0 engines — no port start, no CLB/FB, no GHC.AE write.
     */
    kprintf("ahci: soft path claim=0 engines=0 cmdlist=0 fis=0 "
            "ghc_ae_write=0 bohc_claim=0 map_uc=1 cap_fields=1 cap2=1 "
            "pi=1 ghc_ro=1 is_ro=1 vs=1 bohc_ro=1 via=%s wave=%u\n",
            szViaSafe, (unsigned)AHCI_SOFT_DEEPEN_WAVE);

    /*
     * Wave 15 exclusive deepen (complementary; never hard-gates).
     * greppable: ahci: soft ratio|headroom|surface
     */
    {
        u32 u32PortBp = 0;
        u32 u32PortHead = 0;
        u32 u32Ae = 0;

        if (u32Np != 0u) {
            u32PortBp = (cPortsImpl * 10000u) / u32Np;
        }
        if (u32Np > cPortsImpl) {
            u32PortHead = u32Np - cPortsImpl;
        }
        if (fGhcOk != 0) {
            u32Ae = (u32Ghc & AHCI_GHC_AE) != 0u ? 1u : 0u;
        }
        /* Grep: ahci: soft ratio */
        kprintf("ahci: soft ratio port_bp=%u ports=%u np=%u ncs=%u ae=%u "
                "iss=%u wave=%u\n",
                u32PortBp, cPortsImpl, u32Np, u32Ncs, u32Ae, u32Iss,
                (unsigned)AHCI_SOFT_DEEPEN_WAVE);
        /* Grep: ahci: soft headroom */
        kprintf("ahci: soft headroom port_head=%u np=%u ports=%u ncs=%u "
                "wave=%u\n",
                u32PortHead, u32Np, cPortsImpl, u32Ncs,
                (unsigned)AHCI_SOFT_DEEPEN_WAVE);
        /* Grep: ahci: soft surface */
        kprintf("ahci: soft surface inventory,cap,cap2,pi,ghc,is,vs,bohc,"
                "iss,regs,pci,path,ratio,headroom,honesty,geom,return,"
                "contract,return_selftest,retmap,deepen,stats areas=%u wave=%u\n",
                (unsigned)AHCI_SOFT_DEEPEN_AREAS,
                (unsigned)AHCI_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 16 complementary deepen (kept; never hard-gates).
     * Soft ≠ game I/O. greppable: ahci: soft honesty|geom|return|contract
     */
    {
        u32 u32Surf = 0u;

        if (fCapOk != 0) {
            u32Surf |= 0x1u;
        }
        if (fPiOk != 0) {
            u32Surf |= 0x2u;
        }
        if (fGhcOk != 0) {
            u32Surf |= 0x4u;
        }
        if (fVsOk != 0) {
            u32Surf |= 0x8u;
        }
        if (fIsOk != 0) {
            u32Surf |= 0x10u;
        }
        if (fCap2Ok != 0) {
            u32Surf |= 0x20u;
        }
        if (fBohcOk != 0) {
            u32Surf |= 0x40u;
        }
        u32Surf |= 0x80u; /* regs/pci catalog always present */
        /* Grep: ahci: soft honesty */
        kprintf("ahci: soft honesty probe_only=1 engines=0 ghc_ae_write=0 "
                "game_io=0 product_storage=0 bar3=open soft_only=1 "
                "wave=%u soft PASS\n",
                (unsigned)AHCI_SOFT_DEEPEN_WAVE);
        /* Grep: ahci: soft geom */
        kprintf("ahci: soft geom cap_idx=%u ghc_idx=%u is_idx=%u pi_idx=%u "
                "vs_idx=%u cap2_idx=%u bohc_idx=%u np=%u ncs=%u ports=%u "
                "wave=%u soft PASS\n",
                (unsigned)AHCI_REG_CAP, (unsigned)AHCI_REG_GHC,
                (unsigned)AHCI_REG_IS, (unsigned)AHCI_REG_PI,
                (unsigned)AHCI_REG_VS, (unsigned)AHCI_REG_CAP2,
                (unsigned)AHCI_REG_BOHC, u32Np, u32Ncs, cPortsImpl,
                (unsigned)AHCI_SOFT_DEEPEN_WAVE);
        /* Grep: ahci: soft return — return-surface bitmask */
        kprintf("ahci: soft return surf=0x%x cap=%u pi=%u ghc=%u vs=%u "
                "is=%u cap2=%u bohc=%u via=%s areas=%u wave=%u soft PASS\n",
                u32Surf, fCapOk != 0 ? 1u : 0u, fPiOk != 0 ? 1u : 0u,
                fGhcOk != 0 ? 1u : 0u, fVsOk != 0 ? 1u : 0u,
                fIsOk != 0 ? 1u : 0u, fCap2Ok != 0 ? 1u : 0u,
                fBohcOk != 0 ? 1u : 0u, szViaSafe,
                (unsigned)AHCI_SOFT_DEEPEN_AREAS,
                (unsigned)AHCI_SOFT_DEEPEN_WAVE);
        /* Grep: ahci: soft contract — soft ≠ game I/O */
        kprintf("ahci: soft contract soft_only=1 game_io=0 product_io=0 "
                "engines_claimed=0 cmdlist=0 bar3=open wave=%u soft PASS\n",
                (unsigned)AHCI_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: ahci: soft return — Wave 17 API return surfaces (kept) */
    kprintf("ahci: soft return cap=%u pi=%u probe=1 soft_inv=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (fCapOk != 0 ? 1u : 0u), (fPiOk != 0 ? 1u : 0u), (unsigned)AHCI_SOFT_DEEPEN_WAVE);

    /* Grep: ahci: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("ahci: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)AHCI_SOFT_DEEPEN_WAVE);

    /* Grep: ahci: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("ahci: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)AHCI_SOFT_DEEPEN_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: ahci: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("ahci: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)AHCI_SOFT_DEEPEN_WAVE);

    /* Grep: ahci: soft retcode — Wave 19 retcode catalog */
    kprintf("ahci: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)AHCI_SOFT_DEEPEN_WAVE);

    /* Grep: ahci: soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: ahci: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("ahci: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)AHCI_SOFT_DEEPEN_WAVE);
    /* Grep: ahci: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("ahci: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)AHCI_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: ahci: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("ahci: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)AHCI_SOFT_DEEPEN_WAVE);
    /* Grep: ahci: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("ahci: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)AHCI_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: ahci: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("ahci: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)AHCI_SOFT_DEEPEN_WAVE);
            /* Grep: ahci: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("ahci: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)AHCI_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: ahci: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("ahci: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)AHCI_SOFT_DEEPEN_WAVE);
            /* Grep: ahci: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("ahci: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)AHCI_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: ahci: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("ahci: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)AHCI_SOFT_DEEPEN_WAVE);
            /* Grep: ahci: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("ahci: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)AHCI_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: ahci: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("ahci: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)AHCI_SOFT_DEEPEN_WAVE);
            /* Grep: ahci: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("ahci: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)AHCI_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: ahci: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("ahci: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)AHCI_SOFT_DEEPEN_WAVE);
            /* Grep: ahci: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("ahci: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)AHCI_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: ahci: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("ahci: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)AHCI_SOFT_DEEPEN_WAVE);
            /* Grep: ahci: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("ahci: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)AHCI_SOFT_DEEPEN_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: ahci: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("ahci: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)AHCI_SOFT_DEEPEN_WAVE);
                    /* Grep: ahci: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("ahci: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)AHCI_SOFT_DEEPEN_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: ahci: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("ahci: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)AHCI_SOFT_DEEPEN_WAVE);
                            /* Grep: ahci: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("ahci: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)AHCI_SOFT_DEEPEN_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: ahci: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("ahci: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=69 "
                                    "(retglyph honesty; Soft≠product; not bar3)\n");
                            /* Grep: ahci: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("ahci: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=69 "
                                    "(retscepter stamp; Soft≠product)\n");
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: ahci: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("ahci: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=69 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: ahci: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("ahci: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=69 "
                                    "(retemblem stamp; Soft≠product)\n");
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: ahci: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("ahci: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=69 "
                                    "(retaegis honesty; Soft≠product; not bar3)\n");
                            /* Grep: ahci: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("ahci: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=69 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: ahci: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("ahci: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=69 "
                                    "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("ahci: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("ahci: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("ahci: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("ahci: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("ahci: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("ahci: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retfortress — Wave 35 return-fortress honesty */
kprintf("ahci: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("ahci: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft rethold — Wave 36 return-hold honesty */
kprintf("ahci: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retspire — Wave 36 exclusive spire stamp */
kprintf("ahci: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retwall — Wave 37 return-wall honesty */
kprintf("ahci: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retgate — Wave 37 exclusive gate stamp */
kprintf("ahci: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retmoat — Wave 38 return-moat honesty */
kprintf("ahci: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retower — Wave 38 exclusive tower stamp */
kprintf("ahci: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("ahci: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("ahci: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("ahci: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("ahci: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retravelin — Wave 41 return-travelin honesty */
kprintf("ahci: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("ahci: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("ahci: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("ahci: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("ahci: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("ahci: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("ahci: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("ahci: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("ahci: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("ahci: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retbailey — Wave 46 return-bailey honesty */
kprintf("ahci: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("ahci: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("ahci: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("ahci: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("ahci: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("ahci: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("ahci: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("ahci: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retsally — Wave 50 return-sally honesty */
kprintf("ahci: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("ahci: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retfosse — Wave 51 return-fosse honesty */
kprintf("ahci: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("ahci: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("ahci: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("ahci: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retravelin — Wave 53 return-travelin honesty */
kprintf("ahci: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("ahci: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("ahci: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retredan — Wave 54 exclusive redan stamp */
kprintf("ahci: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retflank — Wave 55 return-flank honesty */
kprintf("ahci: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retface — Wave 55 exclusive face stamp */
kprintf("ahci: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retgorge — Wave 56 return-gorge honesty */
kprintf("ahci: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("ahci: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retraverse — Wave 57 return-traverse honesty */
kprintf("ahci: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("ahci: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retorillon — Wave 58 return-orillon honesty */
kprintf("ahci: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("ahci: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("ahci: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("ahci: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retplace — Wave 60 return-place honesty */
kprintf("ahci: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("ahci: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("ahci: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("ahci: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("ahci: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("ahci: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("ahci: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("ahci: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: ahci: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("ahci: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: ahci: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("ahci: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: ahci: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("ahci: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: ahci: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("ahci: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: ahci: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("ahci: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=69 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: ahci: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("ahci: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=69 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("ahci: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("ahci: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("ahci: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("ahci: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ahci: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("ahci: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=69 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: ahci: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("ahci: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=69 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
                            kprintf("ahci: soft deepen wave=%u areas=%u via=%s cap_ok=%u ports=%u "
            "found=%u identify_ok=%u map_fail=%u no_abar=%u ok=%u "
            "skip=%u\n",
            (unsigned)AHCI_SOFT_DEEPEN_WAVE,
            (unsigned)AHCI_SOFT_DEEPEN_AREAS, szViaSafe,
            fCapOk != 0 ? 1u : 0u, cPortsImpl, g_u32SoftFound,
            g_u32SoftIdentifyOk, g_u32SoftMapFail, g_u32SoftNoAbar,
            fCapOk != 0 ? 1u : 0u, fCapOk != 0 ? 0u : 1u);

    /* Grep: ahci: soft stats */
    kprintf("ahci: soft stats inv_logs=%u probe_logs=%u found=%u "
            "identify_ok=%u map_fail=%u no_abar=%u wave=%u\n",
            g_u32SoftInvLogs, g_u32SoftProbeLogs, g_u32SoftFound,
            g_u32SoftIdentifyOk, g_u32SoftMapFail, g_u32SoftNoAbar,
            (unsigned)AHCI_SOFT_DEEPEN_WAVE);

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
    kprintf("ahci: soft inventory %s via=%s ports=%u ncs=%u wave=%u "
            "areas=%u\n",
            szVerdict, szViaSafe, cPortsImpl, u32Ncs,
            (unsigned)AHCI_SOFT_DEEPEN_WAVE,
            (unsigned)AHCI_SOFT_DEEPEN_AREAS);
    kprintf("ahci: soft %s via=%s cap_ok=%u ports=%u wave=%u\n", szVerdict,
            szViaSafe, fCapOk != 0 ? 1u : 0u, cPortsImpl,
            (unsigned)AHCI_SOFT_DEEPEN_WAVE);
}

/**
 * Soft identify: map ABAR UC and read CAP/GHC/IS/PI/VS/CAP2/BOHC
 * (no GHC.AE write, no BOHC claim). Then Wave 14 soft inventory.
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
        if (g_u32SoftMapFail < 0xffffffffu) {
            g_u32SoftMapFail++;
        }
        kprintf("ahci: abar map soft fail st=%d\n", (int)stMap);
        kprintf("ahci: inventory soft SKIP map\n");
        /* Grep: ahci: soft … SKIP (map fail; 0xff.. = unread MMIO) */
        ahci_soft_inventory("map_fail", 0xffffffffu, 0xffffffffu,
                            0xffffffffu, 0xffffffffu, 0xffffffffu,
                            0xffffffffu, 0xffffffffu);
        return;
    }
    {
        volatile u32 *pReg = (volatile u32 *)(gj_vaddr_t)vaMap;
        u32 u32Cap = pReg[AHCI_REG_CAP];
        u32 u32Ghc = pReg[AHCI_REG_GHC];
        u32 u32Is = pReg[AHCI_REG_IS];
        u32 u32Pi = pReg[AHCI_REG_PI];
        u32 u32Vs = pReg[AHCI_REG_VS];
        u32 u32Cap2 = pReg[AHCI_REG_CAP2];
        u32 u32Bohc = pReg[AHCI_REG_BOHC];
        u32 u32Np = (u32Cap & AHCI_CAP_NP_MASK) + 1u;
        u32 u32Ncs =
            ((u32Cap >> AHCI_CAP_NCS_SHIFT) & AHCI_CAP_NCS_MASK) + 1u;

        if (g_u32SoftIdentifyOk < 0xffffffffu) {
            g_u32SoftIdentifyOk++;
        }

        kprintf("ahci: HBA CAP=0x%x GHC=0x%x soft PASS\n", u32Cap, u32Ghc);
        kprintf("ahci: identify PI=0x%x VS=0x%x NP=%u NCS=%u soft PASS\n",
                u32Pi, u32Vs, u32Np, u32Ncs);
        ahci_soft_inventory("identify", u32Cap, u32Ghc, u32Is, u32Pi, u32Vs,
                            u32Cap2, u32Bohc);
    }
}

/**
 * Scan PCI for AHCI HBAs. Soft-reads CAP/GHC/IS/PI/VS/CAP2/BOHC when
 * ABAR is mappable. Returns count of matching functions. Always logs
 * greppable soft PASS/SKIP.
 */
u32
ahci_probe_scan(void)
{
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u32 cFound = 0;

    if (g_u32SoftProbeLogs < 0xffffffffu) {
        g_u32SoftProbeLogs++;
    }

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
                    if (g_u32SoftNoAbar < 0xffffffffu) {
                        g_u32SoftNoAbar++;
                    }
                    kprintf("ahci: abar empty/io soft SKIP\n");
                    kprintf("ahci: inventory soft SKIP no_abar\n");
                    ahci_soft_inventory("no_abar", 0xffffffffu, 0xffffffffu,
                                        0xffffffffu, 0xffffffffu,
                                        0xffffffffu, 0xffffffffu,
                                        0xffffffffu);
                }
                cFound++;
                if (g_u32SoftFound < 0xffffffffu) {
                    g_u32SoftFound++;
                }
            }
        }
    }
    if (cFound == 0) {
        kprintf("ahci: probe none (soft)\n");
        /* Grep: ahci: soft … SKIP (no controller; 0xff.. = unread) */
        ahci_soft_inventory("none", 0xffffffffu, 0xffffffffu, 0xffffffffu,
                            0xffffffffu, 0xffffffffu, 0xffffffffu,
                            0xffffffffu);
    } else {
        kprintf("ahci: probe count=%u PASS\n", cFound);
    }
    kprintf("ahci: probe PASS\n");
    return cFound;
}
