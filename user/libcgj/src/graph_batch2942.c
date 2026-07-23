/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2942: portable modular multiply on uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mul_mod_u(uint64_t a, uint64_t b, uint64_t m);
 *     — (a * b) mod m via binary modular multiply (portable, no __int128).
 *       m==0 → 0 (invalid modulus). m==1 → 0.
 *   uint64_t __gj_u64_mul_mod_u  (alias)
 *   __libcgj_batch2942_marker = "libcgj-batch2942"
 *
 * CREATE-ONLY exclusive continuum wave (2941–2950). Unique
 * gj_u64_mul_mod_u surface; no parent wires, no multi-def. Portable:
 * no __int128 / no libgcc __umodti3. Distinct from gj_mul_mod_u32
 * (batch841) and internal mulmod helpers of other TUs.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2942_marker[] = "libcgj-batch2942";

/* ---- freestanding helpers (no __int128 / no libgcc soft-quad) --------- */

/*
 * (a * b) mod m for m > 1.
 * Binary modular multiply: freestanding-safe (no 128-bit rem).
 * Addition/doubling use (x >= m - y) ? (x - (m - y)) : (x + y) so
 * intermediate sums never wrap past m.
 */
static uint64_t
b2942_mulmod(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	uint64_t u64R;

	u64A %= u64M;
	u64B %= u64M;
	u64R = 0ull;
	while (u64B != 0ull) {
		if ((u64B & 1ull) != 0ull) {
			uint64_t u64S = u64M - u64R;

			u64R = (u64A >= u64S) ? (u64A - u64S) : (u64R + u64A);
		}
		{
			uint64_t u64S = u64M - u64A;

			u64A = (u64A >= u64S) ? (u64A - u64S) : (u64A + u64A);
		}
		u64B >>= 1;
	}
	return u64R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mul_mod_u — portable (a * b) mod m without __int128.
 *
 * a, b: factors
 * m:    modulus (must be non-zero; soft 0 if m == 0)
 *
 * Special cases:
 *   m == 0  → 0  (invalid modulus)
 *   m == 1  → 0
 */
uint64_t
gj_u64_mul_mod_u(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	(void)NULL;
	if (u64M == 0ull || u64M == 1ull) {
		return 0ull;
	}
	return b2942_mulmod(u64A, u64B, u64M);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mul_mod_u(uint64_t u64A, uint64_t u64B, uint64_t u64M)
    __attribute__((alias("gj_u64_mul_mod_u")));
