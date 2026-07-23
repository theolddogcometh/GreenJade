/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3505: modular add on uint32_t (_u surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mod_add_u(uint32_t a, uint32_t b, uint32_t m);
 *     — compute (a + b) mod m. Precondition m != 0; if m == 0 returns 0
 *       (soft guard). Operands reduced mod m first.
 *   uint32_t __gj_u32_mod_add_u  (alias)
 *   __libcgj_batch3505_marker = "libcgj-batch3505"
 *
 * Post-3500 modular/number-theory exclusive wave (3501–3510). Unique
 * gj_u32_mod_add_u surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3505_marker[] = "libcgj-batch3505";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * (a + b) mod m. When m == 0, returns 0 (soft guard).
 * Overflow-safe: avoids a+b wrap by comparing against m-a.
 */
static uint32_t
b3505_mod_add(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	uint32_t u32S;

	if (u32M == 0u) {
		return 0u;
	}
	u32A %= u32M;
	u32B %= u32M;
	u32S = u32M - u32A;
	if (u32B >= u32S) {
		return u32B - u32S;
	}
	return u32A + u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mod_add_u — (a + b) mod m for uint32_t.
 *
 * a, b: addends; m: modulus (0 → 0 soft guard)
 */
uint32_t
gj_u32_mod_add_u(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	(void)NULL;
	return b3505_mod_add(u32A, u32B, u32M);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mod_add_u(uint32_t u32A, uint32_t u32B, uint32_t u32M)
    __attribute__((alias("gj_u32_mod_add_u")));
