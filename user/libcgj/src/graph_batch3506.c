/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3506: modular add on uint64_t (_u surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mod_add_u(uint64_t a, uint64_t b, uint64_t m);
 *     — compute (a + b) mod m. Precondition m != 0; if m == 0 returns 0
 *       (soft guard). Operands reduced mod m first.
 *   uint64_t __gj_u64_mod_add_u  (alias)
 *   __libcgj_batch3506_marker = "libcgj-batch3506"
 *
 * Post-3500 modular/number-theory exclusive wave (3501–3510). Unique
 * gj_u64_mod_add_u surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3506_marker[] = "libcgj-batch3506";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * (a + b) mod m. When m == 0, returns 0 (soft guard).
 * Overflow-safe: avoids a+b wrap by comparing against m-a.
 */
static uint64_t
b3506_mod_add(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	uint64_t u64S;

	if (u64M == 0ull) {
		return 0ull;
	}
	u64A %= u64M;
	u64B %= u64M;
	u64S = u64M - u64A;
	if (u64B >= u64S) {
		return u64B - u64S;
	}
	return u64A + u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mod_add_u — (a + b) mod m for uint64_t.
 *
 * a, b: addends; m: modulus (0 → 0 soft guard)
 */
uint64_t
gj_u64_mod_add_u(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	(void)NULL;
	return b3506_mod_add(u64A, u64B, u64M);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mod_add_u(uint64_t u64A, uint64_t u64B, uint64_t u64M)
    __attribute__((alias("gj_u64_mod_add_u")));
