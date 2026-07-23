/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2939: uint64 modular add (a+b) mod m (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_add_mod_u(uint64_t a, uint64_t b, uint64_t m);
 *     - (a + b) mod m. m==0 -> 0 (invalid modulus). m==1 -> 0.
 *       Result always in [0, m) when m > 0. No __int128.
 *   uint64_t __gj_u64_add_mod_u  (alias)
 *   __libcgj_batch2939_marker = "libcgj-batch2939"
 *
 * Pack/mod exclusive wave (2931-2940). Distinct from gj_u64_add_wrap
 * (batch2314) and gj_u64_wrap_add (batch738) — unique modular-with-m
 * _u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2939_marker[] = "libcgj-batch2939";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * (a + b) mod m. Overflow-free via (m - a) compare; no wider type.
 */
static uint64_t
b2939_addmod(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	uint64_t u64S;

	if (u64M == 0u) {
		return 0u;
	}
	if (u64M == 1u) {
		return 0u;
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
 * gj_u64_add_mod_u - (a + b) mod m.
 *
 * a, b: summands (any uint64_t; reduced mod m when m > 0)
 * m:    modulus; m==0 yields 0
 *
 * Returns (a + b) mod m in [0, m), or 0 when m is 0 or 1.
 */
uint64_t
gj_u64_add_mod_u(uint64_t u64A, uint64_t u64B, uint64_t u64M)
{
	(void)NULL;
	return b2939_addmod(u64A, u64B, u64M);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_add_mod_u(uint64_t u64A, uint64_t u64B, uint64_t u64M)
    __attribute__((alias("gj_u64_add_mod_u")));
