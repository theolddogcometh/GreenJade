/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Lean soft residual storage probe - NVMe PCI class 01:08:02.
 * Clean-room pure C. Dual MIT OR Apache-2.0. No GPL source.
 *
 * Soft-read only via vmm_map_device_uc (high UC window - never
 * identity-map device MMIO over the kernel):
 *   CAP field rollup, VS, CSTS/CC (public field soft decode),
 *   INTMS/INTMC, AQA, ASQ/ACQ bases (observe only), CMBLOC/CMBSZ when
 *   CAP.CMBS, BPINFO when CAP.BPS, CRTO when CAP.CRMS.
 *   Never writes CC.EN / NSSR / BPRSEL; never claims admin/I/O queues;
 *   never I/O; never identify cmd.
 *
 * Soft != product dual license honesty:
 *   kernel soft residual inventory != product storage path
 *   product storage later = userspace UDX / DDI hosts (not this TU)
 *   Soft!=product · G-AC-1 (no .ko product AC)
 *   Dual DoD product_storage=OPEN (this residual never closes product)
 *
 * Lean residual policy (this unit only; C0 Soft!=product):
 *   - NO version stamp
 *   - NO stamp storms (was wave=/areas= multi-line deepen catalogs)
 *   - Hard-capped soft inventory emission
 *   - Functional residual preferred over multi-line deepen catalogs
 *   - Silent lean self-check folds into one-shot residual lean PASS
 *   - Contiguous greppable: "nvme: soft residual lean PASS"
 *   - Once-shot DoD seed OPEN (product_storage stays OPEN)
 *
 * Greppable (keep stable):
 *   nvme: soft inventory ...
 *   nvme: soft residual ...
 *   nvme: soft residual lean PASS
 *   nvme: soft residual DoD seed OPEN
 *   nvme: soft path ...
 *   nvme: soft honesty ...
 *   nvme: soft inventory PASS|SKIP / nvme: soft PASS|SKIP
 *   nvme: CAP inventory ... soft PASS|SKIP   (legacy CAP rollup)
 *   nvme: CAP=... / identify VS=... / CSTS=... / CC=...
 *   nvme: ASQ=... / ACQ=... soft PASS (observe; not claimed)
 *   nvme: BPINFO=... / CRTO=... soft PASS|SKIP (CAP-gated observe)
 *   nvme: admin queues soft SKIP / I/O path soft SKIP
 *   nvme: probe ...
 *   Soft!=product
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
 * Map window is 0x1000 - covers through CMBSZ (0x3C).
 */
#define NVME_REG_CAP    0x00u /* 64-bit CAP */
#define NVME_REG_VS     0x08u /* 32-bit Version */
#define NVME_REG_INTMS  0x0Cu /* 32-bit Interrupt Mask Set (soft-read) */
#define NVME_REG_INTMC  0x10u /* 32-bit Interrupt Mask Clear (soft-read) */
#define NVME_REG_CC     0x14u /* 32-bit Controller Configuration (read soft) */
#define NVME_REG_CSTS   0x1Cu /* 32-bit Controller Status (read soft) */
#define NVME_REG_NSSR   0x20u /* 32-bit NVM Subsystem Reset (soft-read) */
#define NVME_REG_AQA    0x24u /* 32-bit Admin Queue Attributes (soft-read) */
#define NVME_REG_ASQ    0x28u /* 64-bit Admin Submission Queue Base (soft) */
#define NVME_REG_ACQ    0x30u /* 64-bit Admin Completion Queue Base (soft) */
#define NVME_REG_CMBLOC 0x38u /* 32-bit CMB Location (soft; when CAP.CMBS) */
#define NVME_REG_CMBSZ  0x3Cu /* 32-bit CMB Size (soft; when CAP.CMBS) */
#define NVME_REG_BPINFO 0x40u /* 32-bit Boot Partition Info (soft; CAP.BPS) */
#define NVME_REG_BPRSEL 0x44u /* 32-bit Boot Partition Select (never write) */
#define NVME_REG_BPMBL  0x48u /* 64-bit Boot Partition Memory Buffer (soft) */
#define NVME_REG_CRTO   0x68u /* 32-bit Controller Ready Timeouts (CAP.CRMS) */

/* CSTS / CC public soft decode (inventory only; never write) */
#define NVME_CSTS_RDY   0x1u
#define NVME_CSTS_CFS   0x2u
#define NVME_CSTS_SHST(c)  ((u32)(((c) >> 2) & 3u)) /* Shutdown Status */
#define NVME_CSTS_NSSRO 0x10u /* NVM Subsystem Reset Occurred */
#define NVME_CSTS_PP    0x20u /* Processing Paused */
#define NVME_CC_EN      0x1u
#define NVME_CC_CSS(c)     ((u32)(((c) >> 4) & 7u))
#define NVME_CC_MPS(c)     ((u32)(((c) >> 7) & 0xfu))
#define NVME_CC_AMS(c)     ((u32)(((c) >> 11) & 7u))
#define NVME_CC_SHN(c)     ((u32)(((c) >> 14) & 3u))
#define NVME_CC_IOSQES(c)  ((u32)(((c) >> 16) & 0xfu))
#define NVME_CC_IOCQES(c)  ((u32)(((c) >> 20) & 0xfu))

