/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4059: uint32_t range remap (exclusive remap_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_remap_u(uint32_t x, uint32_t in0, uint32_t in1,
 *                           uint32_t out0, uint32_t out1);
 *     - Map x from input range [in0, in1] onto output range [out0, out1]
 *       via unorm inv_lerp then lerp. Endpoints: x==in0 → out0;
 *       x==in1 → out1 (when in0 != in1). in0==in1 → out0.
 *   uint32_t __gj_u32_remap_u  (alias)
 *   __libcgj_batch4059_marker = "libcgj-batch4059"
 *
 * CREATE-ONLY exclusive continuum wave (4051-4060). Unique
 * gj_u32_remap_u surface only; no multi-def. Self-contained composition
 * of inv_lerp + lerp_unorm math (no parent wires to batches 4055/4057).
 * No __int128 (uses uint64 intermediates).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4059_marker[] = "libcgj-batch4059";

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor of (num * UINT32_MAX) / den for den > 0. */
static uint32_t
b4059_scale_to_unorm(uint32_t u32Num, uint32_t u32Den)
{
	return (uint32_t)(((uint64_t)u32Num * (uint64_t)UINT32_MAX) /
	    (uint64_t)u32Den);
}

/* Floor of (x * t) / UINT32_MAX. */
static uint32_t
b4059_mul_div_max(uint32_t u32X, uint32_t u32T)
{
	return (uint32_t)(((uint64_t)u32X * (uint64_t)u32T) /
	    (uint64_t)UINT32_MAX);
}

static uint32_t
b4059_inv_lerp(uint32_t u32A, uint32_t u32B, uint32_t u32V)
{
	uint32_t u32Span;
	uint32_t u32Off;

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
		return b4059_scale_to_unorm(u32Off, u32Span);
	}
	if (u32V >= u32A) {
		return 0u;
	}
	if (u32V <= u32B) {
		return UINT32_MAX;
	}
	u32Span = u32A - u32B;
	u32Off = u32A - u32V;
	return b4059_scale_to_unorm(u32Off, u32Span);
}

static uint32_t
b4059_lerp_unorm(uint32_t u32A, uint32_t u32B, uint32_t u32T)
{
	uint32_t u32Diff;
	uint32_t u32Mix;

	if (u32T == 0u) {
		return u32A;
	}
	if (u32T == UINT32_MAX) {
		return u32B;
	}
	if (u32A == u32B) {
		return u32A;
	}
	if (u32B >= u32A) {
		u32Diff = u32B - u32A;
		u32Mix = b4059_mul_div_max(u32Diff, u32T);
		return u32A + u32Mix;
	}
	u32Diff = u32A - u32B;
	u32Mix = b4059_mul_div_max(u32Diff, u32T);
	return u32A - u32Mix;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_remap_u - map x from [in0,in1] onto [out0,out1].
 *
 * x:              value in the input domain
 * in0, in1:       input range endpoints (order may be inverted)
 * out0, out1:     output range endpoints (order may be inverted)
 *
 * in0==in1 returns out0. Otherwise computes unorm t of x in the input
 * range and lerps the output range. Self-contained; no parent wires.
 */
uint32_t
gj_u32_remap_u(uint32_t u32X, uint32_t u32In0, uint32_t u32In1,
    uint32_t u32Out0, uint32_t u32Out1)
{
	uint32_t u32T;

	(void)NULL;
	if (u32In0 == u32In1) {
		return u32Out0;
	}
	u32T = b4059_inv_lerp(u32In0, u32In1, u32X);
	return b4059_lerp_unorm(u32Out0, u32Out1, u32T);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_remap_u(uint32_t u32X, uint32_t u32In0, uint32_t u32In1,
    uint32_t u32Out0, uint32_t u32Out1)
    __attribute__((alias("gj_u32_remap_u")));
