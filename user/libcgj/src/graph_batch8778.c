/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8778: soft io_uring IORING_OP_CONNECT opcode id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioring_connect_id_8778(void);
 *     - Returns 16 (IORING_OP_CONNECT soft catalog value). Pure-data
 *       opcode id stub; does not connect sockets or submit SQEs.
 *   uint32_t __gj_ioring_connect_id_8778  (alias)
 *   __libcgj_batch8778_marker = "libcgj-batch8778"
 *
 * Exclusive continuum CREATE-ONLY (8771-8780: io_uring opcode id stubs).
 * Unique gj_ioring_connect_id_8778 surface only; no multi-def.
 * Distinct from accept (batch8777). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8778_marker[] = "libcgj-batch8778";

/* Soft IORING_OP_CONNECT catalog value (enum ordinal 16). */
#define B8778_OP_CONNECT  16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8778_connect_id(void)
{
	return B8778_OP_CONNECT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioring_connect_id_8778 - soft IORING_OP_CONNECT opcode catalog id.
 *
 * Always returns 16. Soft continuum constant; does not connect fds.
 * No parent wires.
 */
uint32_t
gj_ioring_connect_id_8778(void)
{
	(void)NULL;
	return b8778_connect_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioring_connect_id_8778(void)
    __attribute__((alias("gj_ioring_connect_id_8778")));
