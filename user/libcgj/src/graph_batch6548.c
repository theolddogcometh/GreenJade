/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6548: AHCI PxCMD ST bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_pxcmd_st_u_6548(uint32_t pxcmd);
 *     - Return 1 if PxCMD ST (start) bit is set, else 0.
 *   uint32_t __gj_ahci_pxcmd_st_u_6548  (alias)
 *   __libcgj_batch6548_marker = "libcgj-batch6548"
 *
 * Exclusive continuum CREATE-ONLY (6541-6550: AHCI port status stubs —
 * pxssts_det, pxssts_spd, pxssts_ipm, det_present, spd_gen, pxtfd_bsy,
 * pxtfd_drq, pxcmd_st, port_idle, batch_id_6550).
 * Unique gj_ahci_pxcmd_st_u_6548 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6548_marker[] = "libcgj-batch6548";

/* PxCMD ST — start (bit 0). */
#define B6548_CMD_ST ((uint32_t)0x1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6548_pxcmd_st(uint32_t u32PxCmd)
{
	return (u32PxCmd & B6548_CMD_ST) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_pxcmd_st_u_6548 - 1 if AHCI PxCMD ST (start) is set.
 *
 * pxcmd: raw Port Command and Status register value
 *
 * Returns 1 when bit 0 (ST) is set, else 0. Pure mask test.
 * No parent wires.
 */
uint32_t
gj_ahci_pxcmd_st_u_6548(uint32_t u32PxCmd)
{
	(void)NULL;
	return b6548_pxcmd_st(u32PxCmd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_pxcmd_st_u_6548(uint32_t u32PxCmd)
    __attribute__((alias("gj_ahci_pxcmd_st_u_6548")));
