/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Host skeleton for the userspace scsi_mid product path (pure C11).
 *
 * Role
 * ----
 * Compile/link smoke on a host toolchain (no GJ freestanding, no kernel door).
 * Arms the userspace soft LUN and walks a mid-policy sequence:
 *   soft_init → TUR → INQUIRY → READ_CAP → WRITE10/READ10 verify →
 *   illegal LUN sense → REQUEST SENSE → SYNC → stats
 *
 * On host-only links, src/cdb.c compiles with SCSI_HAS_SYS=0, so
 * scsi_mid_submit routes to soft LUN (auto-armed). That is expected:
 * soft PASS keeps CI green without a kernel door.
 *
 * Live product path (GJ_SYS_SCSI → scsi_door → virtio-scsi) is freestanding:
 *   src/scsi_mid_gj.c  →  make scsi_mid-gj  →  build/user/scsi_mid.elf
 */
#include <scsi_mid.h>
#include <stdio.h>
#include <string.h>

static int
io_zero(struct scsi_io *pIo)
{
    if (pIo == NULL) {
        return -1;
    }
    memset(pIo, 0, sizeof(*pIo));
    pIo->u32Lun = 0;
    pIo->u32TimeoutMs = 5000;
    return 0;
}

int
main(void)
{
    struct scsi_io io;
    struct scsi_mid_stats st;
    unsigned char aInq[36];
    unsigned char aCap[8];
    unsigned char aBlk[SCSI_MID_SOFT_SEC_SIZE];
    unsigned char aSense[18];
    unsigned i;
    int fFail = 0;
    uint8_t u8Key = 0;
    uint8_t u8Asc = 0;
    uint8_t u8Ascq = 0;

    scsi_mid_soft_init();

    /* TUR */
    io_zero(&io);
    scsi_cdb_test_unit_ready(&io.cdb);
    if (scsi_mid_submit(&io) != 0) {
        printf("scsi_mid-server: soft TUR FAIL\n");
        fFail = 1;
    }

    /* Standard INQUIRY */
    io_zero(&io);
    memset(aInq, 0, sizeof(aInq));
    scsi_cdb_inquiry(&io.cdb, 0, 0, 36);
    io.pData = aInq;
    io.cbData = sizeof(aInq);
    if (scsi_mid_submit(&io) != 0) {
        printf("scsi_mid-server: soft INQUIRY FAIL\n");
        fFail = 1;
    } else if (aInq[8] != 'G' || aInq[9] != 'r') {
        printf("scsi_mid-server: soft INQUIRY vendor unexpected\n");
        fFail = 1;
    }

    /* READ CAPACITY(10) */
    io_zero(&io);
    memset(aCap, 0, sizeof(aCap));
    scsi_cdb_read_capacity10(&io.cdb);
    io.pData = aCap;
    io.cbData = sizeof(aCap);
    if (scsi_mid_submit(&io) != 0) {
        printf("scsi_mid-server: soft READ_CAP FAIL\n");
        fFail = 1;
    }

    /* WRITE10 + READ10 verify at LBA 2 */
    for (i = 0; i < SCSI_MID_SOFT_SEC_SIZE; i++) {
        aBlk[i] = (unsigned char)(0x5Au ^ (unsigned char)i);
    }
    io_zero(&io);
    scsi_cdb_write10(&io.cdb, 2u, 1u);
    io.pData = aBlk;
    io.cbData = sizeof(aBlk);
    io.fWrite = 1;
    if (scsi_mid_submit(&io) != 0) {
        printf("scsi_mid-server: soft WRITE10 FAIL\n");
        fFail = 1;
    }
    memset(aBlk, 0, sizeof(aBlk));
    io_zero(&io);
    scsi_cdb_read10(&io.cdb, 2u, 1u);
    io.pData = aBlk;
    io.cbData = sizeof(aBlk);
    if (scsi_mid_submit(&io) != 0) {
        printf("scsi_mid-server: soft READ10 FAIL\n");
        fFail = 1;
    } else {
        for (i = 0; i < SCSI_MID_SOFT_SEC_SIZE; i++) {
            if (aBlk[i] != (unsigned char)(0x5Au ^ (unsigned char)i)) {
                printf("scsi_mid-server: soft R/W verify FAIL\n");
                fFail = 1;
                break;
            }
        }
    }

    /* Illegal LUN → CHECK; sense decode KEY/ASC */
    io_zero(&io);
    io.u32Lun = 1;
    scsi_cdb_test_unit_ready(&io.cdb);
    if (scsi_mid_submit(&io) == 0) {
        printf("scsi_mid-server: soft LUN map FAIL (expected CHECK)\n");
        fFail = 1;
    } else if (scsi_sense_decode(&io.sense, &u8Key, &u8Asc, &u8Ascq) != 0 ||
               u8Key != SCSI_SK_ILLEGAL_REQUEST || u8Asc != 0x25u) {
        printf("scsi_mid-server: soft sense decode FAIL key=%u asc=%u\n",
               (unsigned)u8Key, (unsigned)u8Asc);
        fFail = 1;
    }

    /* REQUEST SENSE harvest after CHECK */
    io_zero(&io);
    memset(aSense, 0, sizeof(aSense));
    scsi_cdb_request_sense(&io.cdb, 18);
    io.pData = aSense;
    io.cbData = sizeof(aSense);
    if (scsi_mid_submit(&io) != 0) {
        printf("scsi_mid-server: soft REQ_SENSE FAIL\n");
        fFail = 1;
    }

    /* SYNCHRONIZE CACHE */
    io_zero(&io);
    scsi_cdb_synchronize_cache10(&io.cdb, 0, 0);
    if (scsi_mid_submit(&io) != 0) {
        printf("scsi_mid-server: soft SYNC FAIL\n");
        fFail = 1;
    }

    if (scsi_mid_stats(&st) != 0 || st.u32Soft != 1u || st.u32IoOk == 0) {
        printf("scsi_mid-server: soft stats FAIL ok=%u soft=%u\n",
               (unsigned)st.u32IoOk, (unsigned)st.u32Soft);
        fFail = 1;
    }

    if (fFail) {
        printf("scsi_mid-server: host soft FAIL\n");
        return 1;
    }

    printf("scsi_mid-server: host soft PASS "
           "(soft LUN; no kernel door)\n");
    printf("scsi_mid-server: soft stats ok=%u fail=%u\n",
           (unsigned)st.u32IoOk, (unsigned)st.u32IoFail);
    return 0;
}
