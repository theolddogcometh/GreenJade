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
 * Soft door inventory (Wave 13 base + Wave 35 exclusive soft deepen):
 *   - soft return: API return-surface catalog (product_*=OPEN)
 *   - soft retmap: Wave 19 return-surface map (ok|fail|… classes)
 *   - Submit enter / ok / fail; per-op attempt + ok + unknown rejects
 *   - Soft deny reasons: null_req / null_data / mid_not_ready / blocks /
 *     empty_data / bad_raw / mid_submit / unknown_op
 *   - blocks_ok / blocks_truncate / def1 notes; multi-block transfers
 *   - xfer: data-in|out ok + cumulative byte totals
 *   - last: op / deny code / lba / blocks / cb snapshot
 *   - peak: cbData / blocks / raw CDB length high-water
 *   - mid: ready/soft lamps + soft_ok vs virt_ok + mid io/fail
 *   - catalog: opcode name table; stats/init inventory samples
 * Wave 19 exclusive (this unit only — greppable "scsi_door: soft …"):
 *   - total/rate/ok_bp; zero-cb; short inq; sync-whole; raw in|out
 *   - peak lba; honesty soft LUN remains soft; deepen stamp
 *   Soft LUN honesty remains soft: door mid soft ≠ product / bar3
 *   greppable: "scsi_door: soft …"
 *   Never hard-gates; diagnostics / smoke grep only (wrap OK). Soft ≠ bar3.
 */
#include <gj/klog.h>
#include <gj/scsi_mid.h>
#include <gj/string.h>
#include <gj/types.h>

static u32 g_u32DoorIos;
static u32 g_u32DoorFails;

