/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch430: uint64 modulo with zero-divisor guard.
 *
 * Surface (unique symbols):
 *   uint64_t gj_mod_u64(uint64_t a, uint64_t b);
 *     — Return a % b. b == 0 → 0.
 *   uint64_t __gj_mod_u64  (alias)
 *   __libcgj_batch430_marker = "libcgj-batch430"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch430_marker[] = "libcgj-batch430";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mod_u64 — unsigned remainder a % b with zero-divisor guard.
 *
 * a: dividend
 * b: divisor (0 → return 0)
 *
 * Returns a modulo b. Distinct from modular-power helpers (batch230).
 */
uint64_t
gj_mod_u64(uint64_t a, uint64_t b)
{
	if (b == 0u) {
		return 0u;
	}
	return a % b;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_mod_u64(uint64_t a, uint64_t b)
    __attribute__((alias("gj_mod_u64")));
