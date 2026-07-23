/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3507: modular sub on uint32_t (_u surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mod_sub_u(uint32_t a, uint32_t b, uint32_t m);
 *     — compute (a - b) mod m. Precondition m != 0; if m == 0 returns 0
 *       (soft guard). Operands reduced mod m first.
 *   uint32_t __gj_u32_mod_sub_u  (alias)
 *   __libcgj_batch3507_marker = "libcgj-batch3507"
 *
 * Post-3500 modular/number-theory exclusive wave (3501–3510). Unique
 * gj_u32_mod_sub_u surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3507_marker[] = "libcgj-batch3507";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * (a - b) mod m. When m == 0, returns 0 (soft guard).
 */
static uint32_t
b3507_mod_sub(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	if (u32M == 0u) {
		return 0u;
	}
	u32A %= u32M;
	u32B %= u32M;
	if (u32A >= u32B) {
		return u32A - u32B;
	}
	return u32M - (u32B - u32A);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mod_sub_u — (a - b) mod m for uint32_t.
 *
 * a, b: operands; m: modulus (0 → 0 soft guard)
 */
uint32_t
gj_u32_mod_sub_u(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	(void)NULL;
	return b3507_mod_sub(u32A, u32B, u32M);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mod_sub_u(uint32_t u32A, uint32_t u32B, uint32_t u32M)
    __attribute__((alias("gj_u32_mod_sub_u")));