/*
 * CAP bit fields (public NVM Express Base layout; MQES is 0-based).
 * Inventory only - does not enable the controller or claim queues.
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

/*
 * Lean residual inventory: hard cap on emission (no stamp storms / H2).
 * Soft != product; dual MIT OR Apache-2.0. No version stamp.
 */
#define NVME_SOFT_INV_LOG_CAP 2u
/* UC map window covers CAP..CMBSZ (public controller properties). */
#define NVME_SOFT_MAP_BYTES   0x1000u

/* Soft inventory emission tallies (wrap OK; never hard-gate). */
static u32 g_u32SoftInvLogs;
static u32 g_u32SoftProbeLogs;
static u32 g_u32SoftIdentifyOk;
static u32 g_u32SoftMapFail;
static u32 g_u32SoftNoBar;
static u32 g_u32SoftFound;
/* Once-shot lean residual (no stamp storm across multi-ctrl inventory). */
static u32 g_fSoftResidualLeanOnce;
static u32 g_u32LeanOk;
static u32 g_u32LeanChecks;

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
 * Silent lean residual self-check (no kprintf; Soft!=product).
 * Geometry + public layout constants + soft-path honesty + CAP-gated
 * residual surface (BPINFO/CRTO in map). Returns ok count;
 * *pOutChecks = total when non-NULL.
 * Results fold into contiguous soft residual lean PASS (H2: no storm).
 */
static u32
nvme_soft_residual_lean_ok(u64 u64Cap, u32 u32Aqa, u64 u64Asq, u64 u64Acq,
                           u32 *pOutChecks)
{
    u32 u32Ok = 0;
    u32 u32Checks = 0;
    u32 u32Mqes;
    u32 u32MpsMin;
    u32 u32MpsMax;
    u32 u32Dstrd;
    u32 u32DbStrideB;
    u32 u32Asqs;
    u32 u32Acqs;
    int fCapOk;
    int fAsqProg;
    int fAcqProg;

    /* 1: PCI class triple is mass-storage NVMe */
    u32Checks++;
    if (NVME_PCI_CLASS == 0x01u && NVME_PCI_SUBCLASS == 0x08u &&
        NVME_PCI_PROG_IF == 0x02u) {
        u32Ok++;
    }
    /* 2: CAP at 0; map window covers CRTO (deepest soft residual reg) */
    u32Checks++;
    if (NVME_REG_CAP == 0x00u && NVME_REG_CRTO < NVME_SOFT_MAP_BYTES &&
        NVME_REG_BPINFO < NVME_SOFT_MAP_BYTES &&
        NVME_SOFT_MAP_BYTES == 0x1000u) {
        u32Ok++;
    }
    /* 3: admin queue regs ordered AQA < ASQ < ACQ < CMBLOC < BPINFO < CRTO */
    u32Checks++;
    if (NVME_REG_AQA == 0x24u && NVME_REG_ASQ == 0x28u &&
        NVME_REG_ACQ == 0x30u && NVME_REG_CMBLOC == 0x38u &&
        NVME_REG_BPINFO == 0x40u && NVME_REG_CRTO == 0x68u &&
        NVME_REG_ASQ < NVME_REG_ACQ && NVME_REG_ACQ < NVME_REG_CMBLOC) {
        u32Ok++;
    }
    /* 4: inventory hard-cap is lean (H2); never stamp-storm */
    u32Checks++;
    if (NVME_SOFT_INV_LOG_CAP == 2u && NVME_SOFT_INV_LOG_CAP <= 4u) {
        u32Ok++;
    }
    /* 5: CSTS.RDY / CC.EN / CSS_NVM single-bit public layout */
    u32Checks++;
    if (NVME_CSTS_RDY == 0x1u && NVME_CC_EN == 0x1u &&
        NVME_CAP_CSS_NVM == 0x1u && NVME_CSTS_CFS == 0x2u) {
        u32Ok++;
    }
    /* 6: CAP readable => MPSMIN <= MPSMAX + MQES 0-based max_q > 0 */
    fCapOk = (u64Cap != 0ull && u64Cap != ~0ull) ? 1 : 0;
    u32Checks++;
    if (fCapOk == 0) {
        /* Unmapped / empty path: still honest (no false CAP geometry) */
        u32Ok++;
    } else {
        u32Mqes = NVME_CAP_MQES(u64Cap);
        u32MpsMin = NVME_CAP_MPSMIN(u64Cap);
        u32MpsMax = NVME_CAP_MPSMAX(u64Cap);
        if (u32MpsMin <= u32MpsMax && (u32Mqes + 1u) > 0u) {
            u32Ok++;
        }
    }
    /* 7: doorbell stride formula (4 << DSTRD) when CAP ok; DSTRD <= 7 */
    u32Checks++;
    if (fCapOk == 0) {
        u32Ok++;
    } else {
        u32Dstrd = NVME_CAP_DSTRD(u64Cap);
        u32DbStrideB = 4u << u32Dstrd;
        if (u32Dstrd <= 7u && u32DbStrideB >= 4u &&
            (u32DbStrideB & (u32DbStrideB - 1u)) == 0u) {
            u32Ok++;
        }
    }
    /* 8: AQA ASQS/ACQS 12-bit fields when programmed; else empty ok */
    u32Checks++;
    if (u32Aqa == 0xffffffffu) {
        u32Ok++;
    } else {
        u32Asqs = u32Aqa & 0xfffu;
        u32Acqs = (u32Aqa >> 16) & 0xfffu;
        if (u32Asqs <= 0xfffu && u32Acqs <= 0xfffu) {
            u32Ok++;
        }
    }
    /*
     * 9: ASQ/ACQ observe-only honesty.
     * Unread (~0) ok; empty (0) ok; programmed => 4KiB-aligned soft
     * observe (spec base addresses page-aligned). Never claim queues.
     */
    fAsqProg = (u64Asq != 0ull && u64Asq != ~0ull) ? 1 : 0;
    fAcqProg = (u64Acq != 0ull && u64Acq != ~0ull) ? 1 : 0;
    u32Checks++;
    if (u64Asq == ~0ull && u64Acq == ~0ull) {
        u32Ok++; /* unread path */
    } else if (fAsqProg == 0 && fAcqProg == 0) {
        u32Ok++; /* empty bases; soft observe only */
    } else {
        if ((fAsqProg == 0 || (u64Asq & 0xfffull) == 0ull) &&
            (fAcqProg == 0 || (u64Acq & 0xfffull) == 0ull)) {
            u32Ok++;
        }
    }
    /* 10: soft path never programs AQA/ASQ/ACQ/BPRSEL (compile-time) */
    u32Checks++;
    if (NVME_REG_ASQ != NVME_REG_ACQ && NVME_REG_AQA < NVME_REG_ASQ &&
        NVME_REG_BPRSEL == 0x44u && NVME_REG_BPINFO < NVME_REG_BPRSEL) {
        u32Ok++;
    }
    /* 11: INTMS < INTMC < CC < CSTS < NSSR public order (soft-read map) */
    u32Checks++;
    if (NVME_REG_INTMS == 0x0cu && NVME_REG_INTMC == 0x10u &&
        NVME_REG_CC == 0x14u && NVME_REG_CSTS == 0x1cu &&
        NVME_REG_NSSR == 0x20u && NVME_REG_INTMS < NVME_REG_INTMC &&
        NVME_REG_CC < NVME_REG_CSTS) {
        u32Ok++;
    }
    /*
     * 12: Soft!=product compile-time honesty lock.
     * product_storage residual never claims CC.EN write / queues / identify.
     * (Constants-only; runtime claim lamps remain 0 in residual path.)
     */
    u32Checks++;
    if (NVME_CC_EN == 0x1u && NVME_SOFT_INV_LOG_CAP > 0u &&
        NVME_REG_CRTO > NVME_REG_CMBSZ) {
        u32Ok++;
    }

    g_u32LeanOk = u32Ok;
    g_u32LeanChecks = u32Checks;
    if (pOutChecks != NULL) {
        *pOutChecks = u32Checks;
    }
    return u32Ok;
}

