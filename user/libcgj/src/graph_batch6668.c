/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6668: soft io_uring IORING_OP_ACCEPT opcode.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioring_op_accept_6668(void);
 *     - Returns 13 (IORING_OP_ACCEPT soft catalog value). Pure-data
 *       opcode stub; does not accept sockets or submit SQEs.
 *   uint32_t __gj_ioring_op_accept_6668  (alias)
 *   __libcgj_batch6668_marker = "libcgj-batch6668"
 *
 * Exclusive continuum CREATE-ONLY (6661-6670: io_uring opcode stubs).
 * Unique gj_ioring_op_accept_6668 surface only; no multi-def.
 * Distinct from timeout (batch6667). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6668_marker[] = "libcgj-batch6668";

/* Soft IORING_OP_ACCEPT catalog value (enum ordinal 13). */
#define B6668_OP_ACCEPT  13u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6668_op_accept(void)
{
	return B6668_OP_ACCEPT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioring_op_accept_6668 - soft IORING_OP_ACCEPT opcode catalog value.
 *
 * Always returns 13. Soft continuum constant; does not accept fds.
 * No parent wires.
 */
uint32_t
gj_ioring_op_accept_6668(void)
{
	(void)NULL;
	return b6668_op_accept();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioring_op_accept_6668(void)
    __attribute__((alias("gj_ioring_op_accept_6668")));
