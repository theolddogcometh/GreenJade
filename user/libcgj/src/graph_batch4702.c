/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4702: saturating uint32_t multiply-add.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_sat_mul_add_u(uint32_t a, uint32_t b, uint32_t c);
 *     - sat(a * b + c): multiply then add, saturating at UINT32_MAX.
 *   uint32_t __gj_u32_sat_mul_add_u  (alias)
 *   __libcgj_batch4702_marker = "libcgj-batch4702"
 *
 * Exclusive continuum CREATE-ONLY (4701-4710). Distinct from
 * gj_u32_sat_mul_u (batch2379) and gj_u32_sat_add_u (batch2375) —
 * unique gj_u32_sat_mul_add_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4702_marker[] = "libcgj-batch4702";

/* ---- freestanding helpers ---------------------------------------------- */

/* Overflow iff a > floor(UINT32_MAX / b) for nonzero b. */
static int
b4702_umul_overflows(uint32_t u32A, uint32_t u32B)
{
	if (u32A == 0u || u32B == 0u) {
		return 0;
	}
	return (u32A > (UINT32_MAX / u32B)) ? 1 : 0;
}

/* Saturating a * b in the uint32_t domain. */
static uint32_t
b4702_sat_mul(uint32_t u32A, uint32_t u32B)
{
	if (b4702_umul_overflows(u32A, u32B)) {
		return UINT32_MAX;
	}
	return u32A * u32B;
}

/* Saturating a + b in the uint32_t domain. */
static uint32_t
b4702_sat_add(uint32_t u32A, uint32_t u32B)
{
	if (u32B > (UINT32_MAX - u32A)) {
		return UINT32_MAX;
	}
	return u32A + u32B;
}

/* sat(a * b + c): mul-sat then add-sat. */
static uint32_t
b4702_sat_mul_add(uint32_t u32A, uint32_t u32B, uint32_t u32C)
{
	return b4702_sat_add(b4702_sat_mul(u32A, u32B), u32C);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_sat_mul_add_u - unsigned 32-bit saturating multiply-add.
 *
 * a, b: factors; c: addend
 *
 * Computes sat(sat(a * b) + c) with clamp to UINT32_MAX on any
 * overflow of the product or the sum. Zero factors yield sat(c).
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_sat_mul_add_u(uint32_t u32A, uint32_t u32B, uint32_t u32C)
{
	(void)NULL;
	return b4702_sat_mul_add(u32A, u32B, u32C);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_sat_mul_add_u(uint32_t u32A, uint32_t u32B, uint32_t u32C)
    __attribute__((alias("gj_u32_sat_mul_add_u")));
