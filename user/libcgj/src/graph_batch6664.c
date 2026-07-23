/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6664: soft io_uring IORING_OP_FSYNC opcode.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioring_op_fsync_6664(void);
 *     - Returns 3 (IORING_OP_FSYNC soft catalog value). Pure-data opcode
 *       stub; does not submit SQEs or call io_uring syscalls.
 *   uint32_t __gj_ioring_op_fsync_6664  (alias)
 *   __libcgj_batch6664_marker = "libcgj-batch6664"
 *
 * Exclusive continuum CREATE-ONLY (6661-6670: io_uring opcode stubs).
 * Unique gj_ioring_op_fsync_6664 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6664_marker[] = "libcgj-batch6664";

/* Soft IORING_OP_FSYNC catalog value (enum ordinal 3). */
#define B6664_OP_FSYNC  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6664_op_fsync(void)
{
	return B6664_OP_FSYNC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioring_op_fsync_6664 - soft IORING_OP_FSYNC opcode catalog value.
 *
 * Always returns 3. Soft continuum constant; does not fsync anything.
 * No parent wires.
 */
uint32_t
gj_ioring_op_fsync_6664(void)
{
	(void)NULL;
	return b6664_op_fsync();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioring_op_fsync_6664(void)
    __attribute__((alias("gj_ioring_op_fsync_6664")));
