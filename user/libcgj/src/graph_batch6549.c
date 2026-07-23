/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6549: AHCI port idle predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ahci_port_idle_u_6549(uint32_t pxcmd);
 *     - Return 1 if port looks idle: ST clear and neither CR nor FR
 *       running (bits 15 and 14 clear). Else 0.
 *   uint32_t __gj_ahci_port_idle_u_6549  (alias)
 *   __libcgj_batch6549_marker = "libcgj-batch6549"
 *
 * Exclusive continuum CREATE-ONLY (6541-6550: AHCI port status stubs —
 * pxssts_det, pxssts_spd, pxssts_ipm, det_present, spd_gen, pxtfd_bsy,
 * pxtfd_drq, pxcmd_st, port_idle, batch_id_6550).
 * Unique gj_ahci_port_idle_u_6549 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6549_marker[] = "libcgj-batch6549";

/* PxCMD ST (bit 0), FR (bit 14), CR (bit 15). */
#define B6549_CMD_ST ((uint32_t)0x1u)
#define B6549_CMD_FR ((uint32_t)(1u << 14))
#define B6549_CMD_CR ((uint32_t)(1u << 15))
#define B6549_BUSY_MASK (B6549_CMD_ST | B6549_CMD_FR | B6549_CMD_CR)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6549_port_idle(uint32_t u32PxCmd)
{
	return ((u32PxCmd & B6549_BUSY_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ahci_port_idle_u_6549 - 1 if AHCI port ST/FR/CR all clear.
 *
 * pxcmd: raw Port Command and Status register value
 *
 * Returns 1 when ST, FR, and CR are all clear (idle-looking port).
 * Self-contained; does not call gj_ahci_pxcmd_st_u_6548. No parent wires.
 */
uint32_t
gj_ahci_port_idle_u_6549(uint32_t u32PxCmd)
{
	(void)NULL;
	return b6549_port_idle(u32PxCmd);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ahci_port_idle_u_6549(uint32_t u32PxCmd)
    __attribute__((alias("gj_ahci_port_idle_u_6549")));
