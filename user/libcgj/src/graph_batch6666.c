/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6666: soft io_uring SQE opcode 8-bit pack.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ioring_sqe_opcode_pack_6666(uint32_t op);
 *     - Returns op masked to 8 bits (soft SQE opcode lane). Pure-integer
 *       pack; does not write a real sqe->opcode field.
 *   uint32_t __gj_ioring_sqe_opcode_pack_6666  (alias)
 *   __libcgj_batch6666_marker = "libcgj-batch6666"
 *
 * Exclusive continuum CREATE-ONLY (6661-6670: io_uring opcode stubs).
 * Unique gj_ioring_sqe_opcode_pack_6666 surface only; no multi-def.
 * Distinct from range ok (batch6665). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6666_marker[] = "libcgj-batch6666";

/* Soft SQE opcode lane width (io_uring_sqe.opcode is __u8). */
#define B6666_OPCODE_MASK  0xFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6666_pack(uint32_t uOp)
{
	return (uOp & B6666_OPCODE_MASK);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ioring_sqe_opcode_pack_6666 - soft 8-bit SQE opcode lane pack.
 *
 * op: candidate opcode word.
 * Returns op & 0xFF. Soft pure-integer pack only; no ring touch.
 * No parent wires.
 */
uint32_t
gj_ioring_sqe_opcode_pack_6666(uint32_t uOp)
{
	(void)NULL;
	return b6666_pack(uOp);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ioring_sqe_opcode_pack_6666(uint32_t uOp)
    __attribute__((alias("gj_ioring_sqe_opcode_pack_6666")));
