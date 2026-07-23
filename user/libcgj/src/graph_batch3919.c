/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3919: radical rad(n) for uint32 (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_radical_u(uint32_t n);
 *     - Radical of n: product of distinct prime factors of n.
 *       rad(0)=0, rad(1)=1 (empty product). Square-free kernel of n.
 *   uint32_t __gj_u32_radical_u  (alias)
 *   __libcgj_batch3919_marker = "libcgj-batch3919"
 *
 * Exclusive continuum CREATE-ONLY (3911-3920). Unique
 * gj_u32_radical_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3919_marker[] = "libcgj-batch3919";

/* ---- freestanding helpers ---------------------------------------------- */

/* rad(n) = product of distinct primes dividing n. */
static uint32_t
b3919_radical(uint32_t u32N)
{
	uint32_t u32Result;
	uint32_t u32X;
	uint32_t u32P;

	if (u32N == 0u) {
		return 0u;
	}
	if (u32N == 1u) {
		return 1u;
	}

	u32Result = 1u;
	u32X = u32N;

	/* Factor 2. */
	if ((u32X & 1u) == 0u) {
		u32Result *= 2u;
		while ((u32X & 1u) == 0u) {
			u32X >>= 1u;
		}
	}

	/* Odd factors. */
	for (u32P = 3u; u32P <= u32X / u32P; u32P += 2u) {
		if ((u32X % u32P) == 0u) {
			u32Result *= u32P;
			while ((u32X % u32P) == 0u) {
				u32X /= u32P;
			}
		}
	}

	/* Remaining prime. */
	if (u32X > 1u) {
		u32Result *= u32X;
	}

	return u32Result;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_radical_u - radical (product of distinct prime factors) of n.
 *
 * n: input (0 -> 0, 1 -> 1)
 * No parent wires.
 */
uint32_t
gj_u32_radical_u(uint32_t u32N)
{
	(void)NULL;
	return b3919_radical(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_radical_u(uint32_t u32N)
    __attribute__((alias("gj_u32_radical_u")));