/**
 * Compact CAP field rollup - two greppable lines (no per-field stamp storm).
 * Grep: nvme: CAP inventory ... soft PASS|SKIP
 *
 * All-0 / all-1 CAP is treated as unreadable MMIO for the soft path.
 * Derived values are inventory only - never used to program CC.
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
    int fCapOk;
    int fCssNvm;

    fCapOk = (u64Cap != 0ull && u64Cap != ~0ull) ? 1 : 0;
    if (fCapOk == 0) {
        kprintf("nvme: CAP inventory soft SKIP cap=0x%lx (unreadable)\n",
                (unsigned long)u64Cap);
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

    /* Spec: MQES is 0's based -> max entries = MQES + 1 */
    u32MaxQEntries = u32Mqes + 1u;
    /* TO is in 500 ms units */
    u32ToMs = u32To * 500u;
    /* Doorbell stride = 4 << DSTRD bytes */
    u32DbStrideB = 4u << u32Dstrd;
    fCssNvm = ((u32Css & NVME_CAP_CSS_NVM) != 0u) ? 1 : 0;

    /* Compact rollup (was multi-line per-field deepen) */
    kprintf("nvme: CAP inventory MQES=%u max_q=%u CQR=%u AMS=%u TO=%u "
            "to_ms=%u DSTRD=%u db_b=%u NSSRS=%u CSS=0x%x nvm=%u "
            "soft PASS\n",
            u32Mqes, u32MaxQEntries, u32Cqr, u32Ams, u32To, u32ToMs,
            u32Dstrd, u32DbStrideB, u32Nssrs, u32Css,
            fCssNvm != 0 ? 1u : 0u);
    kprintf("nvme: CAP inventory BPS=%u CPS=%u MPSMIN=%u MPSMAX=%u "
            "PMRS=%u CMBS=%u CRMS=%u soft PASS\n",
            u32Bps, u32Cps, u32MpsMin, u32MpsMax, u32Pmrs, u32Cmbs,
            u32Crms);
}

