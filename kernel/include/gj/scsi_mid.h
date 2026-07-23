/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * SCSI mid-layer shapes (product: userspace scsi_mid server).
 * Clean-room public SCSI architecture — dual MIT OR Apache-2.0, pure C,
 * no GPL source. Kernel interim mid fills CDBs and forwards to virtio-scsi;
 * door-shaped submit is the M5 bridge used by GJ_SYS_SCSI / store_door CAP.
 */
#pragma once

#include <gj/types.h>

#define GJ_SCSI_CDB_MAX   16
#define GJ_SCSI_SENSE_MAX 32

/** SPC/SBC opcodes exercised on bring-up and product smokes. */
#define GJ_SCSI_OP_TEST_UNIT        0x00
#define GJ_SCSI_OP_INQUIRY          0x12
#define GJ_SCSI_OP_READ_CAPACITY_10 0x25
#define GJ_SCSI_OP_READ_10          0x28
#define GJ_SCSI_OP_WRITE_10         0x2A

/** scsi_door_req.u32Op — door-shaped submit opcodes (M5). */
#define GJ_SCSI_DOOR_OP_INQUIRY  0u
#define GJ_SCSI_DOOR_OP_READ_CAP 1u
#define GJ_SCSI_DOOR_OP_READ10   2u
#define GJ_SCSI_DOOR_OP_WRITE10  3u
#define GJ_SCSI_DOOR_OP_RAW      4u

enum gj_scsi_status {
    GJ_SCSI_GOOD            = 0,
    GJ_SCSI_CHECK_CONDITION = 2,
    GJ_SCSI_BUSY            = 8,
    GJ_SCSI_TASK_SET_FULL   = 0x28,
};

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

/** Build INQUIRY CDB (6-byte). Null pCdb is a no-op. */
void scsi_cdb_inquiry(struct gj_scsi_cdb *pCdb, u8 u8Evpd, u8 u8Page,
                      u16 u16Alloc);

/** Build READ CAPACITY(10) (10-byte). */
void scsi_cdb_read_capacity10(struct gj_scsi_cdb *pCdb);

/** Build READ(10) / WRITE(10) (10-byte). */
void scsi_cdb_read10(struct gj_scsi_cdb *pCdb, u32 u32Lba, u16 u16Blocks);
void scsi_cdb_write10(struct gj_scsi_cdb *pCdb, u32 u32Lba, u16 u16Blocks);

/**
 * Submit CDB to first available transport (virtio-scsi when ready).
 * Returns 0 on GOOD, -1 on error / no transport / bad args.
 */
int scsi_mid_submit(struct gj_scsi_request *pReq);

void scsi_mid_init(void);
/** Non-zero when interim mid is inited and virtio-scsi is ready. */
int  scsi_mid_ready(void);

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

void scsi_door_init(void);
/**
 * Build CDB from pReq and submit via scsi_mid.
 * Returns 0 on success, -1 on bad args / no transport / I/O error.
 */
int  scsi_door_submit(struct scsi_door_req *pReq, void *pData, u32 cbData);
/** Lifetime successful door submits (product STATS counter). */
u32  scsi_door_io_count(void);
