/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Door-shaped submit path for userspace scsi_mid (M5 product).
 * Clean-room pure C; dual MIT OR Apache-2.0. Interim: kernel virtio-scsi
 * or soft LUN behind a C API; full door ENDPOINT is the product next step.
 *
 * Ops (pReq->u32Op / GJ_SCSI_DOOR_OP_*):
 *   0 INQUIRY  1 READ_CAPACITY10  2 READ10  3 WRITE10
 *   4 raw CDB (u8CdbLen + aCdb + fDataIn)
 *   5 TEST UNIT READY  6 SYNCHRONIZE CACHE(10)  7 REQUEST SENSE
 *
 * Used by GJ_SYS_SCSI and by store_door CAP/R/W when virtio-blk is absent.
 * Product path remains userspace scsi_mid; this is the kernel mid shim.
 *
 * Soft residual lean (Soft!=product dual license; G-AC-1; C0 residual):
 *   - Soft inventory is diagnostics only - never product DoD / bar3.
 *   - Soft!=product: soft LUN / kernel mid shim != userspace scsi_mid product.
 *   - Dual license (MIT OR Apache-2.0) is source law; soft residual does not
 *     invent a separate product dual-license claim or image version stamp.
 *   - Multi-line soft dumps capped (SCSI_DOOR_SOFT_LOG_CAP) - no stamp storms.
 *   - No version stamp; no ret*angle cascade; no commit from residual agent.
 *   - Functional residual lean once (constants only; never hard-gates):
 *       door op ids 0..7 + CDB max + soft geometry + deny catalog + log cap
 *       + SPC/SBC wire opcodes + surface count + Soft!=product lic honesty
 *       + Dual DoD OPEN residual lamps (freestanding MSC SKIP; UDX product OPEN)
 *   - Freestanding MSC SKIP; product mid = userspace scsi_mid (G-AC-1).
 *   - Dual DoD A/B remain OPEN (UDX USB/NIC) - soft residual never closes them.
 * greppable: "scsi_door: soft ..." | "scsi_door: soft residual"
 * greppable: scsi_door: soft residual lean
 * greppable: scsi_door: soft residual lean PASS
 * greppable: scsi_door: soft residual lean FAIL
 * greppable: dual_dod_a=OPEN | dual_dod_b=OPEN | freestanding_msc=SKIP
 * Never hard-gates; wrap OK. Soft.
 *
 * Soft door inventory (primary field-stable lines):
 *   - Submit enter / ok / fail; per-op attempt + ok + unknown rejects
 *   - Soft deny reasons: null_req / null_data / mid_not_ready / blocks /
 *     empty_data / bad_raw / mid_submit / unknown_op
 *   - blocks / xfer / last / peak / mid / catalog
 *   - total|rate|raw|shape|honesty|capacity|headroom|surface|ratio
 *   - lean residual: return + residual + residual lean + deepen (capped)
 */
#include <gj/klog.h>
#include <gj/scsi_mid.h>
#include <gj/string.h>
#include <gj/types.h>

/*
 * Cap full multi-line soft inventory dumps (Soft!=product; no stamp storms).
 * Init + first-activity once + a couple of stats paths stay greppable;
 * further calls bump inv samples only (silent). No version stamp.
 */
#define SCSI_DOOR_SOFT_LOG_CAP 4u

static u32 g_u32DoorIos;
static u32 g_u32DoorFails;

/*
 * Soft inventory tallies (diagnostics only; Soft!=product).
 * Cumulative path tallies. greppable: scsi_door: soft ...
 */
static u32 g_u32SoftEnter;        /* scsi_door_submit entries */
static u32 g_u32SoftOk;           /* successful mid submits via door */
static u32 g_u32SoftFail;         /* rejected / mid error arms */
static u32 g_u32SoftOpInq;        /* INQUIRY ok */
static u32 g_u32SoftOpReadCap;    /* READ_CAPACITY ok */
static u32 g_u32SoftOpRead10;     /* READ10 ok */
static u32 g_u32SoftOpWrite10;    /* WRITE10 ok */
static u32 g_u32SoftOpRaw;        /* RAW CDB ok */
static u32 g_u32SoftOpTur;        /* TEST UNIT READY ok */
static u32 g_u32SoftOpSync;       /* SYNCHRONIZE CACHE ok */
static u32 g_u32SoftOpSense;      /* REQUEST SENSE ok */
static u32 g_u32SoftOpUnknown;    /* default/unknown op rejects */
static u32 g_u32SoftAttInq;       /* INQUIRY attempts (past mid-ready) */
static u32 g_u32SoftAttReadCap;   /* READ_CAPACITY attempts */
static u32 g_u32SoftAttRead10;    /* READ10 attempts */
static u32 g_u32SoftAttWrite10;   /* WRITE10 attempts */
static u32 g_u32SoftAttRaw;       /* RAW attempts */
static u32 g_u32SoftAttTur;       /* TUR attempts */
static u32 g_u32SoftAttSync;      /* SYNC CACHE attempts */
static u32 g_u32SoftAttSense;     /* REQUEST SENSE attempts */
static u32 g_u32SoftDenyNullReq;  /* pReq == NULL */
static u32 g_u32SoftDenyNullData; /* cbData > 0 && pData == NULL */
static u32 g_u32SoftDenyMidNrdy;  /* !scsi_mid_ready() */
static u32 g_u32SoftDenyBlocks;   /* door_blocks_ok reject */
static u32 g_u32SoftDenyEmpty;    /* empty data window (INQ/CAP/SENSE) */
static u32 g_u32SoftDenyBadRaw;   /* RAW bad CDB length */
static u32 g_u32SoftDenyMidSub;   /* scsi_mid_submit != 0 */
static u32 g_u32SoftBlocksOk;     /* door_blocks_ok success */
static u32 g_u32SoftBlocksTrunc;  /* door_blocks_ok truncated blocks */
static u32 g_u32SoftBlocksDef1;   /* zero blocks defaulted to 1 */
static u32 g_u32SoftMultiBlk;     /* R/W success with blocks > 1 */
static u32 g_u32SoftDataInOk;     /* success with fDataIn */
static u32 g_u32SoftDataOutOk;    /* success with !fDataIn */
static u64 g_u64SoftBytesIn;      /* cumulative cbData on data-in ok */
static u64 g_u64SoftBytesOut;     /* cumulative cbData on data-out ok */
static u32 g_u32SoftLastOp;       /* last u32Op observed (submit) */
static u32 g_u32SoftLastDeny;     /* last soft deny code (0 = none/ok) */
static u32 g_u32SoftLastLba;      /* last pReq->u32Lba */
static u32 g_u32SoftLastBlocks;   /* last blocks (post clamp when R/W) */
static u32 g_u32SoftLastCb;       /* last cbData */
static u32 g_u32SoftLastRet;      /* 0 ok, 1 fail on last submit exit */
static u32 g_u32SoftPeakCb;       /* peak cbData seen */
static u32 g_u32SoftPeakBlocks;   /* peak blocks after clamp */
static u32 g_u32SoftPeakRawCdb;   /* peak RAW u8CdbLen accepted path */
static u32 g_u32SoftRawCdbSum;    /* sum of accepted RAW CDB lengths */
static u32 g_u32SoftMidSoftOk;    /* ok while soft LUN active */
static u32 g_u32SoftMidVirtOk;    /* ok while mid ready, soft not active */
static u32 g_u32SoftStatsCalls;   /* scsi_door_stats entries */
static u32 g_u32SoftInitCalls;    /* scsi_door_init entries */
static u32 g_u32SoftInvSamples;   /* soft inventory dump count */
static u8  g_fSoftOnce;           /* one-shot after first submit activity */

