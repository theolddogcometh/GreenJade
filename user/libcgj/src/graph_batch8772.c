/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8772: soft io_uring IORING_OP_READV opcode id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioring_read_id_8772(void);
 *     - Returns 1 (IORING_OP_READV soft catalog value). Pure-data opcode
 *       id stub; does not submit SQEs or call io_uring syscalls.
 *   uint32_t __gj_ioring_read_id_8772  (alias)
 *   __libcgj_batch8772_marker = "libcgj-batch8772"
 *
 * Exclusive continuum CREATE-ONLY (8771-8780: io_uring opcode id stubs).
 * Unique gj_ioring_read_id_8772 surface only; no multi-def.
 * Distinct from write (batch8773). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8772_marker[] = "libcgj-batch8772";

/* Soft IORING_OP_READV catalog value (enum ordinal 1). */
#define B8772_OP_READ  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8772_read_id(void)
{
	return B8772_OP_READ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioring_read_id_8772 - soft IORING_OP_READV opcode catalog id.
 *
 * Always returns 1. Soft continuum constant; does not issue I/O.
 * No parent wires.
 */
uint32_t
gj_ioring_read_id_8772(void)
{
	(void)NULL;
	return b8772_read_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioring_read_id_8772(void)
    __attribute__((alias("gj_ioring_read_id_8772")));
