/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6667: soft io_uring IORING_OP_TIMEOUT opcode.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioring_op_timeout_6667(void);
 *     - Returns 11 (IORING_OP_TIMEOUT soft catalog value). Pure-data
 *       opcode stub; does not arm timeouts or submit SQEs.
 *   uint32_t __gj_ioring_op_timeout_6667  (alias)
 *   __libcgj_batch6667_marker = "libcgj-batch6667"
 *
 * Exclusive continuum CREATE-ONLY (6661-6670: io_uring opcode stubs).
 * Unique gj_ioring_op_timeout_6667 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6667_marker[] = "libcgj-batch6667";

/* Soft IORING_OP_TIMEOUT catalog value (enum ordinal 11). */
#define B6667_OP_TIMEOUT  11u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6667_op_timeout(void)
{
	return B6667_OP_TIMEOUT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioring_op_timeout_6667 - soft IORING_OP_TIMEOUT opcode catalog value.
 *
 * Always returns 11. Soft continuum constant; does not arm timers.
 * No parent wires.
 */
uint32_t
gj_ioring_op_timeout_6667(void)
{
	(void)NULL;
	return b6667_op_timeout();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioring_op_timeout_6667(void)
    __attribute__((alias("gj_ioring_op_timeout_6667")));
