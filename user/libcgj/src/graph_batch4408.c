/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4408: safe uint32_t modulo (b==0 → 0).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mod_safe_u(uint32_t a, uint32_t b);
 *     - a % b as unsigned remainder. b == 0 → 0 (no trap).
 *   uint32_t __gj_u32_mod_safe_u  (alias)
 *   __libcgj_batch4408_marker = "libcgj-batch4408"
 *
 * Exclusive continuum CREATE-ONLY (4401-4410). Distinct from
 * gj_u32_rem_u / wrap rem surfaces — unique gj_u32_mod_safe_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4408_marker[] = "libcgj-batch4408";

/* ---- freestanding helpers ---------------------------------------------- */

/* a % b; b == 0 yields 0. */
static uint32_t
b4408_mod_safe(uint32_t u32A, uint32_t u32B)
{
	if (u32B == 0u) {
		return 0u;
	}
	return u32A % u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mod_safe_u - safe unsigned remainder; b == 0 yields 0.
 *
 * a: dividend
 * b: divisor (0 → 0)
 * No parent wires.
 */
uint32_t
gj_u32_mod_safe_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4408_mod_safe(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mod_safe_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_mod_safe_u")));
