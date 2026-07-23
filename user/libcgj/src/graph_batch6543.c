/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6543: AHCI PxSSTS IPM field extract stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_pxssts_ipm_u_6543(uint32_t pxssts);
 *     - Return PxSSTS IPM interface power-management field (bits 11:8).
 *   uint32_t __gj_ahci_pxssts_ipm_u_6543  (alias)
 *   __libcgj_batch6543_marker = "libcgj-batch6543"
 *
 * Exclusive continuum CREATE-ONLY (6541-6550: AHCI port status stubs —
 * pxssts_det, pxssts_spd, pxssts_ipm, det_present, spd_gen, pxtfd_bsy,
 * pxtfd_drq, pxcmd_st, port_idle, batch_id_6550).
 * Unique gj_ahci_pxssts_ipm_u_6543 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6543_marker[] = "libcgj-batch6543";

/* PxSSTS IPM field: bits 11:8. */
#define B6543_IPM_SHIFT ((uint32_t)8u)
#define B6543_IPM_MASK  ((uint32_t)0xFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6543_pxssts_ipm(uint32_t u32PxSsts)
{
	return (u32PxSsts >> B6543_IPM_SHIFT) & B6543_IPM_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_pxssts_ipm_u_6543 - extract AHCI PxSSTS IPM field.
 *
 * pxssts: raw Port Serial ATA Status register value
 *
 * Returns IPM (0=none, 1=active, 2=partial, 6=slumber, 8=DevSleep).
 * Pure shift+mask; no parent wires.
 */
uint32_t
gj_ahci_pxssts_ipm_u_6543(uint32_t u32PxSsts)
{
	(void)NULL;
	return b6543_pxssts_ipm(u32PxSsts);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_pxssts_ipm_u_6543(uint32_t u32PxSsts)
    __attribute__((alias("gj_ahci_pxssts_ipm_u_6543")));
