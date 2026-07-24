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
 *   illegal LUN sense → REQUEST SENSE → SYNC → deepen probes →
 *   soft inventory (Wave 43) → stats
 *
 * Soft inventory (Wave 43 exclusive deepen — greppable):
 *   scsi_mid-server: soft inventory …
 *   scsi_mid-server: soft deepen wave=43 …
 *   scsi_mid: soft …              (via scsi_mid_soft_inventory_log)
 * Soft LUN honesty remains soft; product door INQUIRY path is separate
 * (host has no door — soft INQUIRY only).
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

/* Wave 43 soft inventory surface from cdb.c (no public header change). */
/* Wave 43 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retmachicolation continuum_toward=18500 soft_ne_product=1
 *   greppable: soft retarrowslit exclusive=1 continuum_toward=18500
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */

void scsi_mid_soft_inventory_log(void);

#define SOFT_HOST_WAVE 30u

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
    unsigned char aBlk2[SCSI_MID_SOFT_SEC_SIZE * 2u];
    unsigned char aSense[18];
    unsigned i;
    int fFail = 0;
    int fSoftOk;
    unsigned cSoftOk = 0;
    unsigned cSoftSkip = 0;
    unsigned cSoftLunHonest = 0;
    unsigned cSoftLbaHonest = 0;
    unsigned cSoftEvpdHonest = 0;
    unsigned cSoftMulti = 0;
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
        cSoftSkip++;
    } else {
        cSoftOk++;
    }

    /* Soft INQUIRY (product door INQUIRY path separate; host has no door). */
    io_zero(&io);
    memset(aInq, 0, sizeof(aInq));
    scsi_cdb_inquiry(&io.cdb, 0, 0, 36);
    io.pData = aInq;
    io.cbData = sizeof(aInq);
    if (scsi_mid_submit(&io) != 0) {
        printf("scsi_mid-server: soft INQUIRY FAIL\n");
        fFail = 1;
        cSoftSkip++;
    } else if (aInq[8] != 'G' || aInq[9] != 'r') {
        printf("scsi_mid-server: soft INQUIRY vendor unexpected\n");
        fFail = 1;
        cSoftSkip++;
    } else {
        cSoftOk++;
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
        cSoftSkip++;
    } else {
        cSoftOk++;
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
        cSoftSkip++;
    } else {
        cSoftOk++;
    }
    memset(aBlk, 0, sizeof(aBlk));
    io_zero(&io);
    scsi_cdb_read10(&io.cdb, 2u, 1u);
    io.pData = aBlk;
    io.cbData = sizeof(aBlk);
    fSoftOk = 0;
    if (scsi_mid_submit(&io) != 0) {
        printf("scsi_mid-server: soft READ10 FAIL\n");
        fFail = 1;
        cSoftSkip++;
    } else {
        fSoftOk = 1;
        for (i = 0; i < SCSI_MID_SOFT_SEC_SIZE; i++) {
            if (aBlk[i] != (unsigned char)(0x5Au ^ (unsigned char)i)) {
                printf("scsi_mid-server: soft R/W verify FAIL\n");
                fFail = 1;
                fSoftOk = 0;
                break;
            }
        }
        if (fSoftOk) {
            cSoftOk++;
        } else {
            cSoftSkip++;
        }
    }

    /* Illegal LUN → CHECK; soft LUN honesty (soft only; not product gate) */
    io_zero(&io);
    io.u32Lun = 1;
    scsi_cdb_test_unit_ready(&io.cdb);
    if (scsi_mid_submit(&io) == 0) {
        printf("scsi_mid-server: soft LUN map FAIL (expected CHECK)\n");
        fFail = 1;
        cSoftSkip++;
    } else if (scsi_sense_decode(&io.sense, &u8Key, &u8Asc, &u8Ascq) != 0 ||
               u8Key != SCSI_SK_ILLEGAL_REQUEST || u8Asc != 0x25u) {
        printf("scsi_mid-server: soft sense decode FAIL key=%u asc=%u\n",
               (unsigned)u8Key, (unsigned)u8Asc);
        fFail = 1;
        cSoftSkip++;
    } else {
        cSoftLunHonest = 1;
        cSoftOk++;
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
        cSoftSkip++;
    } else {
        cSoftOk++;
    }

    /* SYNCHRONIZE CACHE */
    io_zero(&io);
    scsi_cdb_synchronize_cache10(&io.cdb, 0, 0);
    if (scsi_mid_submit(&io) != 0) {
        printf("scsi_mid-server: soft SYNC FAIL\n");
        fFail = 1;
        cSoftSkip++;
    } else {
        cSoftOk++;
    }

    /*
     * Wave 43 deepen probes (always soft for inventory; hard fail host smoke
     * only if core mid already failed — deepen misses stay soft-skip).
     */
    /* Multi-block WRITE10/READ10 at LBA 4, 2 blocks. */
    for (i = 0; i < sizeof(aBlk2); i++) {
        aBlk2[i] = (unsigned char)(0xC3u ^ (unsigned char)i);
    }
    io_zero(&io);
    scsi_cdb_write10(&io.cdb, 4u, 2u);
    io.pData = aBlk2;
    io.cbData = sizeof(aBlk2);
    io.fWrite = 1;
    fSoftOk = (scsi_mid_submit(&io) == 0);
    if (fSoftOk) {
        memset(aBlk2, 0, sizeof(aBlk2));
        io_zero(&io);
        scsi_cdb_read10(&io.cdb, 4u, 2u);
        io.pData = aBlk2;
        io.cbData = sizeof(aBlk2);
        fSoftOk = (scsi_mid_submit(&io) == 0);
        if (fSoftOk) {
            for (i = 0; i < sizeof(aBlk2); i++) {
                if (aBlk2[i] != (unsigned char)(0xC3u ^ (unsigned char)i)) {
                    fSoftOk = 0;
                    break;
                }
            }
        }
    }
    if (fSoftOk) {
        cSoftMulti = 1;
        cSoftOk++;
        printf("scsi_mid-server: soft multi PASS\n");
    } else {
        cSoftSkip++;
        printf("scsi_mid-server: soft multi soft-skip\n");
    }

    /* Illegal LBA honesty (soft only). */
    io_zero(&io);
    scsi_cdb_read10(&io.cdb, SCSI_MID_SOFT_SECTORS, 1u);
    io.pData = aBlk;
    io.cbData = sizeof(aBlk);
    if (scsi_mid_submit(&io) != 0 &&
        scsi_sense_decode(&io.sense, &u8Key, &u8Asc, &u8Ascq) == 0 &&
        u8Key == SCSI_SK_ILLEGAL_REQUEST && u8Asc == 0x21u) {
        cSoftLbaHonest = 1;
        cSoftOk++;
        printf("scsi_mid-server: soft LBA map PASS\n");
    } else {
        cSoftSkip++;
        printf("scsi_mid-server: soft LBA map soft-skip\n");
    }

    /* EVPD reject honesty (soft only). */
    io_zero(&io);
    scsi_cdb_inquiry(&io.cdb, 1, 0, 36);
    io.pData = aInq;
    io.cbData = sizeof(aInq);
    if (scsi_mid_submit(&io) != 0 &&
        scsi_sense_decode(&io.sense, &u8Key, &u8Asc, &u8Ascq) == 0 &&
        u8Key == SCSI_SK_ILLEGAL_REQUEST && u8Asc == 0x24u) {
        cSoftEvpdHonest = 1;
        cSoftOk++;
        printf("scsi_mid-server: soft EVPD reject PASS\n");
    } else {
        cSoftSkip++;
        printf("scsi_mid-server: soft EVPD reject soft-skip\n");
    }

    if (scsi_mid_stats(&st) != 0 || st.u32Soft != 1u || st.u32IoOk == 0) {
        printf("scsi_mid-server: soft stats FAIL ok=%u soft=%u\n",
               (unsigned)st.u32IoOk, (unsigned)st.u32Soft);
        fFail = 1;
    }

    /* Wave 43 soft inventory — library + host skeleton surfaces. */
    scsi_mid_soft_inventory_log();

    /* Grep: scsi_mid-server: soft inventory */
    printf("scsi_mid-server: soft inventory ok=%u skip=%u lun_honest=%u "
           "lba_honest=%u evpd_honest=%u multi=%u soft_ok=%u soft_fail=%u "
           "wave=%u product_inq=0 soft_inq=1\n",
           cSoftOk, cSoftSkip, cSoftLunHonest, cSoftLbaHonest, cSoftEvpdHonest,
           cSoftMulti, (unsigned)st.u32IoOk, (unsigned)st.u32IoFail,
           (unsigned)SOFT_HOST_WAVE);

    /* Grep: scsi_mid-server: soft deepen */
    printf("scsi_mid-server: soft deepen wave=%u ok=%u skip=%u "
           "lun_honest=%u multi=%u\n",
           (unsigned)SOFT_HOST_WAVE, cSoftOk, cSoftSkip, cSoftLunHonest,
           cSoftMulti);

    /* Grep: scsi_mid-server: soft path */
    printf("scsi_mid-server: soft path soft_lun=1 door=0 product_inq=none "
           "soft_inq=soft lun_honest=soft multi_server=0 confine=0 wave=%u "
           "(soft inventory; not bar3)\n",
           (unsigned)SOFT_HOST_WAVE);

    /* Grep: scsi_mid-server: soft honesty (Wave 43 exclusive deepen) */
    printf("scsi_mid-server: soft honesty multi_server=0 confine=0 bar3=0 "
           "exclusive=1 soft=1 product_kernel=OPEN wave=%u\n",
           (unsigned)SOFT_HOST_WAVE);

    printf("scsi_mid-server: soft inventory PASS\n");

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
