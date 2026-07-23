/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8773: soft io_uring IORING_OP_WRITEV opcode id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioring_write_id_8773(void);
 *     - Returns 2 (IORING_OP_WRITEV soft catalog value). Pure-data opcode
 *       id stub; does not submit SQEs or call io_uring syscalls.
 *   uint32_t __gj_ioring_write_id_8773  (alias)
 *   __libcgj_batch8773_marker = "libcgj-batch8773"
 *
 * Exclusive continuum CREATE-ONLY (8771-8780: io_uring opcode id stubs).
 * Unique gj_ioring_write_id_8773 surface only; no multi-def.
 * Distinct from read (batch8772). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8773_marker[] = "libcgj-batch8773";

/* Soft IORING_OP_WRITEV catalog value (enum ordinal 2). */
#define B8773_OP_WRITE  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8773_write_id(void)
{
	return B8773_OP_WRITE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioring_write_id_8773 - soft IORING_OP_WRITEV opcode catalog id.
 *
 * Always returns 2. Soft continuum constant; does not issue I/O.
 * No parent wires.
 */
uint32_t
gj_ioring_write_id_8773(void)
{
	(void)NULL;
	return b8773_write_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioring_write_id_8773(void)
    __attribute__((alias("gj_ioring_write_id_8773")));
