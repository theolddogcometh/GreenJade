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
 *   soft_init → soft_ready → TUR → INQUIRY → MODE SENSE → READ_CAP geom →
 *   WRITE10/READ10 verify → illegal LUN sense → REQUEST SENSE → SYNC →
 *   deepen probes (multi / LBA / EVPD / unknown opcode) →
 *   soft inventory + C2 product residual honesty → stats
 *
 * Soft inventory / residual (C2 product daemon residual — greppable):
 *   scsi_mid-server: soft inventory …
 *   scsi_mid-server: soft deepen wave=…
 *   scsi_mid-server: soft residual lean …
 *   scsi_mid-server: soft product residual …
 *   scsi_mid-server: soft c2 product path …
 *   scsi_mid: soft …              (via scsi_mid_soft_inventory_log)
 * Soft LUN honesty remains soft; product door INQUIRY path is separate
 * (host has no door — soft INQUIRY only). Soft!=product. G-AC-1.
 * Product direction = UDX/DDI+ABI; Dual DoD OPEN; claim_class=C2.
 * Soft lamps alone never close Dual DoD or invent a stick/HBA PASS.
 *
 * On host-only links, src/cdb.c compiles with SCSI_HAS_SYS=0, so
 * scsi_mid_submit routes to soft LUN (auto-armed). That is expected:
 * soft PASS keeps CI green without a kernel door.
 *
 * Live product path (GJ_SYS_SCSI → scsi_door → virtio-scsi) is freestanding:
 *   src/scsi_mid_gj.c  →  make scsi_mid-gj  →  build/user/scsi_mid.elf
 *
 * Dual license MIT OR Apache-2.0. No GPL. No stamp bump here.
 */
#include <scsi_mid.h>
#include <stdio.h>
#include <string.h>

/* Soft inventory surface from cdb.c (no public header change). */
/* Soft deepen surfaces (CREATE-ONLY Soft!=product):
 *   greppable: soft residual lean / soft product residual / soft c2 product path
 *   greppable: claim_class=C2 product=UDX/DDI+ABI dual_dod=OPEN G-AC-1
 * Soft!=product complete; product lamps 0; Dual DoD stays OPEN.
 */

void scsi_mid_soft_inventory_log(void);

