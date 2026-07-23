/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1349: Cantor unpairing (inverse pair).
 *
 * Surface (unique symbols):
 *   void gj_cantor_unpair(uint64_t z, uint32_t *x, uint32_t *y);
 *     — Recover (x, y) from a Cantor code. NULL out-pointers ignored.
 *       Codes that would yield x or y > UINT32_MAX clamp those outs to
 *       UINT32_MAX (defensive; full encode domain always fits).
 *   void __gj_cantor_unpair  (alias)
 *   __libcgj_batch1349_marker = "libcgj-batch1349"
 *
 * Distinct from Szudzik/elegant unpair. Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1349_marker[] = "libcgj-batch1349";

/* ---- freestanding helpers ---------------------------------------------- */

/* Integer square root via binary search (floor sqrt). */
static uint64_t
b1349_isqrt(uint64_t u64N)
{
	uint64_t u64Lo;
	uint64_t u64Hi;
	uint64_t u64Mid;
	uint64_t u64Sq;

	if (u64N < 2ull) {
		return u64N;
	}
	u64Lo = 1ull;
	u64Hi = u64N;
	if (u64Hi > 0xffffffffull) {
		u64Hi = 0xffffffffull; /* enough for our uses */
	}
	/* Widen hi if n is large: floor(sqrt(2^64-1)) = 2^32-1 */
	u64Hi = (u64N >> 1) + 1ull;
	if (u64Hi > 0xffffffffull) {
		u64Hi = 0xffffffffull;
	}
	while (u64Lo <= u64Hi) {
		u64Mid = u64Lo + ((u64Hi - u64Lo) >> 1);
		u64Sq = u64Mid * u64Mid;
		if (u64Sq == u64N) {
			return u64Mid;
		}
		if (u64Sq < u64N) {
			u64Lo = u64Mid + 1ull;
		} else {
			if (u64Mid == 0ull) {
				break;
			}
			u64Hi = u64Mid - 1ull;
		}
	}
	return u64Hi;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cantor_unpair — invert π(x,y) = (x+y)(x+y+1)/2 + y.
 *
 * w = floor((sqrt(8z+1) - 1) / 2)
 * t = w(w+1)/2
 * y = z - t
 * x = w - y
 */
void
gj_cantor_unpair(uint64_t u64Z, uint32_t *pX, uint32_t *pY)
{
	uint64_t u64W;
	uint64_t u64T;
	uint64_t u64Y;
	uint64_t u64X;
	uint64_t u64Disc;

	/* disc = 8z+1; guard overflow of 8z */
	if (u64Z > (UINT64_MAX - 1ull) / 8ull) {
		u64Disc = UINT64_MAX;
	} else {
		u64Disc = 8ull * u64Z + 1ull;
	}
	u64W = (b1349_isqrt(u64Disc) - 1ull) / 2ull;
	u64T = (u64W * (u64W + 1ull)) / 2ull;
	u64Y = u64Z - u64T;
	u64X = u64W - u64Y;

	if (pX != NULL) {
		*pX = (u64X > 0xffffffffull) ? 0xffffffffu : (uint32_t)u64X;
	}
	if (pY != NULL) {
		*pY = (u64Y > 0xffffffffull) ? 0xffffffffu : (uint32_t)u64Y;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_cantor_unpair(uint64_t u64Z, uint32_t *pX, uint32_t *pY)
    __attribute__((alias("gj_cantor_unpair")));
