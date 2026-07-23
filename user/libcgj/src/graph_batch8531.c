/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8531: complex |z|^2 squared-magnitude integer stub.
 *
 * Surface (unique symbols):
 *   uint64_t gj_cabs_sq_u_8531(int32_t re, int32_t im);
 *     - Returns re*re + im*im as uint64_t (squared complex modulus).
 *       Integer magnitude-ish only — no sqrt, no float. INT32_MIN is
 *       handled via two's-complement unsigned magnitude (no signed
 *       negation of INT32_MIN). Sum saturates at UINT64_MAX on wrap
 *       (cannot occur for int32 legs).
 *   uint64_t __gj_cabs_sq_u_8531  (alias)
 *   __libcgj_batch8531_marker = "libcgj-batch8531"
 *
 * Exclusive continuum CREATE-ONLY (8531-8540: complex/polar id stubs —
 * cabs_sq, creal_id, cimag_id, cconj_re, cconj_im, cmul_re, cmul_im,
 * cadd_re, cadd_im, batch_id_8540). Unique gj_cabs_sq_u_8531 surface
 * only; no multi-def. Distinct from gj_dist2_u32_u (batch3075) /
 * gj_hypot_u32_u (batch3073). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8531_marker[] = "libcgj-batch8531";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b8531_mag_u32 — absolute magnitude of int32 as uint32.
 * INT32_MIN maps to 0x80000000u via two's-complement cast (no UB).
 */
static uint32_t
b8531_mag_u32(int32_t i32V)
{
	if (i32V < 0) {
		return (uint32_t)(-(uint32_t)i32V);
	}
	return (uint32_t)i32V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cabs_sq_u_8531 - squared complex absolute value |re + i*im|^2.
 *
 * re, im: real / imaginary parts (signed int32)
 *
 * Returns re^2 + im^2 as uint64_t. Prefer over hypot when only
 * comparisons / energy matter. Pure integer; no parent wires.
 */
uint64_t
gj_cabs_sq_u_8531(int32_t i32Re, int32_t i32Im)
{
	uint32_t u32Ar;
	uint32_t u32Ai;
	uint64_t u64Rr;
	uint64_t u64Ii;
	uint64_t u64Sum;

	(void)NULL;

	u32Ar = b8531_mag_u32(i32Re);
	u32Ai = b8531_mag_u32(i32Im);
	u64Rr = (uint64_t)u32Ar * (uint64_t)u32Ar;
	u64Ii = (uint64_t)u32Ai * (uint64_t)u32Ai;
	u64Sum = u64Rr + u64Ii;
	if (u64Sum < u64Rr) {
		return UINT64_MAX;
	}
	return u64Sum;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_cabs_sq_u_8531(int32_t i32Re, int32_t i32Im)
    __attribute__((alias("gj_cabs_sq_u_8531")));
