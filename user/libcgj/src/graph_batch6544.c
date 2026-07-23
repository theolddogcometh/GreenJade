/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6544: AHCI DET device-present predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_det_present_u_6544(uint32_t det);
 *     - Return 1 if DET indicates device present with communication
 *       established (DET == 3), else 0.
 *   uint32_t __gj_ahci_det_present_u_6544  (alias)
 *   __libcgj_batch6544_marker = "libcgj-batch6544"
 *
 * Exclusive continuum CREATE-ONLY (6541-6550: AHCI port status stubs —
 * pxssts_det, pxssts_spd, pxssts_ipm, det_present, spd_gen, pxtfd_bsy,
 * pxtfd_drq, pxcmd_st, port_idle, batch_id_6550).
 * Unique gj_ahci_det_present_u_6544 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6544_marker[] = "libcgj-batch6544";

/* PxSSTS DET: device present and communication established. */
#define B6544_DET_ESTABLISHED ((uint32_t)0x3u)
#define B6544_DET_MASK        ((uint32_t)0xFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6544_det_present(uint32_t u32Det)
{
	return ((u32Det & B6544_DET_MASK) == B6544_DET_ESTABLISHED)
	    ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_det_present_u_6544 - 1 if DET means device+comm established.
 *
 * det: DET field (or full register; only bits 3:0 consulted)
 *
 * Returns 1 when DET == 3, else 0. Self-contained; does not call
 * gj_ahci_pxssts_det_u_6541. No parent wires.
 */
uint32_t
gj_ahci_det_present_u_6544(uint32_t u32Det)
{
	(void)NULL;
	return b6544_det_present(u32Det);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_det_present_u_6544(uint32_t u32Det)
    __attribute__((alias("gj_ahci_det_present_u_6544")));
