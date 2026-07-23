/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6662: soft io_uring IORING_OP_READV opcode.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioring_op_readv_6662(void);
 *     - Returns 1 (IORING_OP_READV soft catalog value). Pure-data opcode
 *       stub; does not submit SQEs or call io_uring syscalls.
 *   uint32_t __gj_ioring_op_readv_6662  (alias)
 *   __libcgj_batch6662_marker = "libcgj-batch6662"
 *
 * Exclusive continuum CREATE-ONLY (6661-6670: io_uring opcode stubs).
 * Unique gj_ioring_op_readv_6662 surface only; no multi-def.
 * Distinct from writev (batch6663). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6662_marker[] = "libcgj-batch6662";

/* Soft IORING_OP_READV catalog value (enum ordinal 1). */
#define B6662_OP_READV  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6662_op_readv(void)
{
	return B6662_OP_READV;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioring_op_readv_6662 - soft IORING_OP_READV opcode catalog value.
 *
 * Always returns 1. Soft continuum constant; does not issue I/O.
 * No parent wires.
 */
uint32_t
gj_ioring_op_readv_6662(void)
{
	(void)NULL;
	return b6662_op_readv();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioring_op_readv_6662(void)
    __attribute__((alias("gj_ioring_op_readv_6662")));