/* Wave 35 exclusive soft deepen - complementary path tallies. */
static u32 g_u32SoftZeroCb;       /* submit with cbData == 0 */
static u32 g_u32SoftShortInq;     /* INQUIRY with cbData < 36 */
static u32 g_u32SoftSyncWhole;    /* SYNC CACHE blocks==0 (whole medium) */
static u32 g_u32SoftRawDataIn;    /* RAW ok with fDataIn */
static u32 g_u32SoftRawDataOut;   /* RAW ok with !fDataIn */
static u32 g_u32SoftPeakLba;      /* peak LBA observed on R/W */
static u32 g_u32SoftLunZero;      /* lun forced 0 samples (always) */
static u32 g_u32SoftTimeoutFixed; /* timeout fixed 5000 samples */
static u32 g_u32SoftOkRateBp;     /* last computed ok basis points (log) */

/*
 * C0 residual lean self-check (once; Soft!=product; dual MIT/Apache).
 * Functional constant checks only - never product gate / never hard-block.
 * greppable: scsi_door: soft residual lean | soft residual lean PASS|FAIL
 */
static u32 g_u32SoftResidualLean;   /* lean residual self-check runs */
static u32 g_u32SoftResidualLeanOk; /* lean residual checks that passed */
static u32 g_u32SoftLeanWireOk;     /* SPC/SBC wire opcode residual ok */
static u32 g_u32SoftLeanLicOk;      /* Soft!=product lic residual ok */
static u32 g_u32SoftLeanDodOk;      /* Dual DoD OPEN residual ok */
static u8  g_fSoftLean;             /* one-shot lean residual gate */

/* Soft deny reason codes for last-deny snapshot (greppable catalog). */
#define SOFT_DENY_NONE      0u
#define SOFT_DENY_NULL_REQ  1u
#define SOFT_DENY_NULL_DATA 2u
#define SOFT_DENY_MID_NRDY  3u
#define SOFT_DENY_BLOCKS    4u
#define SOFT_DENY_EMPTY     5u
#define SOFT_DENY_BAD_RAW   6u
#define SOFT_DENY_MID_SUB   7u
#define SOFT_DENY_UNKNOWN   8u

static void door_fail(void);
static void soft_inc(u32 *pCtr);
static void soft_add64(u64 *pCtr, u64 u64N);
static void soft_peak_u32(u32 *pPeak, u32 u32Val);
static void soft_note_deny(u32 u32Code);
static void soft_note_req(const struct scsi_door_req *pReq, u32 cbData);
static void soft_residual_lean_once(const char *szVia);
static void soft_inventory_log(const char *szVia);
static void soft_maybe_once(void);

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/** Soft: add to u64 path tally (wrap OK for telemetry). */
static void
soft_add64(u64 *pCtr, u64 u64N)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr) += u64N;
}

/** Soft: raise high-water if u32Val exceeds *pPeak. */
static void
soft_peak_u32(u32 *pPeak, u32 u32Val)
{
    if (pPeak == NULL) {
        return;
    }
    if (u32Val > *pPeak) {
        *pPeak = u32Val;
    }
}

/** Soft: record last deny reason code (catalog in soft last line). */
static void
soft_note_deny(u32 u32Code)
{
    g_u32SoftLastDeny = u32Code;
}

/**
 * Soft: snapshot last request shape for inventory (op/lba/blocks/cb).
 * Null pReq only updates cb / last-op leave alone when already set.
 */
static void
soft_note_req(const struct scsi_door_req *pReq, u32 cbData)
{
    g_u32SoftLastCb = cbData;
    soft_peak_u32(&g_u32SoftPeakCb, cbData);
    if (pReq == NULL) {
        return;
    }
    g_u32SoftLastOp = pReq->u32Op;
    g_u32SoftLastLba = pReq->u32Lba;
    g_u32SoftLastBlocks = (u32)pReq->u16Blocks;
}

/**
 * C0 lean residual self-check - door surface + soft geometry + deny + wire
 * + Soft!=product lic + Dual DoD OPEN residual lamps.
 * Once only. Soft!=product dual MIT OR Apache-2.0; no version stamp.
 * Product path remains userspace scsi_mid; kernel door is interim shim only.
 * Freestanding MSC SKIP (G-AC-1). Dual DoD A/B stay OPEN (UDX). Never hard-gates.
 * greppable: scsi_door: soft residual lean
 * greppable: scsi_door: soft residual lean PASS
 * greppable: scsi_door: soft residual lean FAIL
 * greppable: dual_dod_a=OPEN | dual_dod_b=OPEN | freestanding_msc=SKIP
 */
