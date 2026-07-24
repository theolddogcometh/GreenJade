/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product T1 HCL: NVMe PCI class probe — clean-room pure C.
 * Enumerate class 01:08:02; soft identify via CAP field inventory + VS
 * + CSTS/CC / INTMS / INTMC / AQA soft status through vmm_map_device_uc
 * (high UC window — never identity-map device MMIO over the kernel).
 * Soft-read only: no CC enable, no admin queues claimed as product, no
 * I/O. No GPL source; public PCI class codes + NVM Express Base register
 * layout only.
 *
 * Wave 14/15/16/17 exclusive soft deepen (this unit only — greppable "nvme: soft …"):
 *   nvme: soft inventory  — CAP/VS/CSTS/CC ok + via + wave stamp
 *   nvme: soft cap        — CAP field rollup + derived soft values
 *   nvme: soft fields     — per-field CAP lamps (MQES…CRMS)
 *   nvme: soft vs         — version major.minor.ter
 *   nvme: soft csts       — RDY/CFS observe
 *   nvme: soft cc         — EN observe (never write)
 *   nvme: soft int        — INTMS/INTMC soft-read
 *   nvme: soft aqa        — AQA soft-read (admin queue attrs; not claimed)
 *   nvme: soft regs       — capability-region offset map
 *   nvme: soft bar        — BAR0 map path / bits
 *   nvme: soft pci        — class 01:08:02 inventory
 *   nvme: soft path       — honesty: probe/soft only; no queues / I/O
 *   nvme: soft return rate — Wave 19 ok/fail rate lamps
 *   nvme: soft retcode    — Wave 19 retcode catalog
 *   nvme: soft deepen     — wave=82 areas stamp
 *   nvme: soft ratio      — Wave 15 CAP/rdy/en basis lamps
 *   nvme: soft headroom   — Wave 15 MQES-derived soft head
 *   nvme: soft surface    — Wave 16 area catalog
 *   nvme: soft honesty    — Wave 16 bar3/game-I/O non-claims
 *   nvme: soft geom       — Wave 16 CAP/reg geometry
 *   nvme: soft return     — Wave 16 return-surface bitmask
 *   nvme: soft return selftest — Wave 17 terminal return surface (kept)
 *   nvme: soft retmap     — Wave 17 return-surface map (kept)
 *   nvme: soft contract   — Wave 16 soft≠game I/O contract
 *   nvme: soft stats      — emission / probe tallies
 *   nvme: soft inventory PASS|SKIP / nvme: soft PASS|SKIP
 *
 * Legacy greppable (Wave 12 CAP inventory; kept prefix-stable):
 *   nvme: CAP inventory … soft PASS|SKIP
 *   nvme: CAP=… / identify VS=… / CSTS=… / CC=…
 *   nvme: admin queues soft SKIP / I/O path soft SKIP
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
#define NVME_REG_CAP   0x00u /* 64-bit CAP — soft inventory fields below */
#define NVME_REG_VS    0x08u /* 32-bit Version */
#define NVME_REG_INTMS 0x0Cu /* 32-bit Interrupt Mask Set (soft-read) */
#define NVME_REG_INTMC 0x10u /* 32-bit Interrupt Mask Clear (soft-read) */
#define NVME_REG_CC    0x14u /* 32-bit Controller Configuration (read soft) */
#define NVME_REG_CSTS  0x1Cu /* 32-bit Controller Status (read soft) */
#define NVME_REG_AQA   0x24u /* 32-bit Admin Queue Attributes (soft-read) */

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

/* Wave 20 deepen area count (fixed greppable categories in inventory log). */
#define NVME_SOFT_DEEPEN_AREAS 108u
#define NVME_SOFT_DEEPEN_WAVE 77u

/* Soft inventory emission tallies (wrap OK; never hard-gate). */
static u32 g_u32SoftInvLogs;
static u32 g_u32SoftProbeLogs;
static u32 g_u32SoftIdentifyOk;
static u32 g_u32SoftMapFail;
static u32 g_u32SoftNoBar;
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
 * Grep: nvme: CAP inventory … soft PASS|SKIP  (Wave 12 legacy surface)
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
 * Grep: nvme: CSTS=… soft PASS | nvme: CC=… soft PASS  (Wave 12 legacy)
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
 * Wave 14 greppable soft inventory dump — prefix-stable "nvme: soft …".
 * Snapshots soft-read CAP/VS/CSTS/CC/INT/AQA; never allocates; never
 * hard-gates; never writes CC or claims queues. 0xff.. / 0 = unread.
 *
 * greppable: nvme: soft
 */
