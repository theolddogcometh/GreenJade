/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4668: bitwise 2:1 mux (u32, select s).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mux_u(uint32_t s, uint32_t a, uint32_t b);
 *     - Per-bit mux: (s & a) | (~s & b). Where s bit is 1 take a,
 *       else take b.
 *   uint32_t __gj_u32_mux_u  (alias)
 *   __libcgj_batch4668_marker = "libcgj-batch4668"
 *
 * Exclusive continuum CREATE-ONLY (4661-4670: u32_andn_u2, u64_andn_u2,
 * u32_orn_u, u32_xnor_u, u32_nand_u2, u32_nor_u, u32_majority_u,
 * u32_mux_u, u32_implies_u, batch_id_4670). Unique gj_u32_mux_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4668_marker[] = "libcgj-batch4668";

/* ---- freestanding helpers ---------------------------------------------- */

/* Per-bit 2:1 mux: s selects a (1) or b (0). */
static uint32_t
b4668_mux(uint32_t u32S, uint32_t u32A, uint32_t u32B)
{
	return (u32S & u32A) | (~u32S & u32B);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mux_u - bitwise multiplex a and b under select s.
 *
 * s: select mask (1 -> a, 0 -> b, per bit)
 * a: value taken where s is 1
 * b: value taken where s is 0
 *
 * Returns (s & a) | (~s & b). Self-contained; no parent wires.
 */
uint32_t
gj_u32_mux_u(uint32_t u32S, uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4668_mux(u32S, u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mux_u(uint32_t u32S, uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_mux_u")));
