/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4663: ORN bitwise or-not (u32, a | ~b).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_orn_u(uint32_t a, uint32_t b);
 *     - Return a | ~b. Pure integer bitwise combine.
 *   uint32_t __gj_u32_orn_u  (alias)
 *   __libcgj_batch4663_marker = "libcgj-batch4663"
 *
 * Exclusive continuum CREATE-ONLY (4661-4670: u32_andn_u2, u64_andn_u2,
 * u32_orn_u, u32_xnor_u, u32_nand_u2, u32_nor_u, u32_majority_u,
 * u32_mux_u, u32_implies_u, batch_id_4670). Unique gj_u32_orn_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4663_marker[] = "libcgj-batch4663";

/* ---- freestanding helpers ---------------------------------------------- */

/* OR with inverted second operand: a | ~b. */
static uint32_t
b4663_orn(uint32_t u32A, uint32_t u32B)
{
	return u32A | ~u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_orn_u - bitwise OR of a with inverted b (32-bit).
 *
 * a: left operand (kept)
 * b: right operand (inverted before OR)
 *
 * Returns a | ~b. Self-contained; no parent wires.
 */
uint32_t
gj_u32_orn_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4663_orn(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_orn_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_orn_u")));