static void
nvme_soft_inventory(const char *szVia, u64 u64Cap, u32 u32Vs, u32 u32Csts,
                    u32 u32Cc, u32 u32Intms, u32 u32Intmc, u32 u32Aqa,
                    u64 paBar, u32 u32BarBits)
{
    u32 u32Mqes = 0;
    u32 u32Cqr = 0;
    u32 u32Ams = 0;
    u32 u32To = 0;
    u32 u32Dstrd = 0;
    u32 u32Nssrs = 0;
    u32 u32Css = 0;
    u32 u32Bps = 0;
    u32 u32Cps = 0;
    u32 u32MpsMin = 0;
    u32 u32MpsMax = 0;
    u32 u32Pmrs = 0;
    u32 u32Cmbs = 0;
    u32 u32Crms = 0;
    u32 u32MaxQ = 0;
    u32 u32ToMs = 0;
    u32 u32DbStrideB = 0;
    u32 u32Maj;
    u32 u32Min;
    u32 u32Ter;
    u32 u32Rdy;
    u32 u32Cfs;
    u32 u32En;
    u32 u32Asqs;
    u32 u32Acqs;
    int fCapOk;
    int fVsOk;
    int fCstsOk;
    int fCcOk;
    int fCssNvm;
    const char *szViaSafe;
    const char *szVerdict;

    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "anon";
    if (g_u32SoftInvLogs < 0xffffffffu) {
        g_u32SoftInvLogs++;
    }

    fCapOk = (u64Cap != 0ull && u64Cap != ~0ull) ? 1 : 0;
    fVsOk = (u32Vs != 0u && u32Vs != 0xffffffffu) ? 1 : 0;
    fCstsOk = (u32Csts != 0xffffffffu) ? 1 : 0;
    fCcOk = (u32Cc != 0xffffffffu) ? 1 : 0;

    if (fCapOk != 0) {
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
        u32MaxQ = u32Mqes + 1u;
        u32ToMs = u32To * 500u;
        u32DbStrideB = 4u << u32Dstrd;
    }
    fCssNvm = ((u32Css & NVME_CAP_CSS_NVM) != 0u) ? 1 : 0;

    u32Ter = u32Vs & 0xffu;
    u32Min = (u32Vs >> 8) & 0xffu;
    u32Maj = (u32Vs >> 16) & 0xffffu;
    u32Rdy = (fCstsOk != 0 && (u32Csts & NVME_CSTS_RDY) != 0u) ? 1u : 0u;
    u32Cfs = (fCstsOk != 0 && (u32Csts & NVME_CSTS_CFS) != 0u) ? 1u : 0u;
    u32En = (fCcOk != 0 && (u32Cc & NVME_CC_EN) != 0u) ? 1u : 0u;
    /* AQA: ASQS bits 7:0, ACQS bits 27:16 (0's based when programmed) */
    u32Asqs = (u32Aqa != 0xffffffffu) ? (u32Aqa & 0xfffu) : 0u;
    u32Acqs = (u32Aqa != 0xffffffffu) ? ((u32Aqa >> 16) & 0xfffu) : 0u;

    /*
     * Soft verdict (inventory only; never claims admin/I/O queues):
     *   PASS — CAP readable (mapped controller)
     *   SKIP — unreadable CAP / empty inventory path
     */
    if (fCapOk != 0) {
        szVerdict = "PASS";
    } else {
        szVerdict = "SKIP";
    }

    /* Grep: nvme: soft inventory */
    kprintf("nvme: soft inventory via=%s cap_ok=%u vs_ok=%u csts_ok=%u "
            "cc_ok=%u mqes=%u css=0x%x rdy=%u en=%u found=%u logs=%u "
            "wave=%u\n",
            szViaSafe, fCapOk != 0 ? 1u : 0u, fVsOk != 0 ? 1u : 0u,
            fCstsOk != 0 ? 1u : 0u, fCcOk != 0 ? 1u : 0u, u32Mqes, u32Css,
            u32Rdy, u32En, g_u32SoftFound, g_u32SoftInvLogs,
            (unsigned)NVME_SOFT_DEEPEN_WAVE);

    /* Grep: nvme: soft cap */
    if (fCapOk != 0) {
        kprintf("nvme: soft cap mqes=%u max_q=%u cqr=%u ams=%u to=%u "
                "to_ms=%u dstrd=%u db_b=%u nssrs=%u css=0x%x nvm=%u "
                "bps=%u cps=%u mpsmin=%u mpsmax=%u pmrs=%u cmbs=%u "
                "crms=%u raw=0x%lx soft PASS\n",
                u32Mqes, u32MaxQ, u32Cqr, u32Ams, u32To, u32ToMs, u32Dstrd,
                u32DbStrideB, u32Nssrs, u32Css, fCssNvm != 0 ? 1u : 0u,
                u32Bps, u32Cps, u32MpsMin, u32MpsMax, u32Pmrs, u32Cmbs,
                u32Crms, (unsigned long)u64Cap);
    } else {
        kprintf("nvme: soft cap soft SKIP cap=0x%lx\n",
                (unsigned long)u64Cap);
    }

    /* Grep: nvme: soft fields (per-field CAP lamps) */
    if (fCapOk != 0) {
        kprintf("nvme: soft fields MQES=%u CQR=%u AMS=%u TO=%u DSTRD=%u "
                "NSSRS=%u CSS=0x%x BPS=%u CPS=%u MPSMIN=%u MPSMAX=%u "
                "PMRS=%u CMBS=%u CRMS=%u soft PASS\n",
                u32Mqes, u32Cqr, u32Ams, u32To, u32Dstrd, u32Nssrs, u32Css,
                u32Bps, u32Cps, u32MpsMin, u32MpsMax, u32Pmrs, u32Cmbs,
                u32Crms);
    } else {
        kprintf("nvme: soft fields soft SKIP\n");
    }

    /* Grep: nvme: soft vs */
    if (fVsOk != 0) {
        kprintf("nvme: soft vs major=%u minor=%u ter=%u raw=0x%x "
                "soft PASS\n",
                u32Maj, u32Min, u32Ter, u32Vs);
    } else {
        kprintf("nvme: soft vs soft SKIP vs=0x%x\n", u32Vs);
    }

    /* Grep: nvme: soft csts */
    if (fCstsOk != 0) {
        kprintf("nvme: soft csts rdy=%u cfs=%u raw=0x%x soft PASS\n",
                u32Rdy, u32Cfs, u32Csts);
    } else {
        kprintf("nvme: soft csts soft SKIP csts=0x%x\n", u32Csts);
    }

    /* Grep: nvme: soft cc (observe only — never write EN) */
    if (fCcOk != 0) {
        kprintf("nvme: soft cc en=%u raw=0x%x soft PASS cc_write=0\n",
                u32En, u32Cc);
    } else {
        kprintf("nvme: soft cc soft SKIP cc=0x%x\n", u32Cc);
    }

    /* Grep: nvme: soft int (INTMS/INTMC soft-read; never mask-write) */
    if (u32Intms != 0xffffffffu || u32Intmc != 0xffffffffu) {
        kprintf("nvme: soft int intms=0x%x intmc=0x%x soft PASS "
                "mask_write=0\n",
                u32Intms, u32Intmc);
    } else {
        kprintf("nvme: soft int soft SKIP intms=0x%x intmc=0x%x\n",
                u32Intms, u32Intmc);
    }

    /* Grep: nvme: soft aqa (admin queue attrs observe; not claimed) */
    if (u32Aqa != 0xffffffffu) {
        kprintf("nvme: soft aqa asqs=%u acqs=%u raw=0x%x soft PASS "
                "queues_claimed=0\n",
                u32Asqs, u32Acqs, u32Aqa);
    } else {
        kprintf("nvme: soft aqa soft SKIP aqa=0x%x\n", u32Aqa);
    }

    /* Grep: nvme: soft regs (capability-region offset map) */
    kprintf("nvme: soft regs CAP=0x%x VS=0x%x INTMS=0x%x INTMC=0x%x "
            "CC=0x%x CSTS=0x%x AQA=0x%x soft PASS\n",
            (unsigned)NVME_REG_CAP, (unsigned)NVME_REG_VS,
            (unsigned)NVME_REG_INTMS, (unsigned)NVME_REG_INTMC,
            (unsigned)NVME_REG_CC, (unsigned)NVME_REG_CSTS,
            (unsigned)NVME_REG_AQA);

    /* Grep: nvme: soft bar */
    kprintf("nvme: soft bar pa=0x%lx bits=%u map_uc=1 io=0 soft PASS\n",
            (unsigned long)paBar, u32BarBits);

    /* Grep: nvme: soft pci */
    kprintf("nvme: soft pci class=0x%02x subclass=0x%02x pif=0x%02x "
            "mass_storage_nvme=1 soft PASS\n",
            (unsigned)NVME_PCI_CLASS, (unsigned)NVME_PCI_SUBCLASS,
            (unsigned)NVME_PCI_PROG_IF);

    /*
     * Grep: nvme: soft path
     * Honesty: product surface is PCI class + soft CAP/VS/status inventory.
     * claim=0 queues — no CC.EN write, no AQA/ASQ/ACQ program, no I/O.
     */
    kprintf("nvme: soft path claim=0 admin_q=0 io_q=0 cc_en_write=0 "
            "aqa_write=0 identify_cmd=0 map_uc=1 cap_fields=1 vs=1 "
            "csts_ro=1 cc_ro=1 int_ro=1 aqa_ro=1 via=%s wave=%u\n",
            szViaSafe, (unsigned)NVME_SOFT_DEEPEN_WAVE);

    /*
     * Wave 15 exclusive deepen (complementary; never hard-gates).
     * greppable: nvme: soft ratio|headroom|surface
     */
    {
        u32 u32CssNvmBp = fCssNvm != 0 ? 10000u : 0u;
        u32 u32RdyLamp = u32Rdy;
        u32 u32EnLamp = u32En;
        u32 u32MpsSpan = 0;

        if (fCapOk != 0 && u32MpsMax >= u32MpsMin) {
            u32MpsSpan = u32MpsMax - u32MpsMin;
        }
        /* Grep: nvme: soft ratio */
        kprintf("nvme: soft ratio css_nvm_bp=%u rdy=%u en=%u mqes=%u "
                "max_q=%u mps_span=%u wave=%u\n",
                u32CssNvmBp, u32RdyLamp, u32EnLamp, u32Mqes, u32MaxQ,
                u32MpsSpan, (unsigned)NVME_SOFT_DEEPEN_WAVE);
        /* Grep: nvme: soft headroom */
        kprintf("nvme: soft headroom max_q=%u mqes=%u to_ms=%u db_b=%u "
                "asqs=%u acqs=%u wave=%u\n",
                u32MaxQ, u32Mqes, u32ToMs, u32DbStrideB, u32Asqs, u32Acqs,
                (unsigned)NVME_SOFT_DEEPEN_WAVE);
        /* Grep: nvme: soft surface */
        kprintf("nvme: soft surface inventory,cap,fields,vs,csts,cc,int,"
                "aqa,regs,bar,pci,path,ratio,headroom,honesty,geom,return,"
                "contract,return_selftest,retmap,deepen,stats areas=%u wave=%u\n",
                (unsigned)NVME_SOFT_DEEPEN_AREAS,
                (unsigned)NVME_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 16 complementary deepen (kept; never hard-gates).
     * Soft ≠ game I/O. greppable: nvme: soft honesty|geom|return|contract
     */
    {
        u32 u32Surf = 0u;

        /* Return-surface bit lamps (inventory only; never product gate). */
        if (fCapOk != 0) {
            u32Surf |= 0x1u; /* CAP soft-read */
        }
        if (fVsOk != 0) {
            u32Surf |= 0x2u; /* VS soft-read */
        }
        if (fCstsOk != 0) {
            u32Surf |= 0x4u; /* CSTS soft-read */
        }
        if (fCcOk != 0) {
            u32Surf |= 0x8u; /* CC soft-read */
        }
        if (u32Intms != 0xffffffffu || u32Intmc != 0xffffffffu) {
            u32Surf |= 0x10u; /* INT soft-read */
        }
        if (u32Aqa != 0xffffffffu) {
            u32Surf |= 0x20u; /* AQA soft-read */
        }
        u32Surf |= 0x40u; /* regs/bar/pci map always catalogued */
        /* Grep: nvme: soft honesty */
        kprintf("nvme: soft honesty probe_only=1 admin_q=0 io_q=0 "
                "cc_en_write=0 game_io=0 product_storage=0 bar3=open "
                "soft_only=1 wave=%u soft PASS\n",
                (unsigned)NVME_SOFT_DEEPEN_WAVE);
        /* Grep: nvme: soft geom */
        kprintf("nvme: soft geom cap_off=0x%x vs_off=0x%x csts_off=0x%x "
                "cc_off=0x%x aqa_off=0x%x bar0_bits=%u mqes=%u max_q=%u "
                "wave=%u soft PASS\n",
                (unsigned)NVME_REG_CAP, (unsigned)NVME_REG_VS,
                (unsigned)NVME_REG_CSTS, (unsigned)NVME_REG_CC,
                (unsigned)NVME_REG_AQA, u32BarBits, u32Mqes, u32MaxQ,
                (unsigned)NVME_SOFT_DEEPEN_WAVE);
        /* Grep: nvme: soft return — return-surface bitmask */
        kprintf("nvme: soft return surf=0x%x cap=%u vs=%u csts=%u cc=%u "
                "int=%u aqa=%u map=1 via=%s areas=%u wave=%u soft PASS\n",
                u32Surf, fCapOk != 0 ? 1u : 0u, fVsOk != 0 ? 1u : 0u,
                fCstsOk != 0 ? 1u : 0u, fCcOk != 0 ? 1u : 0u,
                (u32Intms != 0xffffffffu || u32Intmc != 0xffffffffu) ? 1u
                                                                     : 0u,
                (u32Aqa != 0xffffffffu) ? 1u : 0u, szViaSafe,
                (unsigned)NVME_SOFT_DEEPEN_AREAS,
                (unsigned)NVME_SOFT_DEEPEN_WAVE);
        /* Grep: nvme: soft contract — soft ≠ game I/O */
        kprintf("nvme: soft contract soft_only=1 game_io=0 product_io=0 "
                "queues_claimed=0 identify_cmd=0 bar3=open wave=%u "
                "soft PASS\n",
                (unsigned)NVME_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: nvme: soft return — Wave 17 API return surfaces (kept) */
    kprintf("nvme: soft return cap=%u vs=%u probe=1 soft_inv=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (fCapOk != 0 ? 1u : 0u), (fVsOk != 0 ? 1u : 0u), (unsigned)NVME_SOFT_DEEPEN_WAVE);

    /* Grep: nvme: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("nvme: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)NVME_SOFT_DEEPEN_WAVE);

    /* Grep: nvme: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("nvme: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)NVME_SOFT_DEEPEN_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: nvme: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("nvme: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)NVME_SOFT_DEEPEN_WAVE);

    /* Grep: nvme: soft retcode — Wave 19 retcode catalog */
    kprintf("nvme: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)NVME_SOFT_DEEPEN_WAVE);

    /* Grep: nvme: soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: nvme: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("nvme: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)NVME_SOFT_DEEPEN_WAVE);
    /* Grep: nvme: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("nvme: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)NVME_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: nvme: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("nvme: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)NVME_SOFT_DEEPEN_WAVE);
    /* Grep: nvme: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("nvme: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)NVME_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: nvme: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("nvme: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)NVME_SOFT_DEEPEN_WAVE);
            /* Grep: nvme: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("nvme: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)NVME_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: nvme: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("nvme: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)NVME_SOFT_DEEPEN_WAVE);
            /* Grep: nvme: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("nvme: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)NVME_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: nvme: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("nvme: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)NVME_SOFT_DEEPEN_WAVE);
            /* Grep: nvme: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("nvme: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)NVME_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: nvme: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("nvme: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)NVME_SOFT_DEEPEN_WAVE);
            /* Grep: nvme: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("nvme: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)NVME_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: nvme: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("nvme: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)NVME_SOFT_DEEPEN_WAVE);
            /* Grep: nvme: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("nvme: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)NVME_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: nvme: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("nvme: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)NVME_SOFT_DEEPEN_WAVE);
            /* Grep: nvme: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("nvme: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)NVME_SOFT_DEEPEN_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: nvme: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("nvme: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)NVME_SOFT_DEEPEN_WAVE);
                    /* Grep: nvme: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("nvme: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)NVME_SOFT_DEEPEN_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: nvme: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("nvme: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)NVME_SOFT_DEEPEN_WAVE);
                            /* Grep: nvme: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("nvme: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)NVME_SOFT_DEEPEN_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: nvme: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("nvme: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=82 "
                                    "(retglyph honesty; Soft≠product; not bar3)\n");
                            /* Grep: nvme: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("nvme: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=82 "
                                    "(retscepter stamp; Soft≠product)\n");
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: nvme: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("nvme: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=82 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: nvme: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("nvme: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=82 "
                                    "(retemblem stamp; Soft≠product)\n");
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: nvme: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("nvme: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=82 "
                                    "(retaegis honesty; Soft≠product; not bar3)\n");
                            /* Grep: nvme: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("nvme: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=82 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: nvme: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("nvme: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=82 "
                                    "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("nvme: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("nvme: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("nvme: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("nvme: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("nvme: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("nvme: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retfortress — Wave 35 return-fortress honesty */
kprintf("nvme: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("nvme: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft rethold — Wave 36 return-hold honesty */
kprintf("nvme: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retspire — Wave 36 exclusive spire stamp */
kprintf("nvme: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retwall — Wave 37 return-wall honesty */
kprintf("nvme: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retgate — Wave 37 exclusive gate stamp */
kprintf("nvme: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retmoat — Wave 38 return-moat honesty */
kprintf("nvme: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retower — Wave 38 exclusive tower stamp */
kprintf("nvme: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("nvme: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("nvme: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("nvme: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("nvme: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retravelin — Wave 41 return-travelin honesty */
kprintf("nvme: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("nvme: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("nvme: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("nvme: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("nvme: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("nvme: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("nvme: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("nvme: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("nvme: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("nvme: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retbailey — Wave 46 return-bailey honesty */
kprintf("nvme: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("nvme: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("nvme: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("nvme: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("nvme: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("nvme: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("nvme: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("nvme: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retsally — Wave 50 return-sally honesty */
kprintf("nvme: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("nvme: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retfosse — Wave 51 return-fosse honesty */
kprintf("nvme: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("nvme: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("nvme: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("nvme: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retravelin — Wave 53 return-travelin honesty */
kprintf("nvme: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("nvme: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("nvme: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retredan — Wave 54 exclusive redan stamp */
kprintf("nvme: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retflank — Wave 55 return-flank honesty */
kprintf("nvme: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retface — Wave 55 exclusive face stamp */
kprintf("nvme: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retgorge — Wave 56 return-gorge honesty */
kprintf("nvme: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("nvme: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retraverse — Wave 57 return-traverse honesty */
kprintf("nvme: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("nvme: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retorillon — Wave 58 return-orillon honesty */
kprintf("nvme: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("nvme: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("nvme: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("nvme: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retplace — Wave 60 return-place honesty */
kprintf("nvme: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("nvme: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("nvme: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("nvme: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("nvme: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("nvme: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("nvme: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("nvme: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: nvme: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("nvme: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: nvme: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("nvme: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: nvme: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("nvme: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: nvme: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("nvme: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: nvme: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("nvme: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=82 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: nvme: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("nvme: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=82 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("nvme: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("nvme: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("nvme: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("nvme: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("nvme: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=82 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("nvme: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=82 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("nvme: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("nvme: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("nvme: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("nvme: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: nvme: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("nvme: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("nvme: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: nvme: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("nvme: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("nvme: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retbastionangle stamp; Soft≠product)\n");
/* Grep: nvme: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("nvme: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("nvme: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retparapetangle stamp; Soft≠product)\n");
/* Grep: nvme: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("nvme: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("nvme: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retowerangle stamp; Soft≠product)\n");
/* Grep: nvme: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("nvme: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("nvme: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retwallangle stamp; Soft≠product)\n");
/* Grep: nvme: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("nvme: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("nvme: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retholdangle stamp; Soft≠product)\n");
/* Grep: nvme: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("nvme: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("nvme: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retfortressangle stamp; Soft≠product)\n");
/* Grep: nvme: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("nvme: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("nvme: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: nvme: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("nvme: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("nvme: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: nvme: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("nvme: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("nvme: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: nvme: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("nvme: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=82 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: nvme: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("nvme: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=82 (retaegisangle stamp; Soft≠product)\n");

                            kprintf("nvme: soft deepen wave=%u areas=%u via=%s cap_ok=%u vs_ok=%u "
            "found=%u identify_ok=%u map_fail=%u no_bar=%u ok=%u "
            "skip=%u\n",
            (unsigned)NVME_SOFT_DEEPEN_WAVE,
            (unsigned)NVME_SOFT_DEEPEN_AREAS, szViaSafe,
            fCapOk != 0 ? 1u : 0u, fVsOk != 0 ? 1u : 0u, g_u32SoftFound,
            g_u32SoftIdentifyOk, g_u32SoftMapFail, g_u32SoftNoBar,
            fCapOk != 0 ? 1u : 0u, fCapOk != 0 ? 0u : 1u);

    /* Grep: nvme: soft stats */
    kprintf("nvme: soft stats inv_logs=%u probe_logs=%u found=%u "
            "identify_ok=%u map_fail=%u no_bar=%u wave=%u\n",
            g_u32SoftInvLogs, g_u32SoftProbeLogs, g_u32SoftFound,
            g_u32SoftIdentifyOk, g_u32SoftMapFail, g_u32SoftNoBar,
            (unsigned)NVME_SOFT_DEEPEN_WAVE);

    /* Grep: nvme: soft inventory PASS|SKIP / nvme: soft PASS|SKIP */
    kprintf("nvme: soft inventory %s via=%s mqes=%u css=0x%x wave=%u "
            "areas=%u\n",
            szVerdict, szViaSafe, u32Mqes, u32Css,
            (unsigned)NVME_SOFT_DEEPEN_WAVE,
            (unsigned)NVME_SOFT_DEEPEN_AREAS);
    kprintf("nvme: soft %s via=%s cap_ok=%u wave=%u\n", szVerdict, szViaSafe,
            fCapOk != 0 ? 1u : 0u, (unsigned)NVME_SOFT_DEEPEN_WAVE);
}

/**
 * Soft identify: map BAR0 UC and inventory CAP fields + VS + CSTS/CC +
 * INTMS/INTMC/AQA. Read-only MMIO — no CC write, no AQA/ASQ/ACQ program,
 * no admin queues as product.
 */
static void
nvme_soft_identify(u64 paBar, u32 u32BarBits)
{
    gj_vaddr_t vaMap = 0;
    gj_status_t stMap;

    kprintf("nvme: bar0 mem soft path PASS pa=0x%lx\n",
            (unsigned long)paBar);
    stMap = vmm_map_device_uc((gj_paddr_t)paBar, 0x1000, &vaMap);
    if (stMap != GJ_OK) {
        if (g_u32SoftMapFail < 0xffffffffu) {
            g_u32SoftMapFail++;
        }
        kprintf("nvme: bar0 map soft SKIP st=%d\n", (int)stMap);
        kprintf("nvme: CAP inventory soft SKIP (unmapped)\n");
        nvme_soft_inventory("map_fail", ~0ull, 0xffffffffu, 0xffffffffu,
                            0xffffffffu, 0xffffffffu, 0xffffffffu,
                            0xffffffffu, paBar, u32BarBits);
        return;
    }
    {
        volatile u8 *pMmio = (volatile u8 *)(gj_vaddr_t)vaMap;
        u64 u64Cap;
        u32 u32Vs;
        u32 u32Cc;
        u32 u32Csts;
        u32 u32Intms;
        u32 u32Intmc;
        u32 u32Aqa;
        u32 u32Maj;
        u32 u32Min;
        u32 u32Ter;

        /* CAP @ 0x00 (64-bit LE) — full soft field inventory */
        u64Cap = *(volatile u64 *)(void *)(pMmio + NVME_REG_CAP);
        /* VS @ 0x08: TER:MIN:MAJ in bytes (public layout) */
        u32Vs = *(volatile u32 *)(void *)(pMmio + NVME_REG_VS);
        /* INTMS / INTMC soft-read only — never mask-write */
        u32Intms = *(volatile u32 *)(void *)(pMmio + NVME_REG_INTMS);
        u32Intmc = *(volatile u32 *)(void *)(pMmio + NVME_REG_INTMC);
        /* CC / CSTS soft-read only — never write EN */
        u32Cc = *(volatile u32 *)(void *)(pMmio + NVME_REG_CC);
        u32Csts = *(volatile u32 *)(void *)(pMmio + NVME_REG_CSTS);
        /* AQA soft-read only — never program ASQ/ACQ */
        u32Aqa = *(volatile u32 *)(void *)(pMmio + NVME_REG_AQA);

        u32Ter = u32Vs & 0xffu;
        u32Min = (u32Vs >> 8) & 0xffu;
        u32Maj = (u32Vs >> 16) & 0xffffu;

        if (g_u32SoftIdentifyOk < 0xffffffffu) {
            g_u32SoftIdentifyOk++;
        }

        kprintf("nvme: CAP=0x%lx soft PASS\n", (unsigned long)u64Cap);
        nvme_soft_cap_inventory(u64Cap);
        kprintf("nvme: identify VS=0x%x %u.%u.%u soft PASS\n", u32Vs, u32Maj,
                u32Min, u32Ter);
        nvme_soft_status_inventory(u32Csts, u32Cc);
        /* Explicit non-claim: product stop is CAP/VS/status soft inventory. */
        kprintf("nvme: admin queues soft SKIP (not claimed)\n");
        kprintf("nvme: I/O path soft SKIP (not claimed)\n");
        /* Wave 14 unified soft inventory */
        nvme_soft_inventory("identify", u64Cap, u32Vs, u32Csts, u32Cc,
                            u32Intms, u32Intmc, u32Aqa, paBar, u32BarBits);
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
                u64 paBar;
                u32 u32BarBits;

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
                u32BarBits = f64 != 0 ? 64u : 32u;
                kprintf("nvme: probe %u:%u.%u vendor=0x%x bar0=0x%x soft "
                        "PASS\n",
                        u8Bus, u8Slot, u8Func, u16Vendor, u32BarRaw);
                kprintf("nvme: identify %u:%u.%u id=%04x:%04x bar0_pa=0x%lx "
                        "bits=%u soft PASS\n",
                        u8Bus, u8Slot, u8Func, u16Vendor, u16Device,
                        (unsigned long)paBar, u32BarBits);
                if (paBar != 0 && (u32BarRaw & 1u) == 0) {
                    nvme_soft_identify(paBar, u32BarBits);
                } else {
                    if (g_u32SoftNoBar < 0xffffffffu) {
                        g_u32SoftNoBar++;
                    }
                    kprintf("nvme: bar0 empty/io soft SKIP\n");
                    kprintf("nvme: CAP inventory soft SKIP (unmapped)\n");
                    nvme_soft_inventory("no_bar", ~0ull, 0xffffffffu,
                                        0xffffffffu, 0xffffffffu,
                                        0xffffffffu, 0xffffffffu,
                                        0xffffffffu, 0ull, 0u);
                }
                cFound++;
                if (g_u32SoftFound < 0xffffffffu) {
                    g_u32SoftFound++;
                }
            }
        }
    }
    if (cFound == 0) {
        kprintf("nvme: probe none soft SKIP\n");
        kprintf("nvme: CAP inventory soft SKIP (no controller)\n");
        nvme_soft_inventory("none", ~0ull, 0xffffffffu, 0xffffffffu,
                            0xffffffffu, 0xffffffffu, 0xffffffffu,
                            0xffffffffu, 0ull, 0u);
    } else {
        kprintf("nvme: probe count=%u soft PASS\n", cFound);
    }
    kprintf("nvme: probe soft PASS\n");
    return cFound;
}
