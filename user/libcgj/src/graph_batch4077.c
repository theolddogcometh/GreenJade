/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4077: uint32_t modular mirror / ping-pong (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mod_mirror_u(uint32_t x, uint32_t m);
 *     - Fold x into the closed interval [0, m] by ping-pong reflection
 *       over successive half-periods of length m. Even quotient cycles
 *       rise 0..m; odd cycles fall m..0. m == 0 → 0 (soft guard).
 *   uint32_t __gj_u32_mod_mirror_u  (alias)
 *   __libcgj_batch4077_marker = "libcgj-batch4077"
 *
 * Exclusive continuum CREATE-ONLY wave (4071-4080). Unique
 * gj_u32_mod_mirror_u surface only; no multi-def. Distinct from
 * gj_u32_mod_wrap_u (batch4075), gj_u32_tri_u (batch4071), and
 * gj_u32_wrap_range_u (batch3266). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4077_marker[] = "libcgj-batch4077";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Ping-pong fold of x into [0, m].
 *
 * cycle = x / m, rem = x % m:
 *   even cycle → rem          (rising; rem in [0, m-1], and at exact
 *                              multiples rem==0 lands on 0)
 *   odd cycle  → m - rem      (falling; rem==0 at x=m,3m,... yields m)
 *
 * Avoids computing 2*m (which overflows when m > UINT32_MAX/2).
 */
static uint32_t
b4077_mod_mirror(uint32_t u32X, uint32_t u32M)
{
	uint32_t u32Cycle;
	uint32_t u32Rem;

	if (u32M == 0u) {
		return 0u;
	}
	u32Cycle = u32X / u32M;
	u32Rem = u32X % u32M;
	if ((u32Cycle & 1u) == 0u) {
		return u32Rem;
	}
	return u32M - u32Rem;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mod_mirror_u - ping-pong fold x into [0, m].
 *
 * x: value to fold
 * m: half-period / peak (0 → 0 soft guard)
 *
 * Returns a value in [0, m] by reflecting across successive segments
 * of length m. No parent wires.
 */
uint32_t
gj_u32_mod_mirror_u(uint32_t u32X, uint32_t u32M)
{
	(void)NULL;
	return b4077_mod_mirror(u32X, u32M);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mod_mirror_u(uint32_t u32X, uint32_t u32M)
    __attribute__((alias("gj_u32_mod_mirror_u")));
