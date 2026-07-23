/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6542: AHCI PxSSTS SPD field extract stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_pxssts_spd_u_6542(uint32_t pxssts);
 *     - Return PxSSTS SPD interface-speed field (bits 7:4).
 *   uint32_t __gj_ahci_pxssts_spd_u_6542  (alias)
 *   __libcgj_batch6542_marker = "libcgj-batch6542"
 *
 * Exclusive continuum CREATE-ONLY (6541-6550: AHCI port status stubs —
 * pxssts_det, pxssts_spd, pxssts_ipm, det_present, spd_gen, pxtfd_bsy,
 * pxtfd_drq, pxcmd_st, port_idle, batch_id_6550).
 * Unique gj_ahci_pxssts_spd_u_6542 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6542_marker[] = "libcgj-batch6542";

/* PxSSTS SPD field: bits 7:4. */
#define B6542_SPD_SHIFT ((uint32_t)4u)
#define B6542_SPD_MASK  ((uint32_t)0xFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6542_pxssts_spd(uint32_t u32PxSsts)
{
	return (u32PxSsts >> B6542_SPD_SHIFT) & B6542_SPD_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_pxssts_spd_u_6542 - extract AHCI PxSSTS SPD field.
 *
 * pxssts: raw Port Serial ATA Status register value
 *
 * Returns SPD (0=none, 1=Gen1 1.5G, 2=Gen2 3.0G, 3=Gen3 6.0G).
 * Pure shift+mask; no parent wires.
 */
uint32_t
gj_ahci_pxssts_spd_u_6542(uint32_t u32PxSsts)
{
	(void)NULL;
	return b6542_pxssts_spd(u32PxSsts);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_pxssts_spd_u_6542(uint32_t u32PxSsts)
    __attribute__((alias("gj_ahci_pxssts_spd_u_6542")));
