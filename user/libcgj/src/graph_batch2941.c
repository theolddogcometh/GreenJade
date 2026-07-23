/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2941: modular subtract on uint64_t (_u surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_sub_mod_u(uint64_t a, uint64_t b, uint64_t m);
 *     — (a - b) mod m. m==0 → 0 (invalid modulus). m==1 → 0.
 *       a and b are reduced mod m first when m > 0.
 *   uint64_t __gj_u64_sub_mod_u  (alias)
 *   __libcgj_batch2941_marker = "libcgj-batch2941"
 *
 * CREATE-ONLY exclusive continuum wave (2941–2950). Unique
 * gj_u64_sub_mod_u surface; no parent wires, no multi-def. Distinct from
 * gj_u64_sub_sat / gj_u64_sub_wrap / gj_u64_sub_check.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2941_marker[] = "libcgj-batch2941";

/* ---- freestanding helpers ---------------------------------------------- */

/* (a - b) mod m for m > 1; a, b already in [0, m). */
static uint64_t
b2941_sub_mod(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	if (u64A >= u64B) {
		return u64A - u64B;
	}
	return u64M - (u64B - u64A);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sub_mod_u — modular difference (a - b) mod m.
 *
 * a, b: operands
 * m:    modulus (must be non-zero; soft 0 if m == 0)
 *
 * Special cases:
 *   m == 0  → 0  (invalid modulus)
 *   m == 1  → 0
 */
uint64_t
gj_u64_sub_mod_u(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	(void)NULL;
	if (u64M == 0ull || u64M == 1ull) {
		return 0ull;
	}
	u64A %= u64M;
	u64B %= u64M;
	return b2941_sub_mod(u64A, u64B, u64M);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_sub_mod_u(uint64_t u64A, uint64_t u64B, uint64_t u64M)
    __attribute__((alias("gj_u64_sub_mod_u")));
