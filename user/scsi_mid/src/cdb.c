/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Userspace SCSI CDB builders + scsi_mid_submit (pure C11).
 *
 * CDB path
 * --------
 * scsi_cdb_* fill struct scsi_cdb (opcode + big-endian fields per SPC/SBC).
 * scsi_mid_submit takes a packed scsi_io and:
 *   - GJ freestanding (GJ_FREESTANDING / __gj_freestanding):
 *       decode CDB → gj_scsi(GJ_SCSI_OP_*) → kernel scsi_door → virtio-scsi
 *   - Host / no door (SCSI_HAS_SYS=0):
 *       always -1 (ENOSYS-shaped); server.c treats that as soft PASS
 *
 * Opcode constants: include/scsi_mid.h (SCSI_OP_*).
 * Door ops:         user/libgj/include/gj/syscalls.h (GJ_SCSI_OP_*).
 */
#include <scsi_mid.h>
#include <string.h>

#if defined(GJ_FREESTANDING) || defined(__gj_freestanding)
#include <gj/syscalls.h>
#define SCSI_HAS_SYS 1
#elif defined(__linux__) && !defined(SCSI_HOST_ONLY)
/* Host CI: link without kernel door; submit stays ENOSYS-shaped. */
#define SCSI_HAS_SYS 0
#else
#define SCSI_HAS_SYS 0
#endif

/*
 * INQUIRY (6-byte CDB, SPC):
 *   [0] opcode 0x12
 *   [1] EVPD
 *   [2] page code
 *   [3..4] allocation length (BE)
 */
void
scsi_cdb_inquiry(struct scsi_cdb *pCdb, uint8_t u8Evpd, uint8_t u8Page,
                 uint16_t u16Alloc)
{
    if (pCdb == NULL) {
        return;
    }
    memset(pCdb, 0, sizeof(*pCdb));
    pCdb->aCdb[0] = SCSI_OP_INQUIRY;
    pCdb->aCdb[1] = u8Evpd ? 1u : 0;
    pCdb->aCdb[2] = u8Page;
    pCdb->aCdb[3] = (uint8_t)((u16Alloc >> 8) & 0xff);
    pCdb->aCdb[4] = (uint8_t)(u16Alloc & 0xff);
    pCdb->u8Len = 6;
}

/*
 * READ CAPACITY (10) (10-byte CDB, SBC):
 *   [0] opcode 0x25; remaining bytes zero for simple capacity probe.
 */
void
scsi_cdb_read_capacity10(struct scsi_cdb *pCdb)
{
    if (pCdb == NULL) {
        return;
    }
    memset(pCdb, 0, sizeof(*pCdb));
    pCdb->aCdb[0] = SCSI_OP_READ_CAPACITY_10;
    pCdb->u8Len = 10;
}

/*
 * READ (10) (10-byte CDB, SBC):
 *   [0] opcode 0x28
 *   [2..5] LBA (BE)
 *   [7..8] transfer length in logical blocks (BE)
 */
void
scsi_cdb_read10(struct scsi_cdb *pCdb, uint32_t u32Lba, uint16_t u16Blocks)
{
    if (pCdb == NULL) {
        return;
    }
    memset(pCdb, 0, sizeof(*pCdb));
    pCdb->aCdb[0] = SCSI_OP_READ_10;
    pCdb->aCdb[2] = (uint8_t)((u32Lba >> 24) & 0xff);
    pCdb->aCdb[3] = (uint8_t)((u32Lba >> 16) & 0xff);
    pCdb->aCdb[4] = (uint8_t)((u32Lba >> 8) & 0xff);
    pCdb->aCdb[5] = (uint8_t)(u32Lba & 0xff);
    pCdb->aCdb[7] = (uint8_t)((u16Blocks >> 8) & 0xff);
    pCdb->aCdb[8] = (uint8_t)(u16Blocks & 0xff);
    pCdb->u8Len = 10;
}

/*
 * WRITE (10): same layout as READ (10); opcode 0x2A.
 */
void
scsi_cdb_write10(struct scsi_cdb *pCdb, uint32_t u32Lba, uint16_t u16Blocks)
{
    if (pCdb == NULL) {
        return;
    }
    scsi_cdb_read10(pCdb, u32Lba, u16Blocks);
    pCdb->aCdb[0] = SCSI_OP_WRITE_10;
}