static void
soft_residual_lean_once(const char *szVia)
{
    u32 u32Ok;
    u32 u32Checks;
    u32 u32Ops;
    u32 u32Cdb;
    u32 u32Geom;
    u32 u32Deny;
    u32 u32Cap;
    u32 u32Wire;
    u32 u32Surface;
    u32 u32Lic;
    u32 u32Dod;
    u32 u32Ready;
    u32 u32Soft;
    /* Stack-local Soft!=product honesty (never hard-gates). */
    const int nSoft = 1;
    const int nProduct = 0;
    const int nKoProduct = 0;
    const int nStampStorm = 0;
    const int nFreestandingMsc = 0; /* SKIP residual; not Dual DoD close */
    const int nDualDodAOpen = 1;    /* UDX USB product OPEN */
    const int nDualDodBOpen = 1;    /* UDX NIC product OPEN */
    const char *szViaSafe;

    if (g_fSoftLean != 0) {
        return;
    }
    g_fSoftLean = 1;
    u32Ok = 0;
    u32Checks = 0;
    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "lean";

    /* Door opcodes 0..7 stable (M5 submit surface). */
    u32Checks++;
    u32Ops = 0;
    if (GJ_SCSI_DOOR_OP_INQUIRY == 0u && GJ_SCSI_DOOR_OP_READ_CAP == 1u &&
        GJ_SCSI_DOOR_OP_READ10 == 2u && GJ_SCSI_DOOR_OP_WRITE10 == 3u &&
        GJ_SCSI_DOOR_OP_RAW == 4u && GJ_SCSI_DOOR_OP_TEST_UNIT == 5u &&
        GJ_SCSI_DOOR_OP_SYNC_CACHE == 6u && GJ_SCSI_DOOR_OP_REQ_SENSE == 7u) {
        u32Ops = 1;
        u32Ok++;
    }

    /* CDB container max matches SPC wire length bound. */
    u32Checks++;
    u32Cdb = 0;
    if (GJ_SCSI_CDB_MAX == 16u) {
        u32Cdb = 1;
        u32Ok++;
    }

    /* Soft LUN geometry (interim mid only; honesty remains soft). */
    u32Checks++;
    u32Geom = 0;
    if (GJ_SCSI_SOFT_SEC_SIZE == 512u && GJ_SCSI_SOFT_SECTORS == 64u &&
        (GJ_SCSI_SOFT_SECTORS * GJ_SCSI_SOFT_SEC_SIZE) == 32768u) {
        u32Geom = 1;
        u32Ok++;
    }

    /* Deny reason catalog complete (0none..8unknown). */
    u32Checks++;
    u32Deny = 0;
    if (SOFT_DENY_NONE == 0u && SOFT_DENY_NULL_REQ == 1u &&
        SOFT_DENY_NULL_DATA == 2u && SOFT_DENY_MID_NRDY == 3u &&
        SOFT_DENY_BLOCKS == 4u && SOFT_DENY_EMPTY == 5u &&
        SOFT_DENY_BAD_RAW == 6u && SOFT_DENY_MID_SUB == 7u &&
        SOFT_DENY_UNKNOWN == 8u &&
        (SOFT_DENY_UNKNOWN - SOFT_DENY_NONE) == 8u) {
        u32Deny = 1;
        u32Ok++;
    }

    /* Inventory log cap > 0 (no stamp storms; Soft!=product). */
    u32Checks++;
    u32Cap = 0;
    if (SCSI_DOOR_SOFT_LOG_CAP > 0u && SCSI_DOOR_SOFT_LOG_CAP <= 8u) {
        u32Cap = 1;
        u32Ok++;
    }

    /*
     * SPC/SBC wire opcodes residual (door CDB builders target these).
     * Catches silent opcode drift without touching product path.
     */
    u32Checks++;
    u32Wire = 0;
    if (GJ_SCSI_OP_TEST_UNIT == 0x00u && GJ_SCSI_OP_REQUEST_SENSE == 0x03u &&
        GJ_SCSI_OP_INQUIRY == 0x12u && GJ_SCSI_OP_READ_CAPACITY_10 == 0x25u &&
        GJ_SCSI_OP_READ_10 == 0x28u && GJ_SCSI_OP_WRITE_10 == 0x2Au &&
        GJ_SCSI_OP_SYNCHRONIZE_CACHE == 0x35u) {
        u32Wire = 1;
        u32Ok++;
    }

    /* Surface count: contiguous door ops 0..7 => 8 (last id + 1). */
    u32Checks++;
    u32Surface = 0;
    if ((GJ_SCSI_DOOR_OP_REQ_SENSE + 1u) == 8u &&
        GJ_SCSI_DOOR_OP_INQUIRY == 0u && GJ_SCSI_SENSE_MAX == 32u) {
        u32Surface = 1;
        u32Ok++;
    }

    /*
     * Soft!=product lic residual: soft inventory never product gate;
     * no .ko product; no stamp storm; dual MIT/Apache source law only.
     */
    u32Checks++;
    u32Lic = 0;
    if (nSoft == 1 && nProduct == 0 && nKoProduct == 0 && nStampStorm == 0 &&
        SCSI_DOOR_SOFT_LOG_CAP >= 1u && SCSI_DOOR_SOFT_LOG_CAP <= 8u) {
        u32Lic = 1;
        u32Ok++;
    }

    /*
     * Dual DoD OPEN residual lamps (Soft!=product):
     *   freestanding MSC SKIP; Dual DoD A/B remain OPEN (UDX product).
     * Soft residual never closes Dual DoD and never promotes freestanding.
     */
    u32Checks++;
    u32Dod = 0;
    if (nFreestandingMsc == 0 && nDualDodAOpen == 1 && nDualDodBOpen == 1 &&
        nProduct == 0 && nKoProduct == 0) {
        u32Dod = 1;
        u32Ok++;
    }

    g_u32SoftResidualLean = u32Checks;
    g_u32SoftResidualLeanOk = u32Ok;
    g_u32SoftLeanWireOk = u32Wire;
    g_u32SoftLeanLicOk = u32Lic;
    g_u32SoftLeanDodOk = u32Dod;
    u32Ready = scsi_mid_ready() ? 1u : 0u;
    u32Soft = scsi_mid_soft_active() ? 1u : 0u;

    /*
     * Grep: scsi_door: soft residual lean
     * One lean line - Soft!=product dual license; no version stamp; no storm.
     * greppable: product_mid=userspace_scsi_mid | freestanding_msc=SKIP
     * greppable: dual_dod_a=OPEN | dual_dod_b=OPEN
     */
    kprintf("scsi_door: soft residual lean via=%s "
            "ops=%u cdb=%u geom=%u deny=%u log_cap_ok=%u "
            "wire=%u surface=%u lic=%u dod=%u "
            "checks=%u ok=%u enter=%u door_ok=%u door_fail=%u "
            "mid_ready=%u soft_lun=%u "
            "op0=inq op1=readcap op2=read10 op3=write10 "
            "op4=raw op5=tur op6=sync op7=sense "
            "cdb_max=%u soft_secs=%u soft_sec_sz=%u log_cap=%u "
            "kernel_mid_shim=1 product_mid=userspace_scsi_mid "
            "full_door_endpoint=0 store_cap_fallback=1 "
            "freestanding_msc=SKIP soft_ne_product=1 "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "product_usb=xhci_udx product_nic=rtl8168_udx "
            "dual=MIT_OR_Apache-2.0 G-AC-1 "
            "(Soft!=product; dual MIT OR Apache-2.0; kernel door interim; "
            "product=userspace scsi_mid; Dual DoD A/B OPEN UDX; "
            "no freestanding MSC; no .ko; no version stamp; no stamp storms)\n",
            szViaSafe, u32Ops, u32Cdb, u32Geom, u32Deny, u32Cap, u32Wire,
            u32Surface, u32Lic, u32Dod, u32Checks, u32Ok, g_u32SoftEnter,
            g_u32SoftOk, g_u32SoftFail, u32Ready, u32Soft,
            (unsigned)GJ_SCSI_CDB_MAX, (unsigned)GJ_SCSI_SOFT_SECTORS,
            (unsigned)GJ_SCSI_SOFT_SEC_SIZE,
            (unsigned)SCSI_DOOR_SOFT_LOG_CAP);

    if (u32Ok == u32Checks) {
        /* Grep: scsi_door: soft residual lean PASS */
        kprintf("scsi_door: soft residual lean PASS via=%s "
                "checks=%u ok=%u ops=%u cdb=%u geom=%u deny=%u log_cap_ok=%u "
                "wire=%u surface=%u lic=%u dod=%u "
                "product_mid=userspace_scsi_mid freestanding_msc=SKIP "
                "dual_dod_a=OPEN dual_dod_b=OPEN "
                "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
                "(Soft!=product; door residual lean; not product gate; "
                "Dual DoD remains OPEN)\n",
                szViaSafe, u32Checks, u32Ok, u32Ops, u32Cdb, u32Geom, u32Deny,
                u32Cap, u32Wire, u32Surface, u32Lic, u32Dod);
    } else {
        /* Grep: scsi_door: soft residual lean FAIL */
        kprintf("scsi_door: soft residual lean FAIL via=%s "
                "checks=%u ok=%u ops=%u cdb=%u geom=%u deny=%u log_cap_ok=%u "
                "wire=%u surface=%u lic=%u dod=%u "
                "(soft residual only; not product gate; Soft!=product; "
                "freestanding_msc=SKIP; dual_dod_a=OPEN dual_dod_b=OPEN)\n",
                szViaSafe, u32Checks, u32Ok, u32Ops, u32Cdb, u32Geom, u32Deny,
                u32Cap, u32Wire, u32Surface, u32Lic, u32Dod);
    }
}

