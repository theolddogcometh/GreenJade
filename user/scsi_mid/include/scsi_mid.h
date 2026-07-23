/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Userspace SCSI mid-layer public API (product path, pure C11).
 *
 * Ownership (target): CDB build, LUN routing, sense, timeouts → HBA host.
 * Kernel interim may still mirror CDB helpers until mid ownership is fully
 * userspace; DMA/IRQ windows stay in the kernel.
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

#define SCSI_MID_CDB_MAX 16

/* SPC / SBC opcodes used by the mid builders (subset). */
#define SCSI_OP_INQUIRY          0x12
#define SCSI_OP_READ_CAPACITY_10 0x25
#define SCSI_OP_READ_10          0x28
#define SCSI_OP_WRITE_10         0x2A

/** Fixed-size CDB buffer + valid length (6 or 10 for helpers here). */
struct scsi_cdb {
    uint8_t aCdb[SCSI_MID_CDB_MAX];
    uint8_t u8Len;
};

/** Fixed sense triple (KEY / ASC / ASCQ); filled when submit surfaces sense. */
struct scsi_sense {
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

/* --- CDB builders (big-endian fields per SPC/SBC; zero unused bytes) --- */

void scsi_cdb_inquiry(struct scsi_cdb *pCdb, uint8_t u8Evpd, uint8_t u8Page,
                      uint16_t u16Alloc);
void scsi_cdb_read_capacity10(struct scsi_cdb *pCdb);
void scsi_cdb_read10(struct scsi_cdb *pCdb, uint32_t u32Lba, uint16_t u16Blocks);
void scsi_cdb_write10(struct scsi_cdb *pCdb, uint32_t u32Lba, uint16_t u16Blocks);

/**
 * Submit SCSI I/O built in pIo.
 *
 * Freestanding GJ: maps CDB opcode → GJ_SCSI_OP_* over GJ_SYS_SCSI
 * (kernel scsi_door → virtio-scsi interim).
 * Host-only link: returns -1 (ENOSYS-shaped); no HBA transfer.
 *
 * Returns 0 on success, -1 on failure / unsupported / host stub.
 * Sets pIo->iStatus accordingly.
 */
int scsi_mid_submit(struct scsi_io *pIo);

#ifdef __cplusplus
}
#endif
