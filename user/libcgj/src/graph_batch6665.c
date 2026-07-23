/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6665: soft io_uring opcode catalog range check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioring_op_ok_6665(uint32_t op);
 *     - Returns 1 when op is in soft catalog range [0, 0x3F], else 0.
 *       Pure-data range stub; not a live IORING_OP_LAST compare.
 *   uint32_t __gj_ioring_op_ok_6665  (alias)
 *   __libcgj_batch6665_marker = "libcgj-batch6665"
 *
 * Exclusive continuum CREATE-ONLY (6661-6670: io_uring opcode stubs).
 * Unique gj_ioring_op_ok_6665 surface only; no multi-def.
 * Soft bound is a product catalog ceiling, not kernel last-op.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6665_marker[] = "libcgj-batch6665";

/* Soft opcode catalog ceiling (inclusive). Product bound only. */
#define B6665_OP_MAX  0x3Fu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6665_op_ok(uint32_t uOp)
{
	return (uOp <= B6665_OP_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioring_op_ok_6665 - soft io_uring opcode catalog membership.
 *
 * op: candidate SQE opcode value.
 * Returns 1 if op is in [0, 0x3F], else 0. Does not touch rings.
 * No parent wires.
 */
uint32_t
gj_ioring_op_ok_6665(uint32_t uOp)
{
	(void)NULL;
	return b6665_op_ok(uOp);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioring_op_ok_6665(uint32_t uOp)
    __attribute__((alias("gj_ioring_op_ok_6665")));
