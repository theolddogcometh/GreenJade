/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8771: soft io_uring IORING_OP_NOP opcode id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioring_nop_id_8771(void);
 *     - Returns 0 (IORING_OP_NOP soft catalog value). Pure-data opcode
 *       id stub; does not submit SQEs or call io_uring syscalls.
 *   uint32_t __gj_ioring_nop_id_8771  (alias)
 *   __libcgj_batch8771_marker = "libcgj-batch8771"
 *
 * Exclusive continuum CREATE-ONLY (8771-8780: io_uring opcode id stubs —
 * ioring_nop_id_8771, ioring_read_id_8772, ioring_write_id_8773,
 * ioring_fsync_id_8774, ioring_poll_add_id_8775, ioring_poll_remove_id_8776,
 * ioring_accept_id_8777, ioring_connect_id_8778, ioring_ready_u_8779,
 * batch_id_8780).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8771_marker[] = "libcgj-batch8771";

/* Soft IORING_OP_NOP catalog value (linux/io_uring.h enum ordinal 0). */
#define B8771_OP_NOP  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8771_nop_id(void)
{
	return B8771_OP_NOP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioring_nop_id_8771 - soft IORING_OP_NOP opcode catalog id.
 *
 * Always returns 0. Soft compile-time opcode id; does not touch rings.
 * No parent wires.
 */
uint32_t
gj_ioring_nop_id_8771(void)
{
	(void)NULL;
	return b8771_nop_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioring_nop_id_8771(void)
    __attribute__((alias("gj_ioring_nop_id_8771")));
