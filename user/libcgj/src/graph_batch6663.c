/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6663: soft io_uring IORING_OP_WRITEV opcode.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioring_op_writev_6663(void);
 *     - Returns 2 (IORING_OP_WRITEV soft catalog value). Pure-data opcode
 *       stub; does not submit SQEs or call io_uring syscalls.
 *   uint32_t __gj_ioring_op_writev_6663  (alias)
 *   __libcgj_batch6663_marker = "libcgj-batch6663"
 *
 * Exclusive continuum CREATE-ONLY (6661-6670: io_uring opcode stubs).
 * Unique gj_ioring_op_writev_6663 surface only; no multi-def.
 * Distinct from readv (batch6662). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6663_marker[] = "libcgj-batch6663";

/* Soft IORING_OP_WRITEV catalog value (enum ordinal 2). */
#define B6663_OP_WRITEV  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6663_op_writev(void)
{
	return B6663_OP_WRITEV;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioring_op_writev_6663 - soft IORING_OP_WRITEV opcode catalog value.
 *
 * Always returns 2. Soft continuum constant; does not issue I/O.
 * No parent wires.
 */
uint32_t
gj_ioring_op_writev_6663(void)
{
	(void)NULL;
	return b6663_op_writev();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioring_op_writev_6663(void)
    __attribute__((alias("gj_ioring_op_writev_6663")));