/*
 * Submit packed I/O.
 *
 * Freestanding: map CDB opcode → GJ_SCSI_OP_* and call gj_scsi.
 * Host:         no door — set iStatus=-1 and return -1 (ENOSYS-shaped).
 *
 * Returns 0 on success, -1 on failure / unsupported / host stub.
 */
int
scsi_mid_submit(struct scsi_io *pIo)
{
    if (pIo == NULL) {
        return -1;
    }
#if SCSI_HAS_SYS
    {
        long nRet;
        unsigned uOp;
        uint32_t u32Blocks;
        uint32_t u32Lba;

        if (pIo->cdb.u8Len == 0) {
            pIo->iStatus = -1;
            return -1;
        }
        switch (pIo->cdb.aCdb[0]) {
        case SCSI_OP_INQUIRY:
            /* Door: arg1=buf, arg2=cb → ret bytes or -errno. */
            nRet = gj_scsi(GJ_SCSI_OP_INQUIRY, (long)(uintptr_t)pIo->pData,
                           (long)(pIo->cbData ? pIo->cbData : 36), 0);
            if (nRet < 0) {
                pIo->iStatus = -1;
                return -1;
            }
            pIo->iStatus = 0;
            return 0;
        case SCSI_OP_READ_CAPACITY_10: {
            unsigned aCap[2];

            /* Door: arg1=u32[2] {last_lba, block_len} → 0. */
            nRet = gj_scsi(GJ_SCSI_OP_READ_CAP, (long)(uintptr_t)aCap, 0, 0);
            if (nRet < 0) {
                pIo->iStatus = -1;
                return -1;
            }
            /* SBC READ CAPACITY (10) data: 4B last LBA + 4B block length (BE). */
            if (pIo->pData != NULL && pIo->cbData >= 8) {
                uint8_t *pOut = (uint8_t *)pIo->pData;

                pOut[0] = (uint8_t)((aCap[0] >> 24) & 0xff);
                pOut[1] = (uint8_t)((aCap[0] >> 16) & 0xff);
                pOut[2] = (uint8_t)((aCap[0] >> 8) & 0xff);
                pOut[3] = (uint8_t)(aCap[0] & 0xff);
                pOut[4] = (uint8_t)((aCap[1] >> 24) & 0xff);
                pOut[5] = (uint8_t)((aCap[1] >> 16) & 0xff);
                pOut[6] = (uint8_t)((aCap[1] >> 8) & 0xff);
                pOut[7] = (uint8_t)(aCap[1] & 0xff);
            }
            pIo->iStatus = 0;
            return 0;
        }
        case SCSI_OP_READ_10:
        case SCSI_OP_WRITE_10:
            /* Decode LBA + block count from CDB; door uses scalar args. */
            u32Lba = ((uint32_t)pIo->cdb.aCdb[2] << 24) |
                     ((uint32_t)pIo->cdb.aCdb[3] << 16) |
                     ((uint32_t)pIo->cdb.aCdb[4] << 8) |
                     (uint32_t)pIo->cdb.aCdb[5];
            u32Blocks = ((uint32_t)pIo->cdb.aCdb[7] << 8) |
                        (uint32_t)pIo->cdb.aCdb[8];
            if (u32Blocks == 0) {
                u32Blocks = 1;
            }
            uOp = (pIo->cdb.aCdb[0] == SCSI_OP_WRITE_10) ? GJ_SCSI_OP_WRITE10
                                                         : GJ_SCSI_OP_READ10;
            /* Door: arg1=lba, arg2=buf, arg3=blocks. */
            nRet = gj_scsi(uOp, (long)u32Lba, (long)(uintptr_t)pIo->pData,
                           (long)u32Blocks);
            if (nRet < 0) {
                pIo->iStatus = -1;
                return -1;
            }
            pIo->iStatus = 0;
            return 0;
        default:
            pIo->iStatus = -1;
            return -1;
        }
    }
#else
    /*
     * Host path (no GJ freestanding / no kernel door): always -1
     * (ENOSYS-shaped). Live I/O is scsi_mid_gj via GJ_SYS_SCSI only.
     */
    pIo->iStatus = -1;
    return -1;
#endif
}
