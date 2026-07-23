/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8774: soft io_uring IORING_OP_FSYNC opcode id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioring_fsync_id_8774(void);
 *     - Returns 3 (IORING_OP_FSYNC soft catalog value). Pure-data opcode
 *       id stub; does not submit SQEs or call io_uring syscalls.
 *   uint32_t __gj_ioring_fsync_id_8774  (alias)
 *   __libcgj_batch8774_marker = "libcgj-batch8774"
 *
 * Exclusive continuum CREATE-ONLY (8771-8780: io_uring opcode id stubs).
 * Unique gj_ioring_fsync_id_8774 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8774_marker[] = "libcgj-batch8774";

/* Soft IORING_OP_FSYNC catalog value (enum ordinal 3). */
#define B8774_OP_FSYNC  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8774_fsync_id(void)
{
	return B8774_OP_FSYNC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioring_fsync_id_8774 - soft IORING_OP_FSYNC opcode catalog id.
 *
 * Always returns 3. Soft continuum constant; does not fsync anything.
 * No parent wires.
 */
uint32_t
gj_ioring_fsync_id_8774(void)
{
	(void)NULL;
	return b8774_fsync_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioring_fsync_id_8774(void)
    __attribute__((alias("gj_ioring_fsync_id_8774")));