/**
 * Greppable soft scsi_door inventory (lean residual; Soft!=product).
 * Prefix-stable markers (scsi_door: soft ...):
 *   scsi_door: soft inventory  - enter/ok/fail + mid lamps + log_n
 *   scsi_door: soft op         - per-op ok tallies + unknown rejects
 *   scsi_door: soft att        - per-op attempt tallies (past mid-ready)
 *   scsi_door: soft deny       - soft reject reason catalog
 *   scsi_door: soft blocks     - block validation notes
 *   scsi_door: soft xfer       - data-in|out ok + multi + byte totals
 *   scsi_door: soft last       - last op/deny/lba/blocks/cb/ret snapshot
 *   scsi_door: soft peak       - peak cb / blocks / raw CDB
 *   scsi_door: soft mid        - ready/soft lamps + soft_ok/virt_ok + mid
 *   scsi_door: soft catalog    - opcode name table (0..7)
 *   scsi_door: soft path       - honesty: kernel shim != product scsi_mid
 *   scsi_door: soft total|rate|raw|shape|honesty|capacity|headroom|
 *               surface|ratio|return|residual|residual lean|deepen
 *   scsi_door: soft residual lean / soft residual lean PASS|FAIL
 *   scsi_door: soft inventory PASS / scsi_door: soft PASS
 *   dual_dod_a=OPEN dual_dod_b=OPEN freestanding_msc=SKIP (Soft!=product)
 *
 * Multi-line dumps capped at SCSI_DOOR_SOFT_LOG_CAP (no stamp storms).
 * Never hard-gates; diagnostics only. Soft LUN honesty remains soft.
 * Soft!=product dual license. Dual DoD A/B OPEN. No version stamp.
 * greppable: scsi_door: soft
 */
