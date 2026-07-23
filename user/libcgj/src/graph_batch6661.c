/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6661: soft io_uring IORING_OP_NOP opcode.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioring_op_nop_6661(void);
 *     - Returns 0 (IORING_OP_NOP soft catalog value). Pure-data opcode
 *       stub; does not submit SQEs or call io_uring syscalls.
 *   uint32_t __gj_ioring_op_nop_6661  (alias)
 *   __libcgj_batch6661_marker = "libcgj-batch6661"
 *
 * Exclusive continuum CREATE-ONLY (6661-6670: io_uring opcode stubs —
 * ioring_op_nop_6661, ioring_op_readv_6662, ioring_op_writev_6663,
 * ioring_op_fsync_6664, ioring_op_ok_6665, ioring_sqe_opcode_pack_6666,
 * ioring_op_timeout_6667, ioring_op_accept_6668,
 * ioring_op_stub_ready_6669, continuum + batch_id_6670).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6661_marker[] = "libcgj-batch6661";

/* Soft IORING_OP_NOP catalog value (linux/io_uring.h enum ordinal 0). */
#define B6661_OP_NOP  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6661_op_nop(void)
{
	return B6661_OP_NOP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioring_op_nop_6661 - soft IORING_OP_NOP opcode catalog value.
 *
 * Always returns 0. Soft compile-time opcode id; does not touch rings.
 * No parent wires.
 */
uint32_t
gj_ioring_op_nop_6661(void)
{
	(void)NULL;
	return b6661_op_nop();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioring_op_nop_6661(void)
    __attribute__((alias("gj_ioring_op_nop_6661")));
