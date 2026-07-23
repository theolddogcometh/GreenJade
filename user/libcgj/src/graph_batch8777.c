/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8777: soft io_uring IORING_OP_ACCEPT opcode id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioring_accept_id_8777(void);
 *     - Returns 13 (IORING_OP_ACCEPT soft catalog value). Pure-data
 *       opcode id stub; does not accept sockets or submit SQEs.
 *   uint32_t __gj_ioring_accept_id_8777  (alias)
 *   __libcgj_batch8777_marker = "libcgj-batch8777"
 *
 * Exclusive continuum CREATE-ONLY (8771-8780: io_uring opcode id stubs).
 * Unique gj_ioring_accept_id_8777 surface only; no multi-def.
 * Distinct from connect (batch8778). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8777_marker[] = "libcgj-batch8777";

/* Soft IORING_OP_ACCEPT catalog value (enum ordinal 13). */
#define B8777_OP_ACCEPT  13u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8777_accept_id(void)
{
	return B8777_OP_ACCEPT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioring_accept_id_8777 - soft IORING_OP_ACCEPT opcode catalog id.
 *
 * Always returns 13. Soft continuum constant; does not accept fds.
 * No parent wires.
 */
uint32_t
gj_ioring_accept_id_8777(void)
{
	(void)NULL;
	return b8777_accept_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioring_accept_id_8777(void)
    __attribute__((alias("gj_ioring_accept_id_8777")));
