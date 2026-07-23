/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6541: AHCI PxSSTS DET field extract stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_pxssts_det_u_6541(uint32_t pxssts);
 *     - Return PxSSTS DET device-detection field (bits 3:0).
 *   uint32_t __gj_ahci_pxssts_det_u_6541  (alias)
 *   __libcgj_batch6541_marker = "libcgj-batch6541"
 *
 * Exclusive continuum CREATE-ONLY (6541-6550: AHCI port status stubs —
 * pxssts_det, pxssts_spd, pxssts_ipm, det_present, spd_gen, pxtfd_bsy,
 * pxtfd_drq, pxcmd_st, port_idle, batch_id_6550).
 * Unique gj_ahci_pxssts_det_u_6541 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6541_marker[] = "libcgj-batch6541";

/* PxSSTS DET field: bits 3:0. */
#define B6541_DET_MASK  ((uint32_t)0xFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6541_pxssts_det(uint32_t u32PxSsts)
{
	return u32PxSsts & B6541_DET_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_pxssts_det_u_6541 - extract AHCI PxSSTS DET field.
 *
 * pxssts: raw Port Serial ATA Status register value
 *
 * Returns DET in bits 3:0 (0=none, 1=present no-comm, 3=present+comm,
 * 4=PHY offline). Pure mask; no parent wires.
 */
uint32_t
gj_ahci_pxssts_det_u_6541(uint32_t u32PxSsts)
{
	(void)NULL;
	return b6541_pxssts_det(u32PxSsts);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_pxssts_det_u_6541(uint32_t u32PxSsts)
    __attribute__((alias("gj_ahci_pxssts_det_u_6541")));
