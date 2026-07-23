/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * SCSI mid-layer shapes (product: userspace scsi_mid server).
 * Clean-room public SCSI architecture — dual MIT OR Apache-2.0, pure C,
 * no GPL source. Kernel interim mid fills CDBs and forwards to virtio-scsi;
 * door-shaped submit is the M5 bridge used by GJ_SYS_SCSI / store_door CAP.
 *
 * Soft path (interim): when virtio-scsi is absent, a small software LUN
 * answers TUR / INQUIRY / READ_CAPACITY / READ10 / WRITE10 / SYNC / REQUEST
 * SENSE so door smokes stay green. Product path remains userspace mid + HBA.
 */
#pragma once

#include <gj/types.h>

#define GJ_SCSI_CDB_MAX   16
#define GJ_SCSI_SENSE_MAX 32

/** Soft LUN geometry (interim mid only; product is userspace + real HBA). */
#define GJ_SCSI_SOFT_SECTORS  64u
#define GJ_SCSI_SOFT_SEC_SIZE 512u

/** SPC/SBC opcodes exercised on bring-up and product smokes. */
#define GJ_SCSI_OP_TEST_UNIT          0x00
#define GJ_SCSI_OP_REQUEST_SENSE      0x03
#define GJ_SCSI_OP_INQUIRY            0x12
#define GJ_SCSI_OP_MODE_SENSE_6       0x1A
#define GJ_SCSI_OP_READ_CAPACITY_10   0x25
#define GJ_SCSI_OP_READ_10            0x28
#define GJ_SCSI_OP_WRITE_10           0x2A
#define GJ_SCSI_OP_SYNCHRONIZE_CACHE  0x35

/** scsi_door_req.u32Op — door-shaped submit opcodes (M5). */
#define GJ_SCSI_DOOR_OP_INQUIRY    0u
#define GJ_SCSI_DOOR_OP_READ_CAP   1u
#define GJ_SCSI_DOOR_OP_READ10     2u
#define GJ_SCSI_DOOR_OP_WRITE10    3u
#define GJ_SCSI_DOOR_OP_RAW        4u
#define GJ_SCSI_DOOR_OP_TEST_UNIT  5u
#define GJ_SCSI_DOOR_OP_SYNC_CACHE 6u
#define GJ_SCSI_DOOR_OP_REQ_SENSE  7u

enum gj_scsi_status {
    GJ_SCSI_GOOD            = 0,
    GJ_SCSI_CHECK_CONDITION = 2,
    GJ_SCSI_BUSY            = 8,
    GJ_SCSI_TASK_SET_FULL   = 0x28,
};

/** Fixed-sense KEY values used by soft LUN (SPC). */
#define GJ_SCSI_SK_NO_SENSE        0x0u
#define GJ_SCSI_SK_ILLEGAL_REQUEST 0x5u

struct gj_scsi_cdb {
    u8 aCdb[GJ_SCSI_CDB_MAX];
    u8 u8CdbLen;
    u8 u8Pad[3];
};

struct gj_scsi_sense {
    u8 aSense[GJ_SCSI_SENSE_MAX];
    u8 u8SenseLen;
    u8 u8Pad[3];
};

struct gj_scsi_request {
    u32 u32Lun;
    u32 u32TimeoutMs;
    struct gj_scsi_cdb cdb;
    void *pData;
    u32    cbData;
    int    fDataIn; /* 1 = device→host */
    u8     u8Status;
    struct gj_scsi_sense sense;
};

/** Build TEST UNIT READY CDB (6-byte). Null pCdb is a no-op. */
void scsi_cdb_test_unit_ready(struct gj_scsi_cdb *pCdb);

/** Build REQUEST SENSE CDB (6-byte). */
void scsi_cdb_request_sense(struct gj_scsi_cdb *pCdb, u8 u8Alloc);

/** Build INQUIRY CDB (6-byte). Null pCdb is a no-op. */
void scsi_cdb_inquiry(struct gj_scsi_cdb *pCdb, u8 u8Evpd, u8 u8Page,
                      u16 u16Alloc);

/** Build MODE SENSE(6) CDB. */
void scsi_cdb_mode_sense6(struct gj_scsi_cdb *pCdb, u8 u8Page, u8 u8Alloc);

/** Build READ CAPACITY(10) (10-byte). */
void scsi_cdb_read_capacity10(struct gj_scsi_cdb *pCdb);

/** Build READ(10) / WRITE(10) (10-byte). */
void scsi_cdb_read10(struct gj_scsi_cdb *pCdb, u32 u32Lba, u16 u16Blocks);
void scsi_cdb_write10(struct gj_scsi_cdb *pCdb, u32 u32Lba, u16 u16Blocks);

/** Build SYNCHRONIZE CACHE(10) (10-byte). */
void scsi_cdb_synchronize_cache10(struct gj_scsi_cdb *pCdb, u32 u32Lba,
                                  u16 u16Blocks);

/**
 * Decode fixed-format sense into KEY/ASC/ASCQ.
 * Returns 0 when sense bytes present, -1 if empty/unknown format.
 */
int scsi_sense_decode(const struct gj_scsi_sense *pSense, u8 *pKey, u8 *pAsc,
                      u8 *pAscq);

/**
 * Submit CDB to first available transport:
 *   1) virtio-scsi when ready (product interim HBA)
 *   2) soft LUN when no HBA (bring-up soft path)
 * Returns 0 on GOOD, -1 on error / not inited / bad args.
 */
int scsi_mid_submit(struct gj_scsi_request *pReq);

void scsi_mid_init(void);
/** Non-zero when interim mid is inited and (virtio-scsi or soft LUN) ready. */
int  scsi_mid_ready(void);
/** Non-zero when soft LUN is active and virtio-scsi is not preferred. */
int  scsi_mid_soft_active(void);
/** Successful mid submits (virtio + soft). */
u32  scsi_mid_io_count(void);
/** Failed mid submits (arg / transport / soft CHECK). */
u32  scsi_mid_fail_count(void);

/**
 * Door-shaped request for userspace scsi_mid (M5) and kernel store CAP
 * fallback. Raw op carries aCdb[0..u8CdbLen).
 */
struct scsi_door_req {
    u32 u32Op; /* GJ_SCSI_DOOR_OP_* */
    u32 u32Lba;
    u16 u16Blocks;
    u16 u16Pad;
    u8  aCdb[GJ_SCSI_CDB_MAX];
    u8  u8CdbLen;
    u8  fDataIn;
    u16 u16Pad2;
    u32 cbData;
};

/** Door stats snapshot (product STATS surface). */
struct scsi_door_stats {
    u32 u32Ios;      /* successful submits */
    u32 u32Fails;    /* rejected / I/O errors */
    u32 u32Ready;    /* 1 if mid ready */
    u32 u32Soft;     /* 1 if soft LUN active (no virtio preferred) */
};

void scsi_door_init(void);
/**
 * Build CDB from pReq and submit via scsi_mid.
 * Returns 0 on success, -1 on bad args / no transport / I/O error.
 */
int  scsi_door_submit(struct scsi_door_req *pReq, void *pData, u32 cbData);
/** Lifetime successful door submits (product STATS counter). */
u32  scsi_door_io_count(void);
/** Lifetime failed door submits (arg reject or mid error). */
u32  scsi_door_fail_count(void);
/** Fill stats snapshot; null pOut is a no-op returning -1. */
int  scsi_door_stats(struct scsi_door_stats *pOut);