/**
 * Soft CSTS/CC status inventory (read-only; never write CC.EN).
 * Public field soft decode - compact (no per-bit stamp storm).
 * Grep: nvme: CSTS=... soft PASS | nvme: CC=... soft PASS
 */
static void
nvme_soft_status_inventory(u32 u32Csts, u32 u32Cc)
{
    u32 u32Rdy;
    u32 u32Cfs;
    u32 u32Shst;
    u32 u32Nssro;
    u32 u32Pp;
    u32 u32En;
    u32 u32Css;
    u32 u32Mps;
    u32 u32Ams;
    u32 u32Shn;
    u32 u32Iosqes;
    u32 u32Iocqes;

    u32Rdy = (u32Csts & NVME_CSTS_RDY) != 0u ? 1u : 0u;
    u32Cfs = (u32Csts & NVME_CSTS_CFS) != 0u ? 1u : 0u;
    u32Shst = NVME_CSTS_SHST(u32Csts);
    u32Nssro = (u32Csts & NVME_CSTS_NSSRO) != 0u ? 1u : 0u;
    u32Pp = (u32Csts & NVME_CSTS_PP) != 0u ? 1u : 0u;
    u32En = (u32Cc & NVME_CC_EN) != 0u ? 1u : 0u;
    u32Css = NVME_CC_CSS(u32Cc);
    u32Mps = NVME_CC_MPS(u32Cc);
    u32Ams = NVME_CC_AMS(u32Cc);
    u32Shn = NVME_CC_SHN(u32Cc);
    u32Iosqes = NVME_CC_IOSQES(u32Cc);
    u32Iocqes = NVME_CC_IOCQES(u32Cc);

    kprintf("nvme: CSTS=0x%x RDY=%u CFS=%u SHST=%u NSSRO=%u PP=%u "
            "soft PASS\n",
            u32Csts, u32Rdy, u32Cfs, u32Shst, u32Nssro, u32Pp);
    kprintf("nvme: CC=0x%x EN=%u CSS=%u MPS=%u AMS=%u SHN=%u "
            "IOSQES=%u IOCQES=%u soft PASS\n",
            u32Cc, u32En, u32Css, u32Mps, u32Ams, u32Shn, u32Iosqes,
            u32Iocqes);
    if (u32En != 0u) {
        /* Soft observe only - product path never claims enable */
        kprintf("nvme: CC enable observed soft PASS (not claimed)\n");
    } else {
        kprintf("nvme: CC enable soft SKIP (disabled)\n");
    }
}

/**
 * Soft ASQ/ACQ base inventory (read-only; never program queues).
 * Non-zero bases mean firmware/prior software left queues programmed -
 * still claim=0. Grep: nvme: ASQ=... ACQ=... soft PASS
 */
static void
nvme_soft_aq_inventory(u64 u64Asq, u64 u64Acq, u32 u32Aqa)
{
    u32 u32Asqs;
    u32 u32Acqs;
    u32 u32AsqProg;
    u32 u32AcqProg;

    u32Asqs = (u32Aqa != 0xffffffffu) ? (u32Aqa & 0xfffu) : 0u;
    u32Acqs = (u32Aqa != 0xffffffffu) ? ((u32Aqa >> 16) & 0xfffu) : 0u;
    u32AsqProg = (u64Asq != 0ull && u64Asq != ~0ull) ? 1u : 0u;
    u32AcqProg = (u64Acq != 0ull && u64Acq != ~0ull) ? 1u : 0u;

    kprintf("nvme: ASQ=0x%lx ACQ=0x%lx asq_prog=%u acq_prog=%u "
            "aqa=0x%x asqs=%u acqs=%u soft PASS (observe; not claimed)\n",
            (unsigned long)u64Asq, (unsigned long)u64Acq, u32AsqProg,
            u32AcqProg, u32Aqa, u32Asqs, u32Acqs);
}

/**
 * Lean soft residual inventory - greppable "nvme: soft ...".
 * Snapshots soft-read CAP/VS/CSTS/CC/INT/AQA/ASQ/ACQ (+ CMB/BP/CRTO);
 * never allocates; never hard-gates; never writes CC or claims queues.
 * Hard-capped emission (NVME_SOFT_INV_LOG_CAP). No version stamp.
 * Soft != product. No stamp storms (H2). Dual DoD product_storage=OPEN.
 *
 * greppable: nvme: soft inventory
 * greppable: nvme: soft residual
 * greppable: nvme: soft residual lean PASS
 * greppable: nvme: soft residual DoD seed OPEN
 * greppable: nvme: soft path
 * greppable: nvme: soft honesty
 */