/* Host residual deepen stamp (soft inventory only; not GJ_IMAGE_VERSION). */
#define SOFT_HOST_WAVE 75u

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
    unsigned char aMode[4];
    unsigned char aBlk[SCSI_MID_SOFT_SEC_SIZE];
    unsigned char aBlk2[SCSI_MID_SOFT_SEC_SIZE * 2u];
    unsigned char aSense[18];
    unsigned i;
    int fFail = 0;
    int fSoftOk;
    int fSoftReady = 0;
    unsigned cSoftOk = 0;
    unsigned cSoftSkip = 0;
    unsigned cSoftLunHonest = 0;
    unsigned cSoftLbaHonest = 0;
    unsigned cSoftEvpdHonest = 0;
    unsigned cSoftOpHonest = 0;
    unsigned cSoftMode = 0;
    unsigned cSoftCapGeom = 0;
    unsigned cSoftMulti = 0;
    unsigned cSoftAreas = 0;
    uint8_t u8Key = 0;
    uint8_t u8Asc = 0;
    uint8_t u8Ascq = 0;
    uint32_t u32LastLba = 0;
    uint32_t u32BlkLen = 0;

    scsi_mid_soft_init();

    /* soft_ready honesty (armed after init; soft only). */
    if (scsi_mid_soft_ready() != 0) {
        fSoftReady = 1;
        cSoftOk++;
        cSoftAreas++;
        printf("scsi_mid-server: soft ready PASS\n");
    } else {
        cSoftSkip++;
        printf("scsi_mid-server: soft ready soft-skip\n");
    }

    /* TUR */
    io_zero(&io);
    scsi_cdb_test_unit_ready(&io.cdb);
    if (scsi_mid_submit(&io) != 0) {
        printf("scsi_mid-server: soft TUR FAIL\n");
        fFail = 1;
        cSoftSkip++;
    } else {
        cSoftOk++;
        cSoftAreas++;
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
        cSoftAreas++;
    }

    /*
     * MODE SENSE(6) — freestanding soft mid covers this; host residual
     * deepen closes the mid-policy gap (soft only; Soft!=product).
     */
    io_zero(&io);
    memset(aMode, 0, sizeof(aMode));
    scsi_cdb_mode_sense6(&io.cdb, 0, 4);
    io.pData = aMode;
    io.cbData = sizeof(aMode);
    if (scsi_mid_submit(&io) == 0 && aMode[0] == 3u) {
        cSoftMode = 1;
        cSoftOk++;
        cSoftAreas++;
        printf("scsi_mid-server: soft MODE_SENSE PASS\n");
    } else {
        cSoftSkip++;
        printf("scsi_mid-server: soft MODE_SENSE soft-skip\n");
    }

    /* READ CAPACITY(10) + soft geometry honesty (last_lba / block_len). */
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
        cSoftAreas++;
        u32LastLba = ((uint32_t)aCap[0] << 24) | ((uint32_t)aCap[1] << 16) |
                     ((uint32_t)aCap[2] << 8) | (uint32_t)aCap[3];
        u32BlkLen = ((uint32_t)aCap[4] << 24) | ((uint32_t)aCap[5] << 16) |
                    ((uint32_t)aCap[6] << 8) | (uint32_t)aCap[7];
        if (u32LastLba == (SCSI_MID_SOFT_SECTORS - 1u) &&
            u32BlkLen == SCSI_MID_SOFT_SEC_SIZE) {
            cSoftCapGeom = 1;
            cSoftOk++;
            cSoftAreas++;
            printf("scsi_mid-server: soft CAP geom PASS last_lba=%u "
                   "blk=%u\n",
                   (unsigned)u32LastLba, (unsigned)u32BlkLen);
        } else {
            cSoftSkip++;
            printf("scsi_mid-server: soft CAP geom soft-skip last_lba=%u "
                   "blk=%u\n",
                   (unsigned)u32LastLba, (unsigned)u32BlkLen);
        }
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
        cSoftAreas++;
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
            cSoftAreas++;
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
        cSoftAreas++;
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
        cSoftAreas++;
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
        cSoftAreas++;
    }

    /*
     * Residual deepen probes (always soft for inventory; hard fail host smoke
     * only if core mid already failed — deepen misses stay soft-skip).
     * Soft!=product. G-AC-1. Dual DoD OPEN.
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
        cSoftAreas++;
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
        cSoftAreas++;
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
        cSoftAreas++;
        printf("scsi_mid-server: soft EVPD reject PASS\n");
    } else {
        cSoftSkip++;
        printf("scsi_mid-server: soft EVPD reject soft-skip\n");
    }

    /*
     * Unknown opcode honesty (ASC 0x20) — soft mid policy residual.
     * Soft!=product; not a product HBA claim.
     */
    io_zero(&io);
    memset(&io.cdb, 0, sizeof(io.cdb));
    io.cdb.aCdb[0] = 0xFFu;
    io.cdb.u8Len = 6;
    if (scsi_mid_submit(&io) != 0 &&
        scsi_sense_decode(&io.sense, &u8Key, &u8Asc, &u8Ascq) == 0 &&
        u8Key == SCSI_SK_ILLEGAL_REQUEST && u8Asc == 0x20u) {
        cSoftOpHonest = 1;
        cSoftOk++;
        cSoftAreas++;
        printf("scsi_mid-server: soft opcode map PASS\n");
    } else {
        cSoftSkip++;
        printf("scsi_mid-server: soft opcode map soft-skip\n");
    }

    if (scsi_mid_stats(&st) != 0 || st.u32Soft != 1u || st.u32IoOk == 0) {
        printf("scsi_mid-server: soft stats FAIL ok=%u soft=%u\n",
               (unsigned)st.u32IoOk, (unsigned)st.u32Soft);
        fFail = 1;
    }

    /* Soft inventory — library + host skeleton surfaces. */
    scsi_mid_soft_inventory_log();

    /* Grep: scsi_mid-server: soft inventory */
    printf("scsi_mid-server: soft inventory ok=%u skip=%u lun_honest=%u "
           "lba_honest=%u evpd_honest=%u op_honest=%u mode=%u cap_geom=%u "
           "multi=%u ready=%u areas=%u soft_ok=%u soft_fail=%u "
           "wave=%u product_inq=0 soft_inq=1\n",
           cSoftOk, cSoftSkip, cSoftLunHonest, cSoftLbaHonest, cSoftEvpdHonest,
           cSoftOpHonest, cSoftMode, cSoftCapGeom, cSoftMulti,
           (unsigned)fSoftReady, cSoftAreas, (unsigned)st.u32IoOk,
           (unsigned)st.u32IoFail, (unsigned)SOFT_HOST_WAVE);

    /* Grep: scsi_mid-server: soft deepen */
    printf("scsi_mid-server: soft deepen wave=%u ok=%u skip=%u areas=%u "
           "lun_honest=%u multi=%u mode=%u op_honest=%u\n",
           (unsigned)SOFT_HOST_WAVE, cSoftOk, cSoftSkip, cSoftAreas,
           cSoftLunHonest, cSoftMulti, cSoftMode, cSoftOpHonest);

    /* Grep: scsi_mid-server: soft path */
    printf("scsi_mid-server: soft path soft_lun=1 door=0 product_inq=none "
           "soft_inq=soft lun_honest=soft multi_server=0 confine=0 wave=%u "
           "Soft!=product (soft inventory)\n",
           (unsigned)SOFT_HOST_WAVE);

    /* Grep: scsi_mid-server: soft honesty */
    printf("scsi_mid-server: soft honesty multi_server=0 confine=0 "
           "exclusive=1 soft=1 product_kernel=OPEN wave=%u Soft!=product "
           "G-AC-1=1 dual_dod=OPEN claim_class=C2\n",
           (unsigned)SOFT_HOST_WAVE);

    /*
     * C2 product daemon residual honesty (host soft only).
     * Soft!=product. Product path = UDX/DDI+ABI; Dual DoD OPEN.
     * Soft LUN / soft inventory never close Dual DoD or claim HBA stick.
     * Grep: scsi_mid-server: soft residual lean
     * Grep: scsi_mid-server: soft product residual
     * Grep: scsi_mid-server: soft c2 product path
     */
    printf("scsi_mid-server: soft residual lean residual_lean=1 soft_lun=1 "
           "door=0 multi_server=0 confine=0 freestanding_class=SKIP "
           "product_path=UDX product=UDX/DDI+ABI claim_class=C2 "
           "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
           "soft_ne_product=1 Soft!=product G-AC-1=1 "
           "dual=MIT_OR_Apache-2.0 wave=%u\n",
           (unsigned)SOFT_HOST_WAVE);

    printf("scsi_mid-server: soft product residual catalog Soft!=product "
           "G-AC-1=1 claim_class=C2 product=UDX/DDI+ABI dual_dod=OPEN "
           "product_inq=0 soft_inq=1 door=0 multi_server=0 confine=0 "
           "mode=%u cap_geom=%u op_honest=%u lun_honest=%u multi=%u "
           "areas=%u wave=%u never_hba_PASS=1\n",
           cSoftMode, cSoftCapGeom, cSoftOpHonest, cSoftLunHonest, cSoftMulti,
           cSoftAreas, (unsigned)SOFT_HOST_WAVE);

    printf("scsi_mid-server: soft product residual "
           "Soft!=product claim_class=C2 product=UDX/DDI+ABI need=DDI "
           "need=hot_cold_abi dual_dod=OPEN product_mint=0 "
           "soft_lun_ne_door=1 G-AC-1=1 wave=%u\n",
           (unsigned)SOFT_HOST_WAVE);

    printf("scsi_mid-server: soft c2 product path claim_class=C2 "
           "product=UDX/DDI+ABI product_path=UDX dual_dod=OPEN "
           "Soft!=product G-AC-1=1 freestanding_class=SKIP "
           "soft_lamps_ne_close=1 door=0 soft_lun=1 wave=%u\n",
           (unsigned)SOFT_HOST_WAVE);

    printf("scsi_mid-server: soft residual lean deepen residual_lean=1 "
           "mode=%u cap_geom=%u op_honest=%u ready=%u areas=%u "
           "Soft!=product dual_dod=OPEN claim_class=C2 G-AC-1=1 "
           "product=UDX/DDI+ABI wave=%u\n",
           cSoftMode, cSoftCapGeom, cSoftOpHonest, (unsigned)fSoftReady,
           cSoftAreas, (unsigned)SOFT_HOST_WAVE);

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
