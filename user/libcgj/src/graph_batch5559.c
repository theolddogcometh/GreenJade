/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5559: Steam Input analog action deadzone.
 *
 * Surface (unique symbols):
 *   int32_t gj_steam_analog_deadzone_5559(int32_t x_q15, int32_t dz_q15,
 *                                         int32_t *out_y, int32_t y_q15);
 *     - Radial deadzone on (x,y). If magnitude <= |dz|, zeros both and
 *       stores 0 in *out_y (when non-NULL). Else rescales so the dead
 *       radius maps to 0 and full Q15 radius maps to Q15 max.
 *       Returns scaled x; writes scaled y to *out_y when non-NULL.
 *   int32_t gj_steam_analog_axis_dz_5559(int32_t v_q15, int32_t dz_q15);
 *     - 1D axis deadzone with rescaling to Q15 extremes.
 *   uint32_t gj_batch_id_5559(void);
 *   __ aliases
 *   __libcgj_batch5559_marker = "libcgj-batch5559"
 *
 * Exclusive continuum CREATE-ONLY (5551-5560: controller layouts —
 * glyphs, gyro calib, trackpad; steam input stubs). Unique *_5559
 * surfaces only; no multi-def. No parent wires. No __int128.
 * Not affiliated with Valve; clean-room integer deadzone only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5559_marker[] = "libcgj-batch5559";

#define B5559_BATCH_ID 5559u
#define B5559_Q15_MAX  32767
#define B5559_Q15_MIN  (-32768)

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b5559_abs_i32(int32_t i32V)
{
	if (i32V == INT32_MIN) {
		return INT32_MAX;
	}
	return (i32V < 0) ? -i32V : i32V;
}

static int32_t
b5559_sat_q15(int64_t i64V)
{
	if (i64V > (int64_t)B5559_Q15_MAX) {
		return B5559_Q15_MAX;
	}
	if (i64V < (int64_t)B5559_Q15_MIN) {
		return B5559_Q15_MIN;
	}
	return (int32_t)i64V;
}

static uint32_t
b5559_isqrt_u64(uint64_t u64N)
{
	uint64_t u64Lo;
	uint64_t u64Hi;
	uint64_t u64Mid;
	uint64_t u64Sq;

	if (u64N <= 1ull) {
		return (uint32_t)u64N;
	}
	u64Lo = 1ull;
	u64Hi = (u64N < 0xffffffffull) ? u64N : 0xffffffffull;
	while (u64Lo < u64Hi) {
		u64Mid = u64Lo + ((u64Hi - u64Lo + 1ull) >> 1);
		u64Sq = u64Mid * u64Mid;
		if (u64Sq <= u64N) {
			u64Lo = u64Mid;
		} else {
			u64Hi = u64Mid - 1ull;
		}
	}
	return (uint32_t)u64Lo;
}

static int32_t
b5559_axis_dz(int32_t i32V, int32_t i32Dz)
{
	int32_t i32Adz;
	int32_t i32Av;
	int32_t i32Sign;
	int64_t i64Num;
	int64_t i64Den;

	i32Adz = b5559_abs_i32(i32Dz);
	if (i32Adz >= B5559_Q15_MAX) {
		return 0;
	}
	i32Av = b5559_abs_i32(i32V);
	if (i32Av <= i32Adz) {
		return 0;
	}
	i32Sign = (i32V < 0) ? -1 : 1;
	i64Den = (int64_t)B5559_Q15_MAX - (int64_t)i32Adz;
	if (i64Den <= 0) {
		return 0;
	}
	i64Num = (int64_t)(i32Av - i32Adz) * (int64_t)B5559_Q15_MAX;
	return b5559_sat_q15((i64Num / i64Den) * (int64_t)i32Sign);
}

static int32_t
b5559_radial(int32_t i32X, int32_t i32Dz, int32_t *pOutY, int32_t i32Y)
{
	int32_t i32Adz;
	uint32_t u32Mag;
	int64_t i64R2;
	int64_t i64Den;
	int64_t i64Scale;
	int64_t i64Sx;
	int64_t i64Sy;
	int32_t i32Nx;
	int32_t i32Ny;

	i32Adz = b5559_abs_i32(i32Dz);
	i64R2 = (int64_t)i32X * (int64_t)i32X
	    + (int64_t)i32Y * (int64_t)i32Y;
	u32Mag = b5559_isqrt_u64((uint64_t)i64R2);

	if (u32Mag <= (uint32_t)i32Adz || u32Mag == 0u) {
		if (pOutY != NULL) {
			*pOutY = 0;
		}
		return 0;
	}

	/* Rescale: (mag - dz) / (Q15_MAX - dz) * Q15_MAX on each axis. */
	i64Den = (int64_t)B5559_Q15_MAX - (int64_t)i32Adz;
	if (i64Den <= 0) {
		if (pOutY != NULL) {
			*pOutY = 0;
		}
		return 0;
	}

	i64Scale = ((int64_t)(u32Mag - (uint32_t)i32Adz)
	    * (int64_t)B5559_Q15_MAX) / i64Den;

	/* Unit direction * new magnitude. */
	i64Sx = ((int64_t)i32X * i64Scale) / (int64_t)u32Mag;
	i64Sy = ((int64_t)i32Y * i64Scale) / (int64_t)u32Mag;
	i32Nx = b5559_sat_q15(i64Sx);
	i32Ny = b5559_sat_q15(i64Sy);

	if (pOutY != NULL) {
		*pOutY = i32Ny;
	}
	return i32Nx;
}

/* ---- public surface ---------------------------------------------------- */

int32_t
gj_steam_analog_deadzone_5559(int32_t x_q15, int32_t dz_q15, int32_t *out_y,
    int32_t y_q15)
{
	(void)NULL;
	return b5559_radial(x_q15, dz_q15, out_y, y_q15);
}

int32_t
gj_steam_analog_axis_dz_5559(int32_t v_q15, int32_t dz_q15)
{
	return b5559_axis_dz(v_q15, dz_q15);
}

uint32_t
gj_batch_id_5559(void)
{
	return B5559_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

int32_t __gj_steam_analog_deadzone_5559(int32_t x_q15, int32_t dz_q15,
    int32_t *out_y, int32_t y_q15)
    __attribute__((alias("gj_steam_analog_deadzone_5559")));

int32_t __gj_steam_analog_axis_dz_5559(int32_t v_q15, int32_t dz_q15)
    __attribute__((alias("gj_steam_analog_axis_dz_5559")));

uint32_t __gj_batch_id_5559(void)
    __attribute__((alias("gj_batch_id_5559")));
