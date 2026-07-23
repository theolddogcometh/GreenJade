/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6545: AHCI SPD generation number stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_spd_gen_u_6545(uint32_t spd);
 *     - Map PxSSTS SPD code to SATA generation number (1/2/3), or 0
 *       if not negotiated / unknown.
 *   uint32_t __gj_ahci_spd_gen_u_6545  (alias)
 *   __libcgj_batch6545_marker = "libcgj-batch6545"
 *
 * Exclusive continuum CREATE-ONLY (6541-6550: AHCI port status stubs —
 * pxssts_det, pxssts_spd, pxssts_ipm, det_present, spd_gen, pxtfd_bsy,
 * pxtfd_drq, pxcmd_st, port_idle, batch_id_6550).
 * Unique gj_ahci_spd_gen_u_6545 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6545_marker[] = "libcgj-batch6545";

#define B6545_SPD_MASK ((uint32_t)0xFu)
#define B6545_SPD_GEN1 ((uint32_t)1u)
#define B6545_SPD_GEN2 ((uint32_t)2u)
#define B6545_SPD_GEN3 ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6545_spd_gen(uint32_t u32Spd)
{
	uint32_t u32Code = u32Spd & B6545_SPD_MASK;

	if (u32Code == B6545_SPD_GEN1)
		return 1u;
	if (u32Code == B6545_SPD_GEN2)
		return 2u;
	if (u32Code == B6545_SPD_GEN3)
		return 3u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_spd_gen_u_6545 - map AHCI PxSSTS SPD to SATA gen number.
 *
 * spd: SPD field (or larger value; only bits 3:0 consulted)
 *
 * Returns 1/2/3 for Gen1/2/3, else 0. Self-contained; does not call
 * gj_ahci_pxssts_spd_u_6542. No parent wires.
 */
uint32_t
gj_ahci_spd_gen_u_6545(uint32_t u32Spd)
{
	(void)NULL;
	return b6545_spd_gen(u32Spd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_spd_gen_u_6545(uint32_t u32Spd)
    __attribute__((alias("gj_ahci_spd_gen_u_6545")));
