/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1915: analog axis deadzone apply.
 *
 * Surface (unique symbols):
 *   int16_t gj_axis_deadzone_apply(int16_t v, int16_t dz);
 *     — If |v| is strictly less than |dz|, return 0 (inside deadzone).
 *       Otherwise return v unchanged. dz may be negative; magnitude
 *       is used. Pure integer, no scaling.
 *   int16_t __gj_axis_deadzone_apply  (alias)
 *   __libcgj_batch1915_marker = "libcgj-batch1915"
 *
 * Input-device exclusive pure-data wave (1911–1919; milestone 1920).
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno,
 * no libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1915_marker[] = "libcgj-batch1915";

/* ---- freestanding helpers ---------------------------------------------- */

/* Absolute value for int16_t; maps INT16_MIN to INT16_MAX. */
static int16_t
b1915_abs16(int16_t nV)
{
	int32_t nW;

	nW = (int32_t)nV;
	if (nW < 0) {
		nW = -nW;
		if (nW > 32767) {
			nW = 32767;
		}
	}
	return (int16_t)nW;
}

/* Zero if |v| < |dz|, else v. */
static int16_t
b1915_deadzone(int16_t nV, int16_t nDz)
{
	int16_t nAv;
	int16_t nAd;

	nAv = b1915_abs16(nV);
	nAd = b1915_abs16(nDz);
	if (nAv < nAd) {
		return (int16_t)0;
	}
	return nV;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_axis_deadzone_apply — zero axis sample inside deadzone radius.
 *
 * v:  signed axis sample (e.g. -32768..32767 style)
 * dz: deadzone radius (magnitude used; sign ignored)
 * Returns 0 when |v| < |dz|, else v.
 */
int16_t
gj_axis_deadzone_apply(int16_t nV, int16_t nDz)
{
	(void)NULL;
	return b1915_deadzone(nV, nDz);
}

/* ---- underscored alias ------------------------------------------------- */

int16_t __gj_axis_deadzone_apply(int16_t nV, int16_t nDz)
    __attribute__((alias("gj_axis_deadzone_apply")));
