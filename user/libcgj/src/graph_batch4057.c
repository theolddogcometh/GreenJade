/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4057: uint32_t inverse lerp to unorm
 * (exclusive inv_lerp_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_inv_lerp_u(uint32_t a, uint32_t b, uint32_t v);
 *     - Inverse of unorm lerp: unorm t such that
 *       lerp_unorm(a, b, t) ≈ v. Returns floor(|v-a| * UINT32_MAX /
 *       |b-a|), clamped to [0, UINT32_MAX]. a==b → 0.
 *   uint32_t __gj_u32_inv_lerp_u  (alias)
 *   __libcgj_batch4057_marker = "libcgj-batch4057"
 *
 * CREATE-ONLY exclusive continuum wave (4051-4060). Unique
 * gj_u32_inv_lerp_u surface only; no multi-def. Companion of
 * gj_u32_lerp_unorm_u (batch4055) but self-contained (no parent wires).
 * No __int128 (uses uint64 product).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4057_marker[] = "libcgj-batch4057";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor of (num * UINT32_MAX) / den for den > 0. */
static uint32_t
b4057_scale_to_unorm(uint32_t u32Num, uint32_t u32Den)
{
	return (uint32_t)(((uint64_t)u32Num * (uint64_t)UINT32_MAX) /
	    (uint64_t)u32Den);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_inv_lerp_u — unorm weight of v between a and b.
 *
 * a, b: range endpoints (order may be inverted)
 * v:    sample value
 *
 * When a < b:
 *   v <= a → 0; v >= b → UINT32_MAX; else floor((v-a)*MAX/(b-a)).
 * When a > b:
 *   v >= a → 0; v <= b → UINT32_MAX; else floor((a-v)*MAX/(a-b)).
 * When a == b: returns 0. Self-contained; no parent wires.
 */
uint32_t
gj_u32_inv_lerp_u(uint32_t u32A, uint32_t u32B, uint32_t u32V)
{
	uint32_t u32Span;
	uint32_t u32Off;

	(void)NULL;
	if (u32A == u32B) {
		return 0u;
	}
	if (u32B > u32A) {
		if (u32V <= u32A) {
			return 0u;
		}
		if (u32V >= u32B) {
			return UINT32_MAX;
		}
		u32Span = u32B - u32A;
		u32Off = u32V - u32A;
		return b4057_scale_to_unorm(u32Off, u32Span);
	}
	/* a > b: inverted range; t=0 at a, t=1 at b. */
	if (u32V >= u32A) {
		return 0u;
	}
	if (u32V <= u32B) {
		return UINT32_MAX;
	}
	u32Span = u32A - u32B;
	u32Off = u32A - u32V;
	return b4057_scale_to_unorm(u32Off, u32Span);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_inv_lerp_u(uint32_t u32A, uint32_t u32B, uint32_t u32V)
    __attribute__((alias("gj_u32_inv_lerp_u")));
