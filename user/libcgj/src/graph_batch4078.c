/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4078: uint64_t modular mirror / ping-pong (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mod_mirror_u(uint64_t x, uint64_t m);
 *     - Fold x into the closed interval [0, m] by ping-pong reflection
 *       over successive half-periods of length m. Even quotient cycles
 *       rise 0..m; odd cycles fall m..0. m == 0 → 0 (soft guard).
 *   uint64_t __gj_u64_mod_mirror_u  (alias)
 *   __libcgj_batch4078_marker = "libcgj-batch4078"
 *
 * Exclusive continuum CREATE-ONLY wave (4071-4080). Unique
 * gj_u64_mod_mirror_u surface only; no multi-def. Distinct from
 * gj_u64_mod_wrap_u (batch4076), gj_u64_tri_u (batch4072), and
 * gj_u32_mod_mirror_u (batch4077). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4078_marker[] = "libcgj-batch4078";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Ping-pong fold of x into [0, m]. Avoids 2*m (overflow when
 * m > UINT64_MAX/2) by using cycle parity of x / m.
 */
static uint64_t
b4078_mod_mirror(uint64_t u64X, uint64_t u64M)
{
	uint64_t u64Cycle;
	uint64_t u64Rem;

	if (u64M == 0ull) {
		return 0ull;
	}
	u64Cycle = u64X / u64M;
	u64Rem = u64X % u64M;
	if ((u64Cycle & 1ull) == 0ull) {
		return u64Rem;
	}
	return u64M - u64Rem;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_mod_mirror_u - ping-pong fold x into [0, m].
 *
 * x: value to fold
 * m: half-period / peak (0 → 0 soft guard)
 *
 * Returns a value in [0, m] by reflecting across successive segments
 * of length m. No parent wires.
 */
uint64_t
gj_u64_mod_mirror_u(uint64_t u64X, uint64_t u64M)
{
	(void)NULL;
	return b4078_mod_mirror(u64X, u64M);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mod_mirror_u(uint64_t u64X, uint64_t u64M)
    __attribute__((alias("gj_u64_mod_mirror_u")));