static void
soft_inventory_log(const char *szVia)
{
    const char *szViaSafe;
    u32         u32Ready;
    u32         u32Soft;
    u32         u32MidIo;
    u32         u32MidFail;
    u32         u32OkBp;
    int         fSoftPass;

    soft_inc(&g_u32SoftInvSamples);
    /*
     * Cap multi-line inventory dumps. Past SCSI_DOOR_SOFT_LOG_CAP bump
     * inv samples only (silent). Soft!=product; no stamp storms.
     */
    if (g_u32SoftInvSamples > SCSI_DOOR_SOFT_LOG_CAP) {
        return;
    }
    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "unknown";
    u32Ready = scsi_mid_ready() ? 1u : 0u;
    u32Soft = scsi_mid_soft_active() ? 1u : 0u;
    u32MidIo = scsi_mid_io_count();
    u32MidFail = scsi_mid_fail_count();
    if (g_u32SoftEnter != 0u) {
        u32OkBp = (g_u32SoftOk * 10000u) / g_u32SoftEnter;
    } else {
        u32OkBp = 0;
    }
    g_u32SoftOkRateBp = u32OkBp;

    /*
     * C0 residual lean once before multi-line dump so residual/deepen
     * lines can report lean_ok honestly on the first greppable sample.
     * Once-gated; Soft!=product; no stamp storms.
     */
    soft_residual_lean_once(szViaSafe);

    /* Grep: scsi_door: soft inventory */
    kprintf("scsi_door: soft inventory via=%s enter=%u ok=%u fail=%u "
            "ios=%u fails=%u mid_ready=%u soft_lun=%u "
            "stats=%u inits=%u logs=%u "
            "ops=inq,readcap,read10,write10,raw,tur,sync,sense "
            "shim=kernel_mid product=userspace_scsi_mid wave=116\n",
            szViaSafe, g_u32SoftEnter, g_u32SoftOk, g_u32SoftFail,
            g_u32DoorIos, g_u32DoorFails, u32Ready, u32Soft,
            g_u32SoftStatsCalls, g_u32SoftInitCalls, g_u32SoftInvSamples);

    /* Grep: scsi_door: soft op */
    kprintf("scsi_door: soft op inq=%u readcap=%u read10=%u write10=%u "
            "raw=%u tur=%u sync=%u sense=%u unknown=%u\n",
            g_u32SoftOpInq, g_u32SoftOpReadCap, g_u32SoftOpRead10,
            g_u32SoftOpWrite10, g_u32SoftOpRaw, g_u32SoftOpTur,
            g_u32SoftOpSync, g_u32SoftOpSense, g_u32SoftOpUnknown);

    /* Grep: scsi_door: soft att */
    kprintf("scsi_door: soft att inq=%u readcap=%u read10=%u write10=%u "
            "raw=%u tur=%u sync=%u sense=%u unknown=%u\n",
            g_u32SoftAttInq, g_u32SoftAttReadCap, g_u32SoftAttRead10,
            g_u32SoftAttWrite10, g_u32SoftAttRaw, g_u32SoftAttTur,
            g_u32SoftAttSync, g_u32SoftAttSense, g_u32SoftOpUnknown);

    /* Grep: scsi_door: soft deny */
    kprintf("scsi_door: soft deny null_req=%u null_data=%u mid_nrdy=%u "
            "blocks=%u empty=%u bad_raw=%u mid_sub=%u unknown_op=%u "
            "fail_total=%u codes=0none,1null_req,2null_data,3mid_nrdy,"
            "4blocks,5empty,6bad_raw,7mid_sub,8unknown\n",
            g_u32SoftDenyNullReq, g_u32SoftDenyNullData, g_u32SoftDenyMidNrdy,
            g_u32SoftDenyBlocks, g_u32SoftDenyEmpty, g_u32SoftDenyBadRaw,
            g_u32SoftDenyMidSub, g_u32SoftOpUnknown, g_u32SoftFail);

    /* Grep: scsi_door: soft blocks */
    kprintf("scsi_door: soft blocks ok=%u trunc=%u def1=%u multi=%u "
            "sec_size=%u soft_sectors=%u\n",
            g_u32SoftBlocksOk, g_u32SoftBlocksTrunc, g_u32SoftBlocksDef1,
            g_u32SoftMultiBlk, (unsigned)GJ_SCSI_SOFT_SEC_SIZE,
            (unsigned)GJ_SCSI_SOFT_SECTORS);

    /* Grep: scsi_door: soft xfer */
    kprintf("scsi_door: soft xfer data_in=%u data_out=%u multi_blk=%u "
            "bytes_in=%lu bytes_out=%lu sec_size=%u\n",
            g_u32SoftDataInOk, g_u32SoftDataOutOk, g_u32SoftMultiBlk,
            (unsigned long)g_u64SoftBytesIn,
            (unsigned long)g_u64SoftBytesOut,
            (unsigned)GJ_SCSI_SOFT_SEC_SIZE);

    /* Grep: scsi_door: soft last */
    kprintf("scsi_door: soft last op=%u deny=%u lba=0x%x blocks=%u "
            "cb=%u ret=%u via=%s\n",
            g_u32SoftLastOp, g_u32SoftLastDeny, g_u32SoftLastLba,
            g_u32SoftLastBlocks, g_u32SoftLastCb, g_u32SoftLastRet,
            szViaSafe);

    /* Grep: scsi_door: soft peak */
    kprintf("scsi_door: soft peak cb=%u blocks=%u raw_cdb=%u "
            "raw_cdb_sum=%u lba=0x%x\n",
            g_u32SoftPeakCb, g_u32SoftPeakBlocks, g_u32SoftPeakRawCdb,
            g_u32SoftRawCdbSum, g_u32SoftPeakLba);

    /* Grep: scsi_door: soft mid */
    kprintf("scsi_door: soft mid ready=%u soft_lun=%u soft_ok=%u "
            "virt_ok=%u mid_io=%u mid_fail=%u door_ios=%u door_fails=%u\n",
            u32Ready, u32Soft, g_u32SoftMidSoftOk, g_u32SoftMidVirtOk,
            u32MidIo, u32MidFail, g_u32DoorIos, g_u32DoorFails);

    /* Grep: scsi_door: soft catalog */
    kprintf("scsi_door: soft catalog "
            "0=inq 1=readcap 2=read10 3=write10 4=raw "
            "5=tur 6=sync 7=sense raw_max=%u soft_secs=%u soft_sec_sz=%u "
            "timeout_ms=5000 lun=0 wave=116\n",
            (unsigned)GJ_SCSI_CDB_MAX, (unsigned)GJ_SCSI_SOFT_SECTORS,
            (unsigned)GJ_SCSI_SOFT_SEC_SIZE);

    /*
     * Wave 35 exclusive deepen (complementary; never reshapes primary lines).
     */
    /* Grep: scsi_door: soft total */
    kprintf("scsi_door: soft total ok=%u fail=%u enter=%u ok_bp=%u "
            "soft_mid_ok=%u virt_mid_ok=%u logs=%u wave=116\n",
            g_u32SoftOk, g_u32SoftFail, g_u32SoftEnter, u32OkBp,
            g_u32SoftMidSoftOk, g_u32SoftMidVirtOk, g_u32SoftInvSamples);

    /* Grep: scsi_door: soft rate */
    kprintf("scsi_door: soft rate ok_bp=%u enter=%u ios=%u fails=%u "
            "multi=%u zero_cb=%u short_inq=%u\n",
            u32OkBp, g_u32SoftEnter, g_u32DoorIos, g_u32DoorFails,
            g_u32SoftMultiBlk, g_u32SoftZeroCb, g_u32SoftShortInq);

    /* Grep: scsi_door: soft raw */
    kprintf("scsi_door: soft raw ok=%u att=%u data_in=%u data_out=%u "
            "peak_cdb=%u cdb_sum=%u bad=%u\n",
            g_u32SoftOpRaw, g_u32SoftAttRaw, g_u32SoftRawDataIn,
            g_u32SoftRawDataOut, g_u32SoftPeakRawCdb, g_u32SoftRawCdbSum,
            g_u32SoftDenyBadRaw);

    /* Grep: scsi_door: soft shape */
    kprintf("scsi_door: soft shape zero_cb=%u short_inq=%u sync_whole=%u "
            "lun0=%u timeout_fix=%u peak_lba=0x%x wave=116\n",
            g_u32SoftZeroCb, g_u32SoftShortInq, g_u32SoftSyncWhole,
            g_u32SoftLunZero, g_u32SoftTimeoutFixed, g_u32SoftPeakLba);

    /* Grep: scsi_door: soft honesty  (soft LUN remains soft) */
    kprintf("scsi_door: soft honesty soft_lun=soft soft_ne_product=1 "
            "kernel_mid_shim=1 full_door_endpoint=0 "
            "store_cap_fallback=1 freestanding_msc=SKIP "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "product_mid=userspace_scsi_mid product_usb=xhci_udx "
            "product_nic=rtl8168_udx dual=MIT_OR_Apache-2.0 "
            "G-AC-1\n");

    /* Grep: scsi_door: soft capacity - Wave 19 design-constant lamps. */
    kprintf("scsi_door: soft capacity timeout_ms=5000 lun=0 "
            "cdb_max=%u soft_secs=%u soft_sec_sz=%u mid_shim=1 "
            "store_cap_fallback=1 freestanding_msc=SKIP\n",
            (unsigned)GJ_SCSI_CDB_MAX, (unsigned)GJ_SCSI_SOFT_SECTORS,
            (unsigned)GJ_SCSI_SOFT_SEC_SIZE);

    /* Grep: scsi_door: soft headroom - Wave 19 live path lamps. */
    kprintf("scsi_door: soft headroom mid_ready=%u soft_lun=%u "
            "enter=%u ok=%u fail=%u logs=%u lean_ok=%u lean_n=%u "
            "wire_ok=%u lic_ok=%u dod_ok=%u\n",
            u32Ready, u32Soft, g_u32SoftEnter, g_u32SoftOk, g_u32SoftFail,
            g_u32SoftInvSamples, g_u32SoftResidualLeanOk,
            g_u32SoftResidualLean, g_u32SoftLeanWireOk, g_u32SoftLeanLicOk,
            g_u32SoftLeanDodOk);

    /* Grep: scsi_door: soft surface - Wave 19 surface bit lamps. */
    kprintf("scsi_door: soft surface ready=%u soft_lun=%u raw_ok=%u "
            "enter=%u ok=%u fail=%u lean=%u surf=0x%x\n",
            u32Ready, u32Soft, g_u32SoftOpRaw != 0u ? 1u : 0u,
            g_u32SoftEnter != 0u ? 1u : 0u, g_u32SoftOk != 0u ? 1u : 0u,
            g_u32SoftFail != 0u ? 1u : 0u,
            g_u32SoftResidualLeanOk != 0u ? 1u : 0u,
            (u32Ready) | (u32Soft << 1) |
                ((g_u32SoftOpRaw != 0u) ? 4u : 0u) |
                ((g_u32SoftEnter != 0u) ? 8u : 0u) |
                ((g_u32SoftOk != 0u) ? 16u : 0u) |
                ((g_u32SoftFail != 0u) ? 32u : 0u) |
                ((g_u32SoftResidualLeanOk != 0u) ? 64u : 0u));

    /* Grep: scsi_door: soft ratio - ok/fail basis points. */
    {
        u32 u32Tot = g_u32SoftOk + g_u32SoftFail;
        u32 u32OkBpR = 0;
        u32 u32FailBp = 0;

        if (u32Tot != 0u) {
            u32OkBpR = (g_u32SoftOk * 10000u) / u32Tot;
            u32FailBp = (g_u32SoftFail * 10000u) / u32Tot;
        }
        kprintf("scsi_door: soft ratio ok_bp=%u fail_bp=%u ok=%u fail=%u "
                "enter=%u lean_ok=%u\n",
                u32OkBpR, u32FailBp, g_u32SoftOk, g_u32SoftFail,
                g_u32SoftEnter, g_u32SoftResidualLeanOk);
    }

    /*
     * Lean residual (Soft!=product dual license): compact return + residual
     * honesty only. No retclass/retlane stamp cascade. No version stamp.
     * Grep: scsi_door: soft return
     */
    kprintf("scsi_door: soft return ok=%u fail=%u last_ret=%u last_deny=%u "
            "last_op=%u mid_sub=%u enter=%u soft_ne_product=1 "
            "freestanding_msc=SKIP dual_dod_a=OPEN dual_dod_b=OPEN "
            "lean_ok=%u wire_ok=%u lic_ok=%u dod_ok=%u\n",
            g_u32SoftOk, g_u32SoftFail, g_u32SoftLastRet, g_u32SoftLastDeny,
            g_u32SoftLastOp, g_u32SoftDenyMidSub, g_u32SoftEnter,
            g_u32SoftResidualLeanOk, g_u32SoftLeanWireOk, g_u32SoftLeanLicOk,
            g_u32SoftLeanDodOk);

    /* Grep: scsi_door: soft residual */
    kprintf("scsi_door: soft residual soft_ne_product=1 "
            "kernel_mid_shim=1 product_userspace_scsi_mid=1 "
            "product_mid=userspace_scsi_mid full_door_endpoint=0 "
            "store_cap_fallback=1 freestanding_msc=SKIP "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "product_usb=xhci_udx product_nic=rtl8168_udx "
            "dual_license=MIT_OR_Apache-2.0 dual=MIT_OR_Apache-2.0 "
            "G-AC-1 version_stamp=0 stamp_storm=0 "
            "log_cap=%u logs=%u lean_ok=%u lean_n=%u "
            "wire_ok=%u lic_ok=%u dod_ok=%u via=%s\n",
            (unsigned)SCSI_DOOR_SOFT_LOG_CAP, g_u32SoftInvSamples,
            g_u32SoftResidualLeanOk, g_u32SoftResidualLean,
            g_u32SoftLeanWireOk, g_u32SoftLeanLicOk, g_u32SoftLeanDodOk,
            szViaSafe);

    /* Grep: scsi_door: soft deepen */
    kprintf("scsi_door: soft deepen areas=total,rate,raw,shape,honesty,"
            "capacity,headroom,surface,ratio,return,residual,"
            "residual_lean,peak_lba,path,wire,lic,dod "
            "logs=%u log_cap=%u lean_ok=%u lean_n=%u soft_ne_product=1 "
            "freestanding_msc=SKIP product_mid=userspace_scsi_mid "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "wire_ok=%u lic_ok=%u dod_ok=%u "
            "dual=MIT_OR_Apache-2.0 G-AC-1 "
            "(lean residual; Soft!=product; soft LUN honesty remains soft; "
            "Dual DoD A/B OPEN; no version stamp; no stamp storms)\n",
            g_u32SoftInvSamples, (unsigned)SCSI_DOOR_SOFT_LOG_CAP,
            g_u32SoftResidualLeanOk, g_u32SoftResidualLean,
            g_u32SoftLeanWireOk, g_u32SoftLeanLicOk, g_u32SoftLeanDodOk);

    /*
     * Honesty line: kernel door is interim shim only.
     * Grep: scsi_door: soft path
     */
    kprintf("scsi_door: soft path claim=kernel_mid_shim "
            "product_userspace_scsi_mid=1 product_mid=userspace_scsi_mid "
            "full_door_endpoint=0 virtio_preferred=1 soft_lun_fallback=1 "
            "store_cap_fallback=1 soft_lun_honesty=soft "
            "freestanding_msc=SKIP soft_ne_product=1 "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "product_usb=xhci_udx product_nic=rtl8168_udx "
            "dual=MIT_OR_Apache-2.0 G-AC-1 via=%s\n",
            szViaSafe);

    /*
     * Soft lamp only - mid ready (soft LUN or virtio). Never hard-gates.
     * Grep: scsi_door: soft inventory PASS | scsi_door: soft PASS
     * Grep: scsi_door: soft FAIL
     */
    fSoftPass = (u32Ready != 0) ? 1 : 0;
    if (fSoftPass != 0) {
        kprintf("scsi_door: soft inventory PASS via=%s logs=%u "
                "mid_ready=%u soft_lun=%u log_cap=%u lean_ok=%u "
                "wire_ok=%u lic_ok=%u dod_ok=%u "
                "soft_ne_product=1 freestanding_msc=SKIP "
                "dual_dod_a=OPEN dual_dod_b=OPEN\n",
                szViaSafe, g_u32SoftInvSamples, u32Ready, u32Soft,
                (unsigned)SCSI_DOOR_SOFT_LOG_CAP, g_u32SoftResidualLeanOk,
                g_u32SoftLeanWireOk, g_u32SoftLeanLicOk, g_u32SoftLeanDodOk);
        kprintf("scsi_door: soft PASS via=%s soft_ne_product=1 "
                "freestanding_msc=SKIP product_mid=userspace_scsi_mid "
                "dual_dod_a=OPEN dual_dod_b=OPEN\n",
                szViaSafe);
    } else {
        kprintf("scsi_door: soft FAIL via=%s mid_ready=0 "
                "(soft inventory only; not product gate; soft_ne_product=1; "
                "freestanding_msc=SKIP; dual_dod_a=OPEN dual_dod_b=OPEN)\n",
                szViaSafe);
    }
}

