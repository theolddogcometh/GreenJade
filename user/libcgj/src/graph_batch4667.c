/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4667: bitwise majority of three uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_majority_u(uint32_t a, uint32_t b, uint32_t c);
 *     - Per-bit majority: (a&b)|(a&c)|(b&c). Bit is 1 when at least
 *       two of a, b, c have that bit set.
 *   uint32_t __gj_u32_majority_u  (alias)
 *   __libcgj_batch4667_marker = "libcgj-batch4667"
 *
 * Exclusive continuum CREATE-ONLY (4661-4670: u32_andn_u2, u64_andn_u2,
 * u32_orn_u, u32_xnor_u, u32_nand_u2, u32_nor_u, u32_majority_u,
 * u32_mux_u, u32_implies_u, batch_id_4670). Unique gj_u32_majority_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4667_marker[] = "libcgj-batch4667";

/* ---- freestanding helpers ---------------------------------------------- */

/* Per-bit majority of three 32-bit words. */
static uint32_t
b4667_majority(uint32_t u32A, uint32_t u32B, uint32_t u32C)
{
	return (u32A & u32B) | (u32A & u32C) | (u32B & u32C);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_majority_u - per-bit majority of three uint32_t values.
 *
 * a, b, c: inputs
 *
 * Returns (a&b)|(a&c)|(b&c). Self-contained; no parent wires.
 */
uint32_t
gj_u32_majority_u(uint32_t u32A, uint32_t u32B, uint32_t u32C)
{
	(void)NULL;
	return b4667_majority(u32A, u32B, u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_majority_u(uint32_t u32A, uint32_t u32B, uint32_t u32C)
    __attribute__((alias("gj_u32_majority_u")));
