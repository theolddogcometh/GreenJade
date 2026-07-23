/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3508: modular sub on uint64_t (_u surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mod_sub_u(uint64_t a, uint64_t b, uint64_t m);
 *     — compute (a - b) mod m. Precondition m != 0; if m == 0 returns 0
 *       (soft guard). Operands reduced mod m first.
 *   uint64_t __gj_u64_mod_sub_u  (alias)
 *   __libcgj_batch3508_marker = "libcgj-batch3508"
 *
 * Post-3500 modular/number-theory exclusive wave (3501–3510). Unique
 * gj_u64_mod_sub_u surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3508_marker[] = "libcgj-batch3508";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * (a - b) mod m. When m == 0, returns 0 (soft guard).
 */
static uint64_t
b3508_mod_sub(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	if (u64M == 0ull) {
		return 0ull;
	}
	u64A %= u64M;
	u64B %= u64M;
	if (u64A >= u64B) {
		return u64A - u64B;
	}
	return u64M - (u64B - u64A);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mod_sub_u — (a - b) mod m for uint64_t.
 *
 * a, b: operands; m: modulus (0 → 0 soft guard)
 */
uint64_t
gj_u64_mod_sub_u(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	(void)NULL;
	return b3508_mod_sub(u64A, u64B, u64M);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mod_sub_u(uint64_t u64A, uint64_t u64B, uint64_t u64M)
    __attribute__((alias("gj_u64_mod_sub_u")));
