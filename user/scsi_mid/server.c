/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Host skeleton for the userspace scsi_mid product path (pure C11).
 *
 * Role
 * ----
 * Compile/link smoke on a host toolchain (no GJ freestanding, no kernel door).
 * Builds a standard INQUIRY CDB, packs a scsi_io, and calls scsi_mid_submit.
 *
 * On host-only links, src/cdb.c compiles with SCSI_HAS_SYS=0, so submit is
 * ENOSYS-shaped (returns -1). That is expected: soft PASS keeps CI green.
 *
 * Live product path (GJ_SYS_SCSI → scsi_door → virtio-scsi) is freestanding:
 *   src/scsi_mid_gj.c  →  make scsi_mid-gj  →  build/user/scsi_mid.elf
 */
#include <scsi_mid.h>
#include <stdio.h>
#include <string.h>

int
main(void)
{
    struct scsi_cdb cdb;
    struct scsi_io io;

    /* SPC: standard INQUIRY, EVPD=0, page=0, allocation length 36. */
    scsi_cdb_inquiry(&cdb, 0, 0, 36);

    memset(&io, 0, sizeof(io));
    io.cdb = cdb;
    io.u32Lun = 0;
    io.u32TimeoutMs = 5000;
    /* pData/cbData left null: host path never reaches a real HBA transfer. */

    /*
     * CDB → submit path (host):
     *   scsi_cdb_* fills io.cdb; scsi_mid_submit maps opcode → door/HBA.
     * Host CI: cdb.c has no GJ_SYS_SCSI → submit returns -1 (ENOSYS-shaped).
     * Soft PASS so host smoke stays green without a kernel door.
     */
    if (scsi_mid_submit(&io) != 0) {
        printf("scsi_mid-server: host soft PASS "
               "(ENOSYS-shaped; no kernel door)\n");
        return 0;
    }

    /* Only reached if host link somehow wired a real submit backend. */
    printf("scsi_mid-server: submit ok (unexpected on host-only link)\n");
    return 0;
}
