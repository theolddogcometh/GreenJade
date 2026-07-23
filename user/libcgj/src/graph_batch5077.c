/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5077: power-of-two crypto block-size probe.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crypto_is_pow2_block_u(uint32_t n);
 *     - Returns 1 if n is a non-zero power of two (valid classic cipher
 *       block width: 1, 2, 4, …, 0x80000000); else 0.
 *   uint32_t __gj_crypto_is_pow2_block_u  (alias)
 *   __libcgj_batch5077_marker = "libcgj-batch5077"
 *
 * Exclusive continuum CREATE-ONLY (5071-5080). Distinct from
 * gj_is_pow2_u64 (batch373) and gj_u32_is_pow2 (batch777) —
 * unique gj_crypto_is_pow2_block_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5077_marker[] = "libcgj-batch5077";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5077_is_pow2_block(uint32_t u32N)
{
	if (u32N == 0u) {
		return 0u;
	}
	if ((u32N & (u32N - 1u)) != 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crypto_is_pow2_block_u - test whether n is a power-of-two block size.
 *
 * n: candidate block width in bytes
 *
 * Returns 1 when n is 2^k for some k in [0, 31]; returns 0 when n is 0
 * or has more than one bit set. Self-contained; no parent wires.
 */
uint32_t
gj_crypto_is_pow2_block_u(uint32_t u32N)
{
	(void)NULL;
	return b5077_is_pow2_block(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crypto_is_pow2_block_u(uint32_t u32N)
    __attribute__((alias("gj_crypto_is_pow2_block_u")));
