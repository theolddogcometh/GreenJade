/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4665: bitwise NAND of two uint32_t (_u2).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_nand_u2(uint32_t a, uint32_t b);
 *     - Return ~(a & b). Rename on collision with gj_u32_nand_u
 *       (batch3218); same pure integer NAND semantics, unique symbol.
 *   uint32_t __gj_u32_nand_u2  (alias)
 *   __libcgj_batch4665_marker = "libcgj-batch4665"
 *
 * Exclusive continuum CREATE-ONLY (4661-4670: u32_andn_u2, u64_andn_u2,
 * u32_orn_u, u32_xnor_u, u32_nand_u2, u32_nor_u, u32_majority_u,
 * u32_mux_u, u32_implies_u, batch_id_4670). Distinct from
 * gj_u32_nand_u (batch3218) — rename on collision; unique
 * gj_u32_nand_u2 surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4665_marker[] = "libcgj-batch4665";

/* ---- freestanding helpers ---------------------------------------------- */

/* Bitwise NAND of two 32-bit words. */
static uint32_t
b4665_nand(uint32_t u32A, uint32_t u32B)
{
	return ~(u32A & u32B);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_nand_u2 - bitwise NAND of two uint32_t values.
 *
 * a: left operand
 * b: right operand
 *
 * Returns ~(a & b). Distinct symbol from gj_u32_nand_u (batch3218).
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_nand_u2(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4665_nand(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_nand_u2(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_nand_u2")));