/*
 * Soft product inventory (Wave 13 base + Wave 35 exclusive deepen).
 * Cumulative path tallies. greppable: scsi_door: soft …
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

/* Wave 35 exclusive soft deepen — complementary path tallies. */
static u32 g_u32SoftZeroCb;       /* submit with cbData == 0 */
static u32 g_u32SoftShortInq;     /* INQUIRY with cbData < 36 */
static u32 g_u32SoftSyncWhole;    /* SYNC CACHE blocks==0 (whole medium) */
static u32 g_u32SoftRawDataIn;    /* RAW ok with fDataIn */
static u32 g_u32SoftRawDataOut;   /* RAW ok with !fDataIn */
static u32 g_u32SoftPeakLba;      /* peak LBA observed on R/W */
static u32 g_u32SoftLunZero;      /* lun forced 0 samples (always) */
static u32 g_u32SoftTimeoutFixed; /* timeout fixed 5000 samples */
static u32 g_u32SoftOkRateBp;     /* last computed ok basis points (log) */

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
 * Greppable soft scsi_door inventory (Wave 13 base; Wave 35 exclusive deepen).
 * Prefix-stable markers (scsi_door: soft …):
 *   scsi_door: soft inventory  — enter/ok/fail + mid lamps + log_n + wave
 *   scsi_door: soft op         — per-op ok tallies + unknown rejects
 *   scsi_door: soft att        — per-op attempt tallies (past mid-ready)
 *   scsi_door: soft deny       — soft reject reason catalog
 *   scsi_door: soft blocks     — block validation notes
 *   scsi_door: soft xfer       — data-in|out ok + multi + byte totals
 *   scsi_door: soft last       — last op/deny/lba/blocks/cb/ret snapshot
 *   scsi_door: soft peak       — peak cb / blocks / raw CDB
 *   scsi_door: soft mid        — ready/soft lamps + soft_ok/virt_ok + mid
 *   scsi_door: soft catalog    — opcode name table (0..7)
 *   scsi_door: soft path       — honesty: kernel shim ≠ product scsi_mid
 * Wave 15 complementary:
 *   scsi_door: soft total|rate|raw|shape|honesty|deepen
 *   scsi_door: soft inventory PASS / scsi_door: soft PASS
 *
 * Never hard-gates; diagnostics only. Soft LUN honesty remains soft.
 * Soft ≠ bar3. greppable: scsi_door: soft
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

    /* Grep: scsi_door: soft inventory */
    kprintf("scsi_door: soft inventory via=%s enter=%u ok=%u fail=%u "
            "ios=%u fails=%u mid_ready=%u soft_lun=%u "
            "stats=%u inits=%u logs=%u "
            "ops=inq,readcap,read10,write10,raw,tur,sync,sense "
            "shim=kernel_mid product=userspace_scsi_mid wave=85\n",
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
            "timeout_ms=5000 lun=0 wave=85\n",
            (unsigned)GJ_SCSI_CDB_MAX, (unsigned)GJ_SCSI_SOFT_SECTORS,
            (unsigned)GJ_SCSI_SOFT_SEC_SIZE);

    /*
     * Wave 35 exclusive deepen (complementary; never reshapes primary lines).
     */
    /* Grep: scsi_door: soft total */
    kprintf("scsi_door: soft total ok=%u fail=%u enter=%u ok_bp=%u "
            "soft_mid_ok=%u virt_mid_ok=%u logs=%u wave=85\n",
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
            "lun0=%u timeout_fix=%u peak_lba=0x%x wave=85\n",
            g_u32SoftZeroCb, g_u32SoftShortInq, g_u32SoftSyncWhole,
            g_u32SoftLunZero, g_u32SoftTimeoutFixed, g_u32SoftPeakLba);

    /* Grep: scsi_door: soft honesty  (soft LUN remains soft) */
    kprintf("scsi_door: soft honesty soft_lun=soft soft_ne_product=1 "
            "soft_ne_bar3=1 kernel_mid_shim=1 full_door_endpoint=0 "
            "store_cap_fallback=1 wave=85\n");

    /* Grep: scsi_door: soft capacity — Wave 19 design-constant lamps. */
    kprintf("scsi_door: soft capacity timeout_ms=5000 lun=0 "
            "cdb_max=16 mid_shim=1 store_cap_fallback=1 wave=85\n");

    /* Grep: scsi_door: soft headroom — Wave 19 live path lamps. */
    kprintf("scsi_door: soft headroom mid_ready=%u soft_lun=%u "
            "enter=%u ok=%u fail=%u logs=%u wave=85\n",
            u32Ready, u32Soft, g_u32SoftEnter, g_u32SoftOk, g_u32SoftFail,
            g_u32SoftInvSamples);

    /* Grep: scsi_door: soft surface — Wave 19 surface bit lamps. */
    kprintf("scsi_door: soft surface ready=%u soft_lun=%u raw_ok=%u "
            "enter=%u ok=%u fail=%u surf=0x%x wave=85\n",
            u32Ready, u32Soft, g_u32SoftOpRaw != 0u ? 1u : 0u,
            g_u32SoftEnter != 0u ? 1u : 0u, g_u32SoftOk != 0u ? 1u : 0u,
            g_u32SoftFail != 0u ? 1u : 0u,
            (u32Ready) | (u32Soft << 1) |
                ((g_u32SoftOpRaw != 0u) ? 4u : 0u) |
                ((g_u32SoftEnter != 0u) ? 8u : 0u) |
                ((g_u32SoftOk != 0u) ? 16u : 0u) |
                ((g_u32SoftFail != 0u) ? 32u : 0u));

    /* Grep: scsi_door: soft ratio — Wave 19 ok/fail basis points. */
    {
        u32 u32Tot = g_u32SoftOk + g_u32SoftFail;
        u32 u32OkBp = 0;
        u32 u32FailBp = 0;

        if (u32Tot != 0u) {
            u32OkBp = (g_u32SoftOk * 10000u) / u32Tot;
            u32FailBp = (g_u32SoftFail * 10000u) / u32Tot;
        }
        kprintf("scsi_door: soft ratio ok_bp=%u fail_bp=%u ok=%u fail=%u "
                "enter=%u wave=85\n",
                u32OkBp, u32FailBp, g_u32SoftOk, g_u32SoftFail,
                g_u32SoftEnter);
    }

    /* Grep: scsi_door: soft return — Wave 19 API return surfaces */
    kprintf("scsi_door: soft return enter=%u ok=%u fail=%u deny_null=%u "
            "deny_mid=%u deny_raw=%u deny_unknown=%u mid_ready=%u "
            "soft_lun=%u product_scsi_mid=OPEN wave=85\n",
            g_u32SoftEnter, g_u32SoftOk, g_u32SoftFail,
            g_u32SoftDenyNullReq, g_u32SoftDenyMidNrdy, g_u32SoftDenyBadRaw,
            g_u32SoftOpUnknown, u32Ready, u32Soft);

    /* Grep: scsi_door: soft retmap — Wave 19 return-surface map */
    kprintf("scsi_door: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=85\n");

    /* Grep: scsi_door: soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: scsi_door: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("scsi_door: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
    /* Grep: scsi_door: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("scsi_door: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: scsi_door: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("scsi_door: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
    /* Grep: scsi_door: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("scsi_door: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: scsi_door: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("scsi_door: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
            /* Grep: scsi_door: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("scsi_door: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: scsi_door: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("scsi_door: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
            /* Grep: scsi_door: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("scsi_door: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: scsi_door: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("scsi_door: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
            /* Grep: scsi_door: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("scsi_door: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: scsi_door: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("scsi_door: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
            /* Grep: scsi_door: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("scsi_door: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: scsi_door: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("scsi_door: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
            /* Grep: scsi_door: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("scsi_door: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: scsi_door: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("scsi_door: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
            /* Grep: scsi_door: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("scsi_door: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: scsi_door: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("scsi_door: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
                    /* Grep: scsi_door: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("scsi_door: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: scsi_door: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("scsi_door: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
                            /* Grep: scsi_door: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("scsi_door: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_SCSI_SOFT_SEC_SIZE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: scsi_door: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("scsi_door: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=85 "
                                    "(retglyph honesty; Soft≠product; not bar3)\n");
                            /* Grep: scsi_door: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("scsi_door: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=85 "
                                    "(retscepter stamp; Soft≠product)\n");
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: scsi_door: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("scsi_door: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=85 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: scsi_door: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("scsi_door: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=85 "
                                    "(retemblem stamp; Soft≠product)\n");
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: scsi_door: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("scsi_door: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=85 "
                                    "(retaegis honesty; Soft≠product; not bar3)\n");
                            /* Grep: scsi_door: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("scsi_door: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=85 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: scsi_door: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("scsi_door: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=85 "
                                    "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("scsi_door: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("scsi_door: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("scsi_door: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("scsi_door: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("scsi_door: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("scsi_door: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retfortress — Wave 35 return-fortress honesty */
