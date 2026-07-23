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
 * Soft door inventory (Wave 10 exclusive deepen — this unit only):
 *   - Submit enter / ok / fail; per-op ok + reject tallies
 *   - Soft deny reasons: null_req / null_data / mid_not_ready / blocks /
 *     empty_data / bad_raw / mid_submit / unknown_op
 *   - blocks_ok / blocks_truncate notes; stats/init inventory samples
 *   greppable: "scsi_door: soft …"
 *   Never hard-gates; diagnostics / smoke grep only (wrap OK).
 */
#include <gj/klog.h>
#include <gj/scsi_mid.h>
#include <gj/string.h>
#include <gj/types.h>

static u32 g_u32DoorIos;
static u32 g_u32DoorFails;

/*
 * Soft product inventory (Wave 10 exclusive). Cumulative path tallies.
 * greppable: scsi_door: soft …
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
static u32 g_u32SoftStatsCalls;   /* scsi_door_stats entries */
static u32 g_u32SoftInitCalls;    /* scsi_door_init entries */
static u32 g_u32SoftInvSamples;   /* soft inventory dump count */
static u8  g_fSoftOnce;           /* one-shot after first submit activity */

static void door_fail(void);
static void soft_inc(u32 *pCtr);
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

/**
 * Greppable soft scsi_door inventory (Wave 10 exclusive; product / smoke).
 * Prefix-stable markers (scsi_door: soft …):
 *   scsi_door: soft inventory  — enter/ok/fail + mid lamps + log_n
 *   scsi_door: soft op         — per-op ok tallies + unknown rejects
 *   scsi_door: soft deny       — soft reject reason catalog
 *   scsi_door: soft blocks     — block validation notes
 *   scsi_door: soft path       — honesty: kernel shim ≠ product scsi_mid
 *   scsi_door: soft inventory PASS / scsi_door: soft PASS
 *
 * Never hard-gates; diagnostics only.
 * greppable: scsi_door: soft
 */