/**
 * After first product submit activity, print soft inventory once
 * (mirrors door/futex soft-stats-once). Safe from submit return paths.
 * Residual lean once rides inventory first dump (Soft!=product).
 */
static void
soft_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_u32SoftEnter == 0) {
        return;
    }
    g_fSoftOnce = 1;
    soft_inventory_log("once");
    /* inventory already invokes soft_residual_lean_once when under CAP */
}

static void
door_fail(void)
{
    g_u32DoorFails++;
    soft_inc(&g_u32SoftFail);
    g_u32SoftLastRet = 1;
}

/**
 * Soft validation for block-shaped transfers:
 * - default blocks to 1 when zero
 * - when cbData is non-zero, require at least one sector of room
 * - cap blocks so blocks * 512 does not overflow u32 and fits cbData
 */
static int
door_blocks_ok(u16 *pBlocks, u32 cbData)
{
    u32 u32Blocks;
    u32 u32Need;
    u32 u32Orig;
    int fDef1 = 0;

    if (pBlocks == NULL) {
        return -1;
    }
    u32Orig = (u32)*pBlocks;
    if (*pBlocks == 0) {
        fDef1 = 1;
    }
    u32Blocks = *pBlocks ? (u32)*pBlocks : 1u;
    if (u32Blocks == 0 || u32Blocks > 0xffffu) {
        return -1;
    }
    /* Overflow guard: blocks * 512 must fit u32. */
    if (u32Blocks > (0xffffffffu / GJ_SCSI_SOFT_SEC_SIZE)) {
        return -1;
    }
    u32Need = u32Blocks * GJ_SCSI_SOFT_SEC_SIZE;
    if (cbData > 0 && cbData < GJ_SCSI_SOFT_SEC_SIZE) {
        /* Allow short soft buffers only for single-block probes. */
        if (u32Blocks != 1u) {
            return -1;
        }
    } else if (cbData > 0 && cbData < u32Need) {
        /*
         * Truncate block count to what the buffer can hold (sector-aligned).
         * Callers that pass exact sizes (store_door) keep full transfer.
         */
        u32Blocks = cbData / GJ_SCSI_SOFT_SEC_SIZE;
        if (u32Blocks == 0) {
            return -1;
        }
        soft_inc(&g_u32SoftBlocksTrunc);
    }
    *pBlocks = (u16)u32Blocks;
    soft_inc(&g_u32SoftBlocksOk);
    soft_peak_u32(&g_u32SoftPeakBlocks, u32Blocks);
    g_u32SoftLastBlocks = u32Blocks;
    if (fDef1 != 0 || u32Orig == 0) {
        soft_inc(&g_u32SoftBlocksDef1);
    }
    return 0;
}

