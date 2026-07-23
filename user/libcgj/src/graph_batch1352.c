/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1352: Szudzik unpairing (inverse pair).
 *
 * Surface (unique symbols):
 *   void gj_szudzik_unpair(uint64_t z, uint32_t *x, uint32_t *y);
 *     — Recover (x, y) from a Szudzik code. NULL out-pointers ignored.
 *   void __gj_szudzik_unpair  (alias)
 *   __libcgj_batch1352_marker = "libcgj-batch1352"
 *
 * Distinct from Cantor/elegant unpair. Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1352_marker[] = "libcgj-batch1352";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1352_isqrt(uint64_t u64N)
{
	uint64_t u64Lo;
	uint64_t u64Hi;
	uint64_t u64Mid;
	uint64_t u64Sq;

	if (u64N < 2ull) {
		return u64N;
	}
	u64Lo = 1ull;
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
 * gj_szudzik_unpair — invert Szudzik pairing.
 *
 * shell = floor(sqrt(z))
 * t = z - shell*shell
 * if t < shell then (x,y) = (t, shell) else (shell, t - shell)
 */
void
gj_szudzik_unpair(uint64_t u64Z, uint32_t *pX, uint32_t *pY)
{
	uint64_t u64Shell;
	uint64_t u64T;
	uint64_t u64X;
	uint64_t u64Y;

	u64Shell = b1352_isqrt(u64Z);
	u64T = u64Z - u64Shell * u64Shell;
	if (u64T < u64Shell) {
		u64X = u64T;
		u64Y = u64Shell;
	} else {
		u64X = u64Shell;
		u64Y = u64T - u64Shell;
	}

	if (pX != NULL) {
		*pX = (u64X > 0xffffffffull) ? 0xffffffffu : (uint32_t)u64X;
	}
	if (pY != NULL) {
		*pY = (u64Y > 0xffffffffull) ? 0xffffffffu : (uint32_t)u64Y;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_szudzik_unpair(uint64_t u64Z, uint32_t *pX, uint32_t *pY)
    __attribute__((alias("gj_szudzik_unpair")));