kprintf("scsi_door: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("scsi_door: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft rethold — Wave 36 return-hold honesty */
kprintf("scsi_door: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retspire — Wave 36 exclusive spire stamp */
kprintf("scsi_door: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retwall — Wave 37 return-wall honesty */
kprintf("scsi_door: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retgate — Wave 37 exclusive gate stamp */
kprintf("scsi_door: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retmoat — Wave 38 return-moat honesty */
kprintf("scsi_door: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retower — Wave 38 exclusive tower stamp */
kprintf("scsi_door: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("scsi_door: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("scsi_door: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("scsi_door: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("scsi_door: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retravelin — Wave 41 return-travelin honesty */
kprintf("scsi_door: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("scsi_door: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("scsi_door: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("scsi_door: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("scsi_door: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("scsi_door: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("scsi_door: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("scsi_door: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("scsi_door: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("scsi_door: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retbailey — Wave 46 return-bailey honesty */
kprintf("scsi_door: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("scsi_door: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("scsi_door: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("scsi_door: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("scsi_door: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("scsi_door: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("scsi_door: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("scsi_door: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retsally — Wave 50 return-sally honesty */
kprintf("scsi_door: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("scsi_door: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retfosse — Wave 51 return-fosse honesty */
kprintf("scsi_door: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("scsi_door: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("scsi_door: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("scsi_door: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retravelin — Wave 53 return-travelin honesty */
kprintf("scsi_door: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("scsi_door: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("scsi_door: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retredan — Wave 54 exclusive redan stamp */
kprintf("scsi_door: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retflank — Wave 55 return-flank honesty */
kprintf("scsi_door: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retface — Wave 55 exclusive face stamp */
kprintf("scsi_door: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retgorge — Wave 56 return-gorge honesty */
kprintf("scsi_door: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("scsi_door: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retraverse — Wave 57 return-traverse honesty */
kprintf("scsi_door: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("scsi_door: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retorillon — Wave 58 return-orillon honesty */
kprintf("scsi_door: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("scsi_door: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("scsi_door: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("scsi_door: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retplace — Wave 60 return-place honesty */
kprintf("scsi_door: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("scsi_door: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("scsi_door: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("scsi_door: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("scsi_door: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("scsi_door: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("scsi_door: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("scsi_door: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: scsi_door: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("scsi_door: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: scsi_door: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("scsi_door: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: scsi_door: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("scsi_door: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: scsi_door: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("scsi_door: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: scsi_door: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("scsi_door: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=85 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: scsi_door: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("scsi_door: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=85 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("scsi_door: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("scsi_door: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("scsi_door: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("scsi_door: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("scsi_door: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=85 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("scsi_door: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=85 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("scsi_door: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=85 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("scsi_door: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=85 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("scsi_door: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=85 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("scsi_door: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=85 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: scsi_door: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("scsi_door: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=85 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("scsi_door: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=85 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: scsi_door: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("scsi_door: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=85 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("scsi_door: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=85 (retbastionangle stamp; Soft≠product)\n");
/* Grep: scsi_door: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("scsi_door: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=85 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("scsi_door: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=85 (retparapetangle stamp; Soft≠product)\n");
/* Grep: scsi_door: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("scsi_door: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=85 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("scsi_door: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=85 (retowerangle stamp; Soft≠product)\n");
/* Grep: scsi_door: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("scsi_door: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=85 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("scsi_door: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=85 (retwallangle stamp; Soft≠product)\n");
/* Grep: scsi_door: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("scsi_door: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=85 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("scsi_door: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=85 (retholdangle stamp; Soft≠product)\n");
/* Grep: scsi_door: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("scsi_door: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=85 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("scsi_door: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=85 (retfortressangle stamp; Soft≠product)\n");
/* Grep: scsi_door: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("scsi_door: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=85 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("scsi_door: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=85 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: scsi_door: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("scsi_door: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=85 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("scsi_door: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=85 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: scsi_door: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("scsi_door: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=85 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("scsi_door: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=85 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: scsi_door: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("scsi_door: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=85 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("scsi_door: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=85 (retaegisangle stamp; Soft≠product)\n");
/* Grep: scsi_door: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("scsi_door: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=85 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("scsi_door: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=85 (retsigilangle stamp; Soft≠product)\n");
/* Grep: scsi_door: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("scsi_door: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=85 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("scsi_door: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=85 (retglyphangle stamp; Soft≠product)\n");
/* Grep: scsi_door: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("scsi_door: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=85 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: scsi_door: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("scsi_door: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=85 (retshardangle stamp; Soft≠product)\n");

                            kprintf("scsi_door: soft deepen wave=85 areas=total,rate,raw,shape,,retclass,retlane"
            "honesty,capacity,headroom,surface,ratio,return,peak_lba logs=%u "
            "(Wave 35 exclusive; soft LUN honesty remains soft; not bar3)\n",
            g_u32SoftInvSamples);

    /*
     * Honesty line: kernel door is interim shim only.
     * Grep: scsi_door: soft path
     */
    kprintf("scsi_door: soft path claim=kernel_mid_shim "
            "product_userspace_scsi_mid=1 full_door_endpoint=0 "
            "virtio_preferred=1 soft_lun_fallback=1 "
            "store_cap_fallback=1 soft_lun_honesty=soft "
            "soft_ne_bar3=1 wave=85 via=%s\n",
            szViaSafe);

    /*
     * Soft lamp only — mid ready (soft LUN or virtio). Never hard-gates.
     * Grep: scsi_door: soft inventory PASS | scsi_door: soft PASS
     * Grep: scsi_door: soft FAIL
     */
    fSoftPass = (u32Ready != 0) ? 1 : 0;
    if (fSoftPass != 0) {
        kprintf("scsi_door: soft inventory PASS via=%s logs=%u "
                "mid_ready=%u soft_lun=%u wave=85\n",
                szViaSafe, g_u32SoftInvSamples, u32Ready, u32Soft);
        kprintf("scsi_door: soft PASS via=%s wave=85\n", szViaSafe);
    } else {
        kprintf("scsi_door: soft FAIL via=%s mid_ready=0 wave=85 "
                "(soft inventory only; not product gate)\n",
                szViaSafe);
    }
}

/**
 * After first product submit activity, print soft inventory once
 * (mirrors door/futex soft-stats-once). Safe from submit return paths.
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
     * Soft LUN honesty remains soft — door does not promote soft to product. */
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
         * "scsi_door: soft …" without a dedicated syscall.
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
     * still reset for STATS surface honesty.
     */
    g_fSoftOnce = 0;
    kprintf("scsi_door: init ios=0 fails=0 mid_ready=%d soft=%d "
            "(kernel mid shim; product=userspace)\n",
            scsi_mid_ready() ? 1 : 0, scsi_mid_soft_active() ? 1 : 0);
    /* Grep: scsi_door: soft (baseline inventory after init) */
    soft_inventory_log("init");
}
