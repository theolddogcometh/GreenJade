/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6547: AHCI PxTFD DRQ bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_pxtfd_drq_u_6547(uint32_t pxtfd);
 *     - Return 1 if PxTFD status DRQ bit is set, else 0.
 *   uint32_t __gj_ahci_pxtfd_drq_u_6547  (alias)
 *   __libcgj_batch6547_marker = "libcgj-batch6547"
 *
 * Exclusive continuum CREATE-ONLY (6541-6550: AHCI port status stubs —
 * pxssts_det, pxssts_spd, pxssts_ipm, det_present, spd_gen, pxtfd_bsy,
 * pxtfd_drq, pxcmd_st, port_idle, batch_id_6550).
 * Unique gj_ahci_pxtfd_drq_u_6547 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6547_marker[] = "libcgj-batch6547";

/* ATA status register DRQ bit (bit 3 of low byte of PxTFD). */
#define B6547_TFD_DRQ ((uint32_t)0x08u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6547_pxtfd_drq(uint32_t u32PxTfd)
{
	return (u32PxTfd & B6547_TFD_DRQ) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_pxtfd_drq_u_6547 - 1 if AHCI PxTFD status DRQ is set.
 *
 * pxtfd: raw Port Task File Data register value
 *
 * Returns 1 when status bit 3 (DRQ) is set, else 0. Pure mask test.
 * No parent wires.
 */
uint32_t
gj_ahci_pxtfd_drq_u_6547(uint32_t u32PxTfd)
{
	(void)NULL;
	return b6547_pxtfd_drq(u32PxTfd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_pxtfd_drq_u_6547(uint32_t u32PxTfd)
    __attribute__((alias("gj_ahci_pxtfd_drq_u_6547")));