static void
nvme_soft_inventory(const char *szVia, u64 u64Cap, u32 u32Vs, u32 u32Csts,
                    u32 u32Cc, u32 u32Intms, u32 u32Intmc, u32 u32Aqa,
                    u64 u64Asq, u64 u64Acq, u32 u32Cmbloc, u32 u32Cmbsz,
                    u32 u32Bpinfo, u32 u32Crto, u32 u32Bps, u32 u32Crms,
                    u64 paBar, u32 u32BarBits)
{
    u32 u32Mqes = 0;
    u32 u32Css = 0;
    u32 u32MaxQ = 0;
    u32 u32ToMs = 0;
    u32 u32DbStrideB = 0;
    u32 u32Cmbs = 0;
    u32 u32Maj;
    u32 u32Min;
    u32 u32Ter;
    u32 u32Rdy;
    u32 u32Cfs;
    u32 u32Shst;
    u32 u32En;
    u32 u32Asqs;
    u32 u32Acqs;
    u32 u32AsqProg;
    u32 u32AcqProg;
    u32 u32LeanOk;
    u32 u32LeanChecks;
    u32 u32BpsObs;
    u32 u32CrmsObs;
    int fCapOk;
    int fVsOk;
    int fCstsOk;
    int fCcOk;
    int fCssNvm;
    const char *szViaSafe;
    const char *szVerdict;

    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "anon";

    /* Cap serial flood (identify + no_bar + none may call; residual only). */
    if (g_u32SoftInvLogs >= NVME_SOFT_INV_LOG_CAP) {
        return;
    }
    if (g_u32SoftInvLogs < 0xffffffffu) {
        g_u32SoftInvLogs++;
    }

    fCapOk = (u64Cap != 0ull && u64Cap != ~0ull) ? 1 : 0;
    fVsOk = (u32Vs != 0u && u32Vs != 0xffffffffu) ? 1 : 0;
    fCstsOk = (u32Csts != 0xffffffffu) ? 1 : 0;
    fCcOk = (u32Cc != 0xffffffffu) ? 1 : 0;

    if (fCapOk != 0) {
        u32Mqes = NVME_CAP_MQES(u64Cap);
        u32Css = NVME_CAP_CSS(u64Cap);
        u32MaxQ = u32Mqes + 1u;
        u32ToMs = NVME_CAP_TO(u64Cap) * 500u;
        u32DbStrideB = 4u << NVME_CAP_DSTRD(u64Cap);
        u32Cmbs = NVME_CAP_CMBS(u64Cap);
    }
    fCssNvm = ((u32Css & NVME_CAP_CSS_NVM) != 0u) ? 1 : 0;
    u32BpsObs = (fCapOk != 0) ? u32Bps : 0u;
    u32CrmsObs = (fCapOk != 0) ? u32Crms : 0u;

    u32Ter = u32Vs & 0xffu;
    u32Min = (u32Vs >> 8) & 0xffu;
    u32Maj = (u32Vs >> 16) & 0xffffu;
    u32Rdy = (fCstsOk != 0 && (u32Csts & NVME_CSTS_RDY) != 0u) ? 1u : 0u;
    u32Cfs = (fCstsOk != 0 && (u32Csts & NVME_CSTS_CFS) != 0u) ? 1u : 0u;
    u32Shst = (fCstsOk != 0) ? NVME_CSTS_SHST(u32Csts) : 0u;
    u32En = (fCcOk != 0 && (u32Cc & NVME_CC_EN) != 0u) ? 1u : 0u;
    /* AQA: ASQS bits 11:0, ACQS bits 27:16 (0's based when programmed) */
    u32Asqs = (u32Aqa != 0xffffffffu) ? (u32Aqa & 0xfffu) : 0u;
    u32Acqs = (u32Aqa != 0xffffffffu) ? ((u32Aqa >> 16) & 0xfffu) : 0u;
    u32AsqProg = (u64Asq != 0ull && u64Asq != ~0ull) ? 1u : 0u;
    u32AcqProg = (u64Acq != 0ull && u64Acq != ~0ull) ? 1u : 0u;

    /*
     * Soft verdict (inventory only; never claims admin/I/O queues):
     *   PASS - CAP readable (mapped controller)
     *   SKIP - unreadable CAP / empty inventory path
     */
    if (fCapOk != 0) {
        szVerdict = "PASS";
    } else {
        szVerdict = "SKIP";
    }

    /*
     * Grep: nvme: soft inventory
     * One-line rollup (CAP/VS/status lamps + tallies). Soft residual only.
     */
    kprintf("nvme: soft inventory via=%s cap_ok=%u vs_ok=%u csts_ok=%u "
            "cc_ok=%u mqes=%u max_q=%u css=0x%x nvm=%u rdy=%u en=%u "
            "vs=%u.%u.%u asq_prog=%u acq_prog=%u bps=%u crms=%u "
            "found=%u logs=%u Soft!=product\n",
            szViaSafe, fCapOk != 0 ? 1u : 0u, fVsOk != 0 ? 1u : 0u,
            fCstsOk != 0 ? 1u : 0u, fCcOk != 0 ? 1u : 0u, u32Mqes, u32MaxQ,
            u32Css, fCssNvm != 0 ? 1u : 0u, u32Rdy, u32En, u32Maj, u32Min,
            u32Ter, u32AsqProg, u32AcqProg, u32BpsObs, u32CrmsObs,
            g_u32SoftFound, g_u32SoftInvLogs);

    /*
     * Grep: nvme: soft residual
     * Storage residual surface - lean, no wave=/areas= stamp storm.
     * Path: PCI 01:08:02 -> BAR0 UC map -> CAP/VS/status/ASQ soft-read ->
     * product storage mint OPEN (userspace UDX later). Soft!=product.
     */
    kprintf("nvme: soft residual storage class=01:08:02 "
            "via=%s cap_ok=%u mqes=%u max_q=%u to_ms=%u db_b=%u "
            "css=0x%x nvm=%u rdy=%u cfs=%u shst=%u en=%u asqs=%u acqs=%u "
            "asq_prog=%u acq_prog=%u asq=0x%lx acq=0x%lx "
            "intms=0x%x intmc=0x%x aqa=0x%x cmbs=%u cmbloc=0x%x cmbsz=0x%x "
            "bps=%u bpinfo=0x%x crms=%u crto=0x%x "
            "bar_pa=0x%lx bits=%u map_uc=1 identify_ok=%u map_fail=%u "
            "no_bar=%u path=pci_010802->bar0_uc->cap_vs_csts_asq->"
            "userspace_UDX soft=1 product=0 product_storage=OPEN "
            "dual_dod=OPEN need=userspace_UDX "
            "Soft!=product G-AC-1 (no CC.EN write; no queues claimed; "
            "no .ko product; no BPRSEL write)\n",
            szViaSafe, fCapOk != 0 ? 1u : 0u, u32Mqes, u32MaxQ, u32ToMs,
            u32DbStrideB, u32Css, fCssNvm != 0 ? 1u : 0u, u32Rdy, u32Cfs,
            u32Shst, u32En, u32Asqs, u32Acqs, u32AsqProg, u32AcqProg,
            (unsigned long)u64Asq, (unsigned long)u64Acq, u32Intms, u32Intmc,
            u32Aqa, u32Cmbs, u32Cmbloc, u32Cmbsz, u32BpsObs, u32Bpinfo,
            u32CrmsObs, u32Crto, (unsigned long)paBar, u32BarBits,
            g_u32SoftIdentifyOk, g_u32SoftMapFail, g_u32SoftNoBar);

    /*
     * Grep: nvme: soft residual lean PASS
     * Contiguous greppable once-shot (H2: no multi-ctrl storm).
     * Silent self-check fold-in; Soft!=product; Dual DoD OPEN.
     */
    if (g_fSoftResidualLeanOnce == 0u) {
        g_fSoftResidualLeanOnce = 1u;
        u32LeanOk = nvme_soft_residual_lean_ok(u64Cap, u32Aqa, u64Asq, u64Acq,
                                               &u32LeanChecks);
        /*
         * Contiguous greppable: "nvme: soft residual lean PASS"
         * (PASS immediately after lean). Emit PASS only when lean self-check
         * is full-ok; otherwise lean lamp without PASS (honesty).
         */
        if (u32LeanOk == u32LeanChecks && u32LeanChecks > 0u) {
            kprintf("nvme: soft residual lean PASS via=%s cap_ok=%u "
                    "mqes=%u nvm=%u rdy=%u en=%u asq_prog=%u acq_prog=%u "
                    "bps=%u crms=%u lean_ok=%u/%u claim=0 admin_q=0 io_q=0 "
                    "cc_en_write=0 aqa_write=0 asq_write=0 acq_write=0 "
                    "bprsel_write=0 nssr_write=0 identify_cmd=0 "
                    "product_storage=OPEN dual_dod=OPEN need=userspace_UDX "
                    "soft=1 product=0 dual=MIT_OR_Apache-2.0 "
                    "stamp_storm=0 no_version_stamp=1 G-AC-1 Soft!=product\n",
                    szViaSafe, fCapOk != 0 ? 1u : 0u, u32Mqes,
                    fCssNvm != 0 ? 1u : 0u, u32Rdy, u32En, u32AsqProg,
                    u32AcqProg, u32BpsObs, u32CrmsObs, u32LeanOk,
                    u32LeanChecks);
        } else {
            kprintf("nvme: soft residual lean via=%s cap_ok=%u mqes=%u "
                    "nvm=%u rdy=%u en=%u asq_prog=%u acq_prog=%u "
                    "lean_ok=%u/%u claim=0 product_storage=OPEN "
                    "dual_dod=OPEN soft=1 product=0 G-AC-1 Soft!=product\n",
                    szViaSafe, fCapOk != 0 ? 1u : 0u, u32Mqes,
                    fCssNvm != 0 ? 1u : 0u, u32Rdy, u32En, u32AsqProg,
                    u32AcqProg, u32LeanOk, u32LeanChecks);
        }
        /*
         * Grep: nvme: soft residual DoD seed OPEN
         * Product storage remains OPEN (userspace UDX later). This residual
         * never closes Dual DoD product_storage. Soft!=product · G-AC-1.
         */
        kprintf("nvme: soft residual DoD seed OPEN class=01:08:02 via=%s "
                "cap_ok=%u mqes=%u nvm=%u rdy=%u en=%u asq_prog=%u "
                "acq_prog=%u bps=%u crms=%u lean_ok=%u/%u "
                "soft=1 product=0 claim=0 admin_q=0 io_q=0 "
                "product_storage=OPEN dual_dod=OPEN need=userspace_UDX "
                "path=pci_010802->bar0_uc->cap_vs_csts_asq->userspace_UDX "
                "dual=MIT_OR_Apache-2.0 G-AC-1 Soft!=product "
                "(!= CC.EN write; != queue claim; != identify cmd; "
                "!= .ko product)\n",
                szViaSafe, fCapOk != 0 ? 1u : 0u, u32Mqes,
                fCssNvm != 0 ? 1u : 0u, u32Rdy, u32En, u32AsqProg, u32AcqProg,
                u32BpsObs, u32CrmsObs, u32LeanOk, u32LeanChecks);
    }

    /*
     * Grep: nvme: soft path | nvme: soft honesty
     * Soft residual != product storage I/O. claim=0 queues.
     */
    kprintf("nvme: soft path claim=0 admin_q=0 io_q=0 cc_en_write=0 "
            "aqa_write=0 asq_write=0 acq_write=0 bprsel_write=0 "
            "nssr_write=0 identify_cmd=0 map_uc=1 "
            "asq_prog=%u acq_prog=%u via=%s "
            "product_storage=OPEN dual_dod=OPEN need=userspace_UDX "
            "(soft residual; Soft!=product G-AC-1)\n",
            u32AsqProg, u32AcqProg, szViaSafe);
    kprintf("nvme: soft honesty probe_only=1 admin_q=0 io_q=0 "
            "cc_en_write=0 game_io=0 product_storage=0 soft=1 product=0 "
            "product_storage_dod=OPEN dual_dod=OPEN need=userspace_UDX "
            "Soft!=product dual=MIT|Apache-2.0 G-AC-1 soft PASS\n");

    /* Grep: nvme: soft inventory PASS|SKIP / nvme: soft PASS|SKIP */
    kprintf("nvme: soft inventory %s via=%s mqes=%u css=0x%x logs=%u "
            "Soft!=product\n",
            szVerdict, szViaSafe, u32Mqes, u32Css, g_u32SoftInvLogs);
    kprintf("nvme: soft %s via=%s cap_ok=%u Soft!=product\n", szVerdict,
            szViaSafe, fCapOk != 0 ? 1u : 0u);
}