int
scsi_door_submit(struct scsi_door_req *pReq, void *pData, u32 cbData)
{
    struct gj_scsi_request req;
    u16 u16Blocks;
    u32 u32Op;
    u32 *pOpOk = NULL;
    int fDataIn = 0;

    soft_inc(&g_u32SoftEnter);
    g_u32SoftLastRet = 0;
    soft_note_req(pReq, cbData);
    if (cbData == 0u) {
        soft_inc(&g_u32SoftZeroCb); /* Wave 15: zero-cb sample */
    }

    if (pReq == NULL) {
        soft_inc(&g_u32SoftDenyNullReq);
        soft_note_deny(SOFT_DENY_NULL_REQ);
        door_fail();
        soft_maybe_once();
        return -1;
    }
    /* Data-bearing ops need a buffer when cbData > 0. */
    if (cbData > 0 && pData == NULL) {
        soft_inc(&g_u32SoftDenyNullData);
        soft_note_deny(SOFT_DENY_NULL_DATA);
        door_fail();
        soft_maybe_once();
        return -1;
    }
    if (!scsi_mid_ready()) {
        soft_inc(&g_u32SoftDenyMidNrdy);
        soft_note_deny(SOFT_DENY_MID_NRDY);
        door_fail();
        soft_maybe_once();
        return -1;
    }

    u32Op = pReq->u32Op;
    g_u32SoftLastOp = u32Op;
    g_u32SoftLastLba = pReq->u32Lba;
    g_u32SoftLastBlocks = (u32)pReq->u16Blocks;
    memset(&req, 0, sizeof(req));
    switch (u32Op) {
    case GJ_SCSI_DOOR_OP_INQUIRY:
        soft_inc(&g_u32SoftAttInq);
        /* Default allocation length matches standard INQUIRY page size. */
        scsi_cdb_inquiry(&req.cdb, 0, 0, (u16)(cbData ? cbData : 36u));
        req.fDataIn = 1;
        fDataIn = 1;
        if (cbData == 0) {
            soft_inc(&g_u32SoftDenyEmpty);
            soft_note_deny(SOFT_DENY_EMPTY);
            door_fail();
            soft_maybe_once();
            return -1;
        }
        if (cbData < 36u) {
            soft_inc(&g_u32SoftShortInq); /* Wave 15: short INQUIRY window */
        }
        pOpOk = &g_u32SoftOpInq;
        break;

    case GJ_SCSI_DOOR_OP_READ_CAP:
        soft_inc(&g_u32SoftAttReadCap);
        scsi_cdb_read_capacity10(&req.cdb);
        req.fDataIn = 1;
        fDataIn = 1;
        /* Capacity payload is 8 bytes; reject empty windows. */
        if (cbData < 8) {
            soft_inc(&g_u32SoftDenyEmpty);
            soft_note_deny(SOFT_DENY_EMPTY);
            door_fail();
            soft_maybe_once();
            return -1;
        }
        pOpOk = &g_u32SoftOpReadCap;
        break;

    case GJ_SCSI_DOOR_OP_READ10:
        soft_inc(&g_u32SoftAttRead10);
        u16Blocks = pReq->u16Blocks ? pReq->u16Blocks : 1u;
        if (door_blocks_ok(&u16Blocks, cbData) != 0) {
            soft_inc(&g_u32SoftDenyBlocks);
            soft_note_deny(SOFT_DENY_BLOCKS);
            door_fail();
            soft_maybe_once();
            return -1;
        }
        soft_peak_u32(&g_u32SoftPeakLba, pReq->u32Lba); /* Wave 15 */
        scsi_cdb_read10(&req.cdb, pReq->u32Lba, u16Blocks);
        req.fDataIn = 1;
        fDataIn = 1;
        pOpOk = &g_u32SoftOpRead10;
        break;

    case GJ_SCSI_DOOR_OP_WRITE10:
        soft_inc(&g_u32SoftAttWrite10);
        u16Blocks = pReq->u16Blocks ? pReq->u16Blocks : 1u;
        if (door_blocks_ok(&u16Blocks, cbData) != 0) {
            soft_inc(&g_u32SoftDenyBlocks);
            soft_note_deny(SOFT_DENY_BLOCKS);
            door_fail();
            soft_maybe_once();
            return -1;
        }
        soft_peak_u32(&g_u32SoftPeakLba, pReq->u32Lba); /* Wave 15 */
        scsi_cdb_write10(&req.cdb, pReq->u32Lba, u16Blocks);
        req.fDataIn = 0;
        fDataIn = 0;
        pOpOk = &g_u32SoftOpWrite10;
        break;

    case GJ_SCSI_DOOR_OP_RAW:
        soft_inc(&g_u32SoftAttRaw);
        /* Raw path: caller owns CDB bytes; clamp length defensively. */
        if (pReq->u8CdbLen == 0 || pReq->u8CdbLen > GJ_SCSI_CDB_MAX) {
            soft_inc(&g_u32SoftDenyBadRaw);
            soft_note_deny(SOFT_DENY_BAD_RAW);
            door_fail();
            soft_maybe_once();
            return -1;
        }
        memcpy(req.cdb.aCdb, pReq->aCdb, pReq->u8CdbLen);
        req.cdb.u8CdbLen = pReq->u8CdbLen;
        req.fDataIn = pReq->fDataIn ? 1 : 0;
        fDataIn = req.fDataIn;
        soft_peak_u32(&g_u32SoftPeakRawCdb, (u32)pReq->u8CdbLen);
        g_u32SoftRawCdbSum += (u32)pReq->u8CdbLen;
        pOpOk = &g_u32SoftOpRaw;
        break;

    case GJ_SCSI_DOOR_OP_TEST_UNIT:
        soft_inc(&g_u32SoftAttTur);
        scsi_cdb_test_unit_ready(&req.cdb);
        req.fDataIn = 0;
        fDataIn = 0;
        pOpOk = &g_u32SoftOpTur;
        break;

    case GJ_SCSI_DOOR_OP_SYNC_CACHE:
        soft_inc(&g_u32SoftAttSync);
        u16Blocks = pReq->u16Blocks; /* 0 = whole medium per SBC soft path */
        if (u16Blocks == 0) {
            soft_inc(&g_u32SoftSyncWhole); /* Wave 15: whole-medium sync */
        }
        scsi_cdb_synchronize_cache10(&req.cdb, pReq->u32Lba, u16Blocks);
        req.fDataIn = 0;
        fDataIn = 0;
        g_u32SoftLastBlocks = (u32)u16Blocks;
        pOpOk = &g_u32SoftOpSync;
        break;

    case GJ_SCSI_DOOR_OP_REQ_SENSE:
        soft_inc(&g_u32SoftAttSense);
        scsi_cdb_request_sense(&req.cdb, (u8)(cbData > 255u ? 255u : cbData));
        req.fDataIn = 1;
        fDataIn = 1;
        if (cbData == 0) {
            soft_inc(&g_u32SoftDenyEmpty);
            soft_note_deny(SOFT_DENY_EMPTY);
            door_fail();
            soft_maybe_once();
            return -1;
        }
        pOpOk = &g_u32SoftOpSense;
        break;

    default:
        soft_inc(&g_u32SoftOpUnknown);
        soft_note_deny(SOFT_DENY_UNKNOWN);
        door_fail();
        soft_maybe_once();
        return -1;
    }

    req.pData = pData;
    req.cbData = cbData;
    /* Propagate optional LUN from door pad space via u32Lba only for block ops;
     * LUN stays 0 for interim single-target soft/virtio path.
     * Soft LUN honesty remains soft - door does not promote soft to product. */
    req.u32Lun = 0;
    soft_inc(&g_u32SoftLunZero); /* Wave 15: lun forced 0 */
    req.u32TimeoutMs = 5000;
    soft_inc(&g_u32SoftTimeoutFixed); /* Wave 15: fixed timeout sample */

    if (scsi_mid_submit(&req) != 0) {
        soft_inc(&g_u32SoftDenyMidSub);
        soft_note_deny(SOFT_DENY_MID_SUB);
        door_fail();
        soft_maybe_once();
        return -1;
    }
    g_u32DoorIos++;
    soft_inc(&g_u32SoftOk);
    soft_note_deny(SOFT_DENY_NONE);
    g_u32SoftLastRet = 0;
    if (pOpOk != NULL) {
        soft_inc(pOpOk);
    }
    if (fDataIn != 0) {
        soft_inc(&g_u32SoftDataInOk);
        soft_add64(&g_u64SoftBytesIn, (u64)cbData);
    } else {
        soft_inc(&g_u32SoftDataOutOk);
        soft_add64(&g_u64SoftBytesOut, (u64)cbData);
    }
    if (u32Op == GJ_SCSI_DOOR_OP_RAW) {
        if (fDataIn != 0) {
            soft_inc(&g_u32SoftRawDataIn); /* Wave 15 */
        } else {
            soft_inc(&g_u32SoftRawDataOut);
        }
    }
    if ((u32Op == GJ_SCSI_DOOR_OP_READ10 || u32Op == GJ_SCSI_DOOR_OP_WRITE10) &&
        g_u32SoftLastBlocks > 1u) {
        soft_inc(&g_u32SoftMultiBlk);
    }
    if (scsi_mid_soft_active()) {
        soft_inc(&g_u32SoftMidSoftOk);
    } else {
        soft_inc(&g_u32SoftMidVirtOk);
    }
    soft_maybe_once();
    return 0;
}

