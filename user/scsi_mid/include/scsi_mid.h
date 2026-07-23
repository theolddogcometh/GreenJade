/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Userspace SCSI mid-layer public API (product path, pure C11).
 *
 * Ownership (target): CDB build, LUN routing, sense, timeouts, soft LUN
 * policy → HBA host via door when present. Kernel interim may still mirror
 * CDB helpers until mid ownership is fully userspace; DMA/IRQ windows stay
 * in the kernel.
 *
 * Soft path: when no door/HBA is available (host CI or freestanding no-HBA),
 * a tiny software LUN answers TUR / INQUIRY / MODE SENSE / READ CAPACITY /
 * READ10 / WRITE10 / SYNC / REQUEST SENSE so mid smokes stay green.
 *
 * Dual license: MIT OR Apache-2.0 (see tree LICENSE). Clean-room C only —
 * no Linux kernel source.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SCSI_MID_CDB_MAX   16
#define SCSI_MID_SENSE_MAX 32

/* Soft LUN geometry (product userspace soft path; interim bring-up). */
#define SCSI_MID_SOFT_SECTORS  64u
#define SCSI_MID_SOFT_SEC_SIZE 512u

/* SPC / SBC opcodes used by the mid builders (subset). */
#define SCSI_OP_TEST_UNIT_READY  0x00
#define SCSI_OP_REQUEST_SENSE    0x03
#define SCSI_OP_INQUIRY          0x12
#define SCSI_OP_MODE_SENSE_6     0x1A
#define SCSI_OP_READ_CAPACITY_10 0x25
#define SCSI_OP_READ_10          0x28
#define SCSI_OP_WRITE_10         0x2A
#define SCSI_OP_SYNCHRONIZE_CACHE 0x35

/* SCSI status byte (subset). */
#define SCSI_STATUS_GOOD            0x00
#define SCSI_STATUS_CHECK_CONDITION 0x02
#define SCSI_STATUS_BUSY            0x08

/* Fixed-sense KEY values used by soft LUN (SPC). */
#define SCSI_SK_NO_SENSE        0x0u
#define SCSI_SK_ILLEGAL_REQUEST 0x5u

/** Fixed-size CDB buffer + valid length (6 or 10 for helpers here). */
struct scsi_cdb {
    uint8_t aCdb[SCSI_MID_CDB_MAX];
    uint8_t u8Len;
};

/**
 * Fixed-format sense buffer + KEY/ASC/ASCQ triple.
 * aSense[0..] holds the raw fixed sense when present; u8SenseLen is valid
 * length. The triple is a decoded convenience (also filled by soft path).
 */
struct scsi_sense {
    uint8_t aSense[SCSI_MID_SENSE_MAX];
    uint8_t u8SenseLen;
    uint8_t u8Key;
    uint8_t u8Asc;
    uint8_t u8Ascq;
};

/**
 * One mid-layer I/O: LUN + CDB + data window + timeout + result.
 * Build CDB with scsi_cdb_*, then pass to scsi_mid_submit.
 */
struct scsi_io {
    uint32_t u32Lun;
    struct scsi_cdb cdb;
    void    *pData;
    size_t   cbData;
    int      fWrite; /* 0 = read into pData, 1 = write from pData */
    uint32_t u32TimeoutMs;
    struct scsi_sense sense;
    int      iStatus; /* 0 ok, non-zero fail / SCSI status otherwise */
};

/** Soft / mid stats snapshot (product STATS-shaped surface). */
struct scsi_mid_stats {
    uint32_t u32IoOk;   /* successful soft or door submits */
    uint32_t u32IoFail; /* rejected / CHECK / unsupported */
    uint32_t u32Ready;  /* 1 if door ready or soft armed */
    uint32_t u32Soft;   /* 1 if soft LUN is the active path */
};

/* --- CDB builders (big-endian fields per SPC/SBC; zero unused bytes) --- */

void scsi_cdb_test_unit_ready(struct scsi_cdb *pCdb);
void scsi_cdb_request_sense(struct scsi_cdb *pCdb, uint8_t u8Alloc);
void scsi_cdb_inquiry(struct scsi_cdb *pCdb, uint8_t u8Evpd, uint8_t u8Page,
                      uint16_t u16Alloc);
void scsi_cdb_mode_sense6(struct scsi_cdb *pCdb, uint8_t u8Page,
                          uint8_t u8Alloc);
void scsi_cdb_read_capacity10(struct scsi_cdb *pCdb);
void scsi_cdb_read10(struct scsi_cdb *pCdb, uint32_t u32Lba, uint16_t u16Blocks);
void scsi_cdb_write10(struct scsi_cdb *pCdb, uint32_t u32Lba, uint16_t u16Blocks);
void scsi_cdb_synchronize_cache10(struct scsi_cdb *pCdb, uint32_t u32Lba,
                                  uint16_t u16Blocks);

/**
 * Decode fixed-format sense (0x70/0x71) into KEY/ASC/ASCQ.
 * Returns 0 when sense bytes present, -1 if empty/unknown format.
 */
int scsi_sense_decode(const struct scsi_sense *pSense, uint8_t *pKey,
                      uint8_t *pAsc, uint8_t *pAscq);

/**
 * Arm soft LUN (geometry zeroed, sense cleared, counters reset).
 * Safe to call more than once; idempotent re-arm zeros the soft disk.
 */
void scsi_mid_soft_init(void);

/** Non-zero when soft LUN is armed. */
int scsi_mid_soft_ready(void);

/**
 * Submit I/O exclusively to the userspace soft LUN (never the door).
 * Returns 0 on GOOD, -1 on CHECK / bad args / soft not armed.
 */
int scsi_mid_soft_submit(struct scsi_io *pIo);

/**
 * Fill stats snapshot (soft counters + ready/soft flags).
 * Null pOut is a no-op returning -1.
 */
int scsi_mid_stats(struct scsi_mid_stats *pOut);

/**
 * Submit SCSI I/O built in pIo.
 *
 * Freestanding GJ (SCSI_HAS_SYS): map CDB opcode → GJ_SCSI_OP_* over
 * GJ_SYS_SCSI when the door is ready; else fall through to soft LUN.
 * Host-only / no door: soft LUN (after scsi_mid_soft_init, or auto-arm).
 *
 * Returns 0 on success, -1 on failure / unsupported.
 * Sets pIo->iStatus and sense on CHECK-shaped soft failures.
 */
int scsi_mid_submit(struct scsi_io *pIo);

#ifdef __cplusplus
}
#endif