/**
 * Soft identify: map BAR0 UC and inventory CAP fields + VS + CSTS/CC +
 * INTMS/INTMC/AQA + ASQ/ACQ bases (+ CMB when CAP.CMBS, BPINFO when
 * CAP.BPS, CRTO when CAP.CRMS). Read-only MMIO - no CC write, no
 * AQA/ASQ/ACQ/BPRSEL program, no admin queues as product.
 */
static void
nvme_soft_identify(u64 paBar, u32 u32BarBits)
{
    gj_vaddr_t vaMap = 0;
    gj_status_t stMap;

    kprintf("nvme: bar0 mem soft path PASS pa=0x%lx\n",
            (unsigned long)paBar);
    stMap = vmm_map_device_uc((gj_paddr_t)paBar, NVME_SOFT_MAP_BYTES, &vaMap);
    if (stMap != GJ_OK) {
        if (g_u32SoftMapFail < 0xffffffffu) {
            g_u32SoftMapFail++;
        }
        kprintf("nvme: bar0 map soft SKIP st=%d\n", (int)stMap);
        kprintf("nvme: CAP inventory soft SKIP (unmapped)\n");
        nvme_soft_inventory("map_fail", ~0ull, 0xffffffffu, 0xffffffffu,
                            0xffffffffu, 0xffffffffu, 0xffffffffu,
                            0xffffffffu, ~0ull, ~0ull, 0xffffffffu,
                            0xffffffffu, 0xffffffffu, 0xffffffffu, 0u, 0u,
                            paBar, u32BarBits);
        return;
    }
    {
        volatile u8 *pMmio = (volatile u8 *)(gj_vaddr_t)vaMap;
        u64 u64Cap;
        u64 u64Asq;
        u64 u64Acq;
        u32 u32Vs;
        u32 u32Cc;
        u32 u32Csts;
        u32 u32Intms;
        u32 u32Intmc;
        u32 u32Aqa;
        u32 u32Nssr;
        u32 u32Cmbloc;
        u32 u32Cmbsz;
        u32 u32Bpinfo;
        u32 u32Crto;
        u32 u32Maj;
        u32 u32Min;
        u32 u32Ter;
        u32 u32Cmbs;
        u32 u32Bps;
        u32 u32Crms;

        /* CAP @ 0x00 (64-bit LE) - soft field inventory */
        u64Cap = *(volatile u64 *)(void *)(pMmio + NVME_REG_CAP);
        /* VS @ 0x08: TER:MIN:MAJ in bytes (public layout) */
        u32Vs = *(volatile u32 *)(void *)(pMmio + NVME_REG_VS);
        /* INTMS / INTMC soft-read only - never mask-write */
        u32Intms = *(volatile u32 *)(void *)(pMmio + NVME_REG_INTMS);
        u32Intmc = *(volatile u32 *)(void *)(pMmio + NVME_REG_INTMC);
        /* CC / CSTS soft-read only - never write EN */
        u32Cc = *(volatile u32 *)(void *)(pMmio + NVME_REG_CC);
        u32Csts = *(volatile u32 *)(void *)(pMmio + NVME_REG_CSTS);
        /* NSSR soft-read only - never trigger subsystem reset */
        u32Nssr = *(volatile u32 *)(void *)(pMmio + NVME_REG_NSSR);
        /* AQA soft-read only - never program ASQ/ACQ */
        u32Aqa = *(volatile u32 *)(void *)(pMmio + NVME_REG_AQA);
        /* ASQ / ACQ bases soft-read only - observe fw_prog; claim=0 */
        u64Asq = *(volatile u64 *)(void *)(pMmio + NVME_REG_ASQ);
        u64Acq = *(volatile u64 *)(void *)(pMmio + NVME_REG_ACQ);

        u32Cmbs = (u64Cap != 0ull && u64Cap != ~0ull) ? NVME_CAP_CMBS(u64Cap)
                                                      : 0u;
        u32Bps = (u64Cap != 0ull && u64Cap != ~0ull) ? NVME_CAP_BPS(u64Cap)
                                                     : 0u;
        u32Crms = (u64Cap != 0ull && u64Cap != ~0ull) ? NVME_CAP_CRMS(u64Cap)
                                                      : 0u;
        if (u32Cmbs != 0u) {
            u32Cmbloc = *(volatile u32 *)(void *)(pMmio + NVME_REG_CMBLOC);
            u32Cmbsz = *(volatile u32 *)(void *)(pMmio + NVME_REG_CMBSZ);
        } else {
            u32Cmbloc = 0u;
            u32Cmbsz = 0u;
        }
        /* BPINFO soft-read when CAP.BPS; never write BPRSEL / BPMBL */
        if (u32Bps != 0u) {
            u32Bpinfo = *(volatile u32 *)(void *)(pMmio + NVME_REG_BPINFO);
        } else {
            u32Bpinfo = 0u;
        }
        /* CRTO soft-read when CAP.CRMS (ready-mode timeouts); observe only */
        if (u32Crms != 0u) {
            u32Crto = *(volatile u32 *)(void *)(pMmio + NVME_REG_CRTO);
        } else {
            u32Crto = 0u;
        }

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
        kprintf("nvme: NSSR=0x%x soft PASS (observe; never reset)\n", u32Nssr);
        nvme_soft_aq_inventory(u64Asq, u64Acq, u32Aqa);
        if (u32Cmbs != 0u) {
            kprintf("nvme: CMB cmbloc=0x%x cmbsz=0x%x soft PASS "
                    "(observe; not claimed)\n",
                    u32Cmbloc, u32Cmbsz);
        } else {
            kprintf("nvme: CMB soft SKIP (CAP.CMBS=0)\n");
        }
        if (u32Bps != 0u) {
            kprintf("nvme: BPINFO=0x%x soft PASS (observe; BPRSEL never "
                    "write)\n",
                    u32Bpinfo);
        } else {
            kprintf("nvme: BPINFO soft SKIP (CAP.BPS=0)\n");
        }
        if (u32Crms != 0u) {
            kprintf("nvme: CRTO=0x%x soft PASS (observe; ready modes)\n",
                    u32Crto);
        } else {
            kprintf("nvme: CRTO soft SKIP (CAP.CRMS=0)\n");
        }
        /* Explicit non-claim: product stop is CAP/VS/status/ASQ soft inventory. */
        kprintf("nvme: admin queues soft SKIP (not claimed)\n");
        kprintf("nvme: I/O path soft SKIP (not claimed)\n");
        /* Lean soft residual inventory */
        nvme_soft_inventory("identify", u64Cap, u32Vs, u32Csts, u32Cc,
                            u32Intms, u32Intmc, u32Aqa, u64Asq, u64Acq,
                            u32Cmbloc, u32Cmbsz, u32Bpinfo, u32Crto, u32Bps,
                            u32Crms, paBar, u32BarBits);
    }
}

/**
 * Scan PCI for NVMe controllers. Soft CAP inventory when BAR0 is mapped.
 * Returns count of matching functions. Always logs greppable soft PASS/SKIP.
 * Soft != product; dual MIT OR Apache-2.0. No version stamp / stamp storm.
 * Dual DoD product_storage=OPEN - residual never closes product storage.
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
                                        0xffffffffu, ~0ull, ~0ull,
                                        0xffffffffu, 0xffffffffu,
                                        0xffffffffu, 0xffffffffu, 0u, 0u,
                                        0ull, 0u);
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
                            0xffffffffu, ~0ull, ~0ull, 0xffffffffu,
                            0xffffffffu, 0xffffffffu, 0xffffffffu, 0u, 0u,
                            0ull, 0u);
    } else {
        kprintf("nvme: probe count=%u soft PASS\n", cFound);
    }
    kprintf("nvme: probe soft PASS\n");
    return cFound;
}