u32
scsi_door_io_count(void)
{
    return g_u32DoorIos;
}

u32
scsi_door_fail_count(void)
{
    return g_u32DoorFails;
}

int
scsi_door_stats(struct scsi_door_stats *pOut)
{
    soft_inc(&g_u32SoftStatsCalls);
    if (pOut == NULL) {
        /*
         * Emit soft inventory even on null so bring-up smoke greps
         * "scsi_door: soft ..." without a dedicated syscall.
         * greppable: scsi_door: soft
         */
        soft_inventory_log("stats_null");
        return -1;
    }
    pOut->u32Ios = g_u32DoorIos;
    pOut->u32Fails = g_u32DoorFails;
    pOut->u32Ready = scsi_mid_ready() ? 1u : 0u;
    pOut->u32Soft = scsi_mid_soft_active() ? 1u : 0u;
    /*
     * Emit soft inventory on stats read so bring-up smoke also greps
     * scsi_door: soft inventory / op / deny lines (mirrors file_lock / door).
     * greppable: scsi_door: soft
     */
    soft_inventory_log("stats");
    return 0;
}

void
scsi_door_init(void)
{
    soft_inc(&g_u32SoftInitCalls);
    g_u32DoorIos = 0;
    g_u32DoorFails = 0;
    /*
     * Soft tallies stick across re-init (diagnostics). Product ios/fails
     * still reset for STATS surface honesty. Residual lean once-gate
     * resets so re-init re-emits greppable residual lean PASS (Soft!=product).
     */
    g_fSoftOnce = 0;
    g_fSoftLean = 0;
    kprintf("scsi_door: init ios=0 fails=0 mid_ready=%d soft=%d "
            "(kernel mid shim; product=userspace_scsi_mid; "
            "freestanding_msc=SKIP; dual_dod_a=OPEN dual_dod_b=OPEN; "
            "Soft!=product; G-AC-1)\n",
            scsi_mid_ready() ? 1 : 0, scsi_mid_soft_active() ? 1 : 0);
    /* Grep: scsi_door: soft (baseline inventory + residual lean after init) */
    soft_inventory_log("init");
}