static void
soft_inventory_log(const char *szVia)
{
    const char *szViaSafe;
    u32         u32Ready;
    u32         u32Soft;
    int         fSoftPass;

    soft_inc(&g_u32SoftInvSamples);
    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "unknown";
    u32Ready = scsi_mid_ready() ? 1u : 0u;
    u32Soft = scsi_mid_soft_active() ? 1u : 0u;

    /* Grep: scsi_door: soft inventory */
    kprintf("scsi_door: soft inventory via=%s enter=%u ok=%u fail=%u "
            "ios=%u fails=%u mid_ready=%u soft_lun=%u "
            "stats=%u inits=%u logs=%u "
            "ops=inq,readcap,read10,write10,raw,tur,sync,sense "
            "shim=kernel_mid product=userspace_scsi_mid\n",
            szViaSafe, g_u32SoftEnter, g_u32SoftOk, g_u32SoftFail,
            g_u32DoorIos, g_u32DoorFails, u32Ready, u32Soft,
            g_u32SoftStatsCalls, g_u32SoftInitCalls, g_u32SoftInvSamples);

    /* Grep: scsi_door: soft op */
    kprintf("scsi_door: soft op inq=%u readcap=%u read10=%u write10=%u "
            "raw=%u tur=%u sync=%u sense=%u unknown=%u\n",
            g_u32SoftOpInq, g_u32SoftOpReadCap, g_u32SoftOpRead10,
            g_u32SoftOpWrite10, g_u32SoftOpRaw, g_u32SoftOpTur,
            g_u32SoftOpSync, g_u32SoftOpSense, g_u32SoftOpUnknown);

    /* Grep: scsi_door: soft deny */
    kprintf("scsi_door: soft deny null_req=%u null_data=%u mid_nrdy=%u "
            "blocks=%u empty=%u bad_raw=%u mid_sub=%u unknown_op=%u "
            "fail_total=%u\n",
            g_u32SoftDenyNullReq, g_u32SoftDenyNullData, g_u32SoftDenyMidNrdy,
            g_u32SoftDenyBlocks, g_u32SoftDenyEmpty, g_u32SoftDenyBadRaw,
            g_u32SoftDenyMidSub, g_u32SoftOpUnknown, g_u32SoftFail);

    /* Grep: scsi_door: soft blocks */
    kprintf("scsi_door: soft blocks ok=%u trunc=%u def1=%u "
            "sec_size=%u soft_sectors=%u\n",
            g_u32SoftBlocksOk, g_u32SoftBlocksTrunc, g_u32SoftBlocksDef1,
            (unsigned)GJ_SCSI_SOFT_SEC_SIZE, (unsigned)GJ_SCSI_SOFT_SECTORS);

    /*
     * Honesty line: kernel door is interim shim only.
     * Grep: scsi_door: soft path
     */
    kprintf("scsi_door: soft path claim=kernel_mid_shim "
            "product_userspace_scsi_mid=1 full_door_endpoint=0 "
            "virtio_preferred=1 soft_lun_fallback=1 "
            "store_cap_fallback=1 via=%s\n",
            szViaSafe);

    /*
     * Soft lamp only — mid ready (soft LUN or virtio). Never hard-gates.
     * Grep: scsi_door: soft inventory PASS | scsi_door: soft PASS
     * Grep: scsi_door: soft FAIL
     */
    fSoftPass = (u32Ready != 0) ? 1 : 0;
    if (fSoftPass != 0) {
        kprintf("scsi_door: soft inventory PASS via=%s logs=%u "
                "mid_ready=%u soft_lun=%u\n",
                szViaSafe, g_u32SoftInvSamples, u32Ready, u32Soft);
        kprintf("scsi_door: soft PASS via=%s\n", szViaSafe);
    } else {
        kprintf("scsi_door: soft FAIL via=%s mid_ready=0 "
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

    soft_inc(&g_u32SoftEnter);

    if (pReq == NULL) {
        soft_inc(&g_u32SoftDenyNullReq);
        door_fail();
        soft_maybe_once();
        return -1;
    }
    /* Data-bearing ops need a buffer when cbData > 0. */
    if (cbData > 0 && pData == NULL) {
        soft_inc(&g_u32SoftDenyNullData);
        door_fail();
        soft_maybe_once();
        return -1;
    }
    if (!scsi_mid_ready()) {
        soft_inc(&g_u32SoftDenyMidNrdy);
        door_fail();
        soft_maybe_once();
        return -1;
    }

    u32Op = pReq->u32Op;
    memset(&req, 0, sizeof(req));
    switch (u32Op) {
    case GJ_SCSI_DOOR_OP_INQUIRY:
        /* Default allocation length matches standard INQUIRY page size. */
        scsi_cdb_inquiry(&req.cdb, 0, 0, (u16)(cbData ? cbData : 36u));
        req.fDataIn = 1;
        if (cbData == 0) {
            soft_inc(&g_u32SoftDenyEmpty);
            door_fail();
            soft_maybe_once();
            return -1;
        }
        pOpOk = &g_u32SoftOpInq;
        break;

    case GJ_SCSI_DOOR_OP_READ_CAP:
        scsi_cdb_read_capacity10(&req.cdb);
        req.fDataIn = 1;
        /* Capacity payload is 8 bytes; reject empty windows. */
        if (cbData < 8) {
            soft_inc(&g_u32SoftDenyEmpty);
            door_fail();
            soft_maybe_once();
            return -1;
        }
        pOpOk = &g_u32SoftOpReadCap;
        break;

    case GJ_SCSI_DOOR_OP_READ10:
        u16Blocks = pReq->u16Blocks ? pReq->u16Blocks : 1u;
        if (door_blocks_ok(&u16Blocks, cbData) != 0) {
            soft_inc(&g_u32SoftDenyBlocks);
            door_fail();
            soft_maybe_once();
            return -1;
        }
        scsi_cdb_read10(&req.cdb, pReq->u32Lba, u16Blocks);
        req.fDataIn = 1;
        pOpOk = &g_u32SoftOpRead10;
        break;

    case GJ_SCSI_DOOR_OP_WRITE10:
        u16Blocks = pReq->u16Blocks ? pReq->u16Blocks : 1u;
        if (door_blocks_ok(&u16Blocks, cbData) != 0) {
            soft_inc(&g_u32SoftDenyBlocks);
            door_fail();
            soft_maybe_once();
            return -1;
        }
        scsi_cdb_write10(&req.cdb, pReq->u32Lba, u16Blocks);
        req.fDataIn = 0;
        pOpOk = &g_u32SoftOpWrite10;
        break;

    case GJ_SCSI_DOOR_OP_RAW:
        /* Raw path: caller owns CDB bytes; clamp length defensively. */
        if (pReq->u8CdbLen == 0 || pReq->u8CdbLen > GJ_SCSI_CDB_MAX) {
            soft_inc(&g_u32SoftDenyBadRaw);
            door_fail();
            soft_maybe_once();
            return -1;
        }
        memcpy(req.cdb.aCdb, pReq->aCdb, pReq->u8CdbLen);
        req.cdb.u8CdbLen = pReq->u8CdbLen;
        req.fDataIn = pReq->fDataIn ? 1 : 0;
        pOpOk = &g_u32SoftOpRaw;
        break;

    case GJ_SCSI_DOOR_OP_TEST_UNIT:
        scsi_cdb_test_unit_ready(&req.cdb);
        req.fDataIn = 0;
        pOpOk = &g_u32SoftOpTur;
        break;

    case GJ_SCSI_DOOR_OP_SYNC_CACHE:
        u16Blocks = pReq->u16Blocks; /* 0 = whole medium per SBC soft path */
        scsi_cdb_synchronize_cache10(&req.cdb, pReq->u32Lba, u16Blocks);
        req.fDataIn = 0;
        pOpOk = &g_u32SoftOpSync;
        break;

    case GJ_SCSI_DOOR_OP_REQ_SENSE:
        scsi_cdb_request_sense(&req.cdb, (u8)(cbData > 255u ? 255u : cbData));
        req.fDataIn = 1;
        if (cbData == 0) {
            soft_inc(&g_u32SoftDenyEmpty);
            door_fail();
            soft_maybe_once();
            return -1;
        }
        pOpOk = &g_u32SoftOpSense;
        break;

    default:
        soft_inc(&g_u32SoftOpUnknown);
        door_fail();
        soft_maybe_once();
        return -1;
    }

    req.pData = pData;
    req.cbData = cbData;
    /* Propagate optional LUN from door pad space via u32Lba only for block ops;
     * LUN stays 0 for interim single-target soft/virtio path. */
    req.u32Lun = 0;
    req.u32TimeoutMs = 5000;

    if (scsi_mid_submit(&req) != 0) {
        soft_inc(&g_u32SoftDenyMidSub);
        door_fail();
        soft_maybe_once();
        return -1;
    }
    g_u32DoorIos++;
    soft_inc(&g_u32SoftOk);
    if (pOpOk != NULL) {
        soft_inc(pOpOk);
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
