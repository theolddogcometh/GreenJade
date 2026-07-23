/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5555: trackpad axis normalize to Q15.
 *
 * Surface (unique symbols):
 *   int32_t gj_trackpad_norm_axis_5555(int32_t raw, int32_t raw_min,
 *                                      int32_t raw_max);
 *     - Map raw in [raw_min, raw_max] to signed Q15 [-32768, 32767].
 *       Degenerate span → 0. Out-of-range raw is clamped first.
 *   int32_t gj_trackpad_circ_clamp_5555(int32_t x_q15, int32_t y_q15,
 *                                       int32_t *out_y_q15);
 *     - If x^2+y^2 exceeds Q15 unit circle radius^2, scale both axes
 *       down (integer). Writes scaled y to *out_y_q15 when non-NULL.
 *       Returns scaled x. NULL out_y → y still computed but discarded.
 *   uint32_t gj_batch_id_5555(void);
 *   __ aliases
 *   __libcgj_batch5555_marker = "libcgj-batch5555"
 *
 * Exclusive continuum CREATE-ONLY (5551-5560: controller layouts —
 * glyphs, gyro calib, trackpad; steam input stubs). Unique *_5555
 * surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5555_marker[] = "libcgj-batch5555";

#define B5555_BATCH_ID  5555u
#define B5555_Q15_MAX   32767
#define B5555_Q15_MIN (-32768)
#define B5555_Q15_RAD2  ((int64_t)32767 * (int64_t)32767)

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b5555_clamp_raw(int32_t i32V, int32_t i32Lo, int32_t i32Hi)
{
	if (i32Lo > i32Hi) {
		int32_t i32T = i32Lo;
		i32Lo = i32Hi;
		i32Hi = i32T;
	}
	if (i32V < i32Lo) {
		return i32Lo;
	}
	if (i32V > i32Hi) {
		return i32Hi;
	}
	return i32V;
}

static int32_t
b5555_norm_axis(int32_t i32Raw, int32_t i32Min, int32_t i32Max)
{
	int32_t i32Lo;
	int32_t i32Hi;
	int32_t i32Span;
	int32_t i32Mid;
	int64_t i64Off;
	int64_t i64Num;
	int64_t i64Den;
	int64_t i64R;

	if (i32Min == i32Max) {
		return 0;
	}
	if (i32Min < i32Max) {
		i32Lo = i32Min;
		i32Hi = i32Max;
	} else {
		i32Lo = i32Max;
		i32Hi = i32Min;
	}

	i32Raw = b5555_clamp_raw(i32Raw, i32Lo, i32Hi);
	i32Span = i32Hi - i32Lo;
	i32Mid = i32Lo + (i32Span / 2);

	/* Map [lo,hi] → [-32768, 32767] via centered offset. */
	i64Off = (int64_t)i32Raw - (int64_t)i32Mid;
	i64Den = (int64_t)i32Span;
	i64Num = i64Off * (int64_t)(B5555_Q15_MAX - B5555_Q15_MIN);
	i64R = i64Num / i64Den;

	if (i64R > (int64_t)B5555_Q15_MAX) {
		return B5555_Q15_MAX;
	}
	if (i64R < (int64_t)B5555_Q15_MIN) {
		return B5555_Q15_MIN;
	}
	return (int32_t)i64R;
}

/* Floor isqrt for uint64 (root fits in 32 bits for our radii). */
static uint32_t
b5555_isqrt_u64(uint64_t u64N)
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
b5555_circ_clamp(int32_t i32X, int32_t i32Y, int32_t *pOutY)
{
	int64_t i64Xx;
	int64_t i64Yy;
	int64_t i64R2;
	uint32_t u32R;
	uint32_t u32Lim;
	int64_t i64Sx;
	int64_t i64Sy;

	i64Xx = (int64_t)i32X * (int64_t)i32X;
	i64Yy = (int64_t)i32Y * (int64_t)i32Y;
	i64R2 = i64Xx + i64Yy;

	if (i64R2 <= B5555_Q15_RAD2) {
		if (pOutY != NULL) {
			*pOutY = i32Y;
		}
		return i32X;
	}

	u32R = b5555_isqrt_u64((uint64_t)i64R2);
	u32Lim = (uint32_t)B5555_Q15_MAX;
	if (u32R == 0u) {
		if (pOutY != NULL) {
			*pOutY = 0;
		}
		return 0;
	}

	i64Sx = ((int64_t)i32X * (int64_t)u32Lim) / (int64_t)u32R;
	i64Sy = ((int64_t)i32Y * (int64_t)u32Lim) / (int64_t)u32R;

	if (i64Sx > (int64_t)B5555_Q15_MAX) {
		i64Sx = B5555_Q15_MAX;
	}
	if (i64Sx < (int64_t)B5555_Q15_MIN) {
		i64Sx = B5555_Q15_MIN;
	}
	if (i64Sy > (int64_t)B5555_Q15_MAX) {
		i64Sy = B5555_Q15_MAX;
	}
	if (i64Sy < (int64_t)B5555_Q15_MIN) {
		i64Sy = B5555_Q15_MIN;
	}

	if (pOutY != NULL) {
		*pOutY = (int32_t)i64Sy;
	}
	return (int32_t)i64Sx;
}

/* ---- public surface ---------------------------------------------------- */

int32_t
gj_trackpad_norm_axis_5555(int32_t raw, int32_t raw_min, int32_t raw_max)
{
	(void)NULL;
	return b5555_norm_axis(raw, raw_min, raw_max);
}

int32_t
gj_trackpad_circ_clamp_5555(int32_t x_q15, int32_t y_q15, int32_t *out_y_q15)
{
	return b5555_circ_clamp(x_q15, y_q15, out_y_q15);
}

uint32_t
gj_batch_id_5555(void)
{
	return B5555_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

int32_t __gj_trackpad_norm_axis_5555(int32_t raw, int32_t raw_min,
    int32_t raw_max)
    __attribute__((alias("gj_trackpad_norm_axis_5555")));

int32_t __gj_trackpad_circ_clamp_5555(int32_t x_q15, int32_t y_q15,
    int32_t *out_y_q15)
    __attribute__((alias("gj_trackpad_circ_clamp_5555")));

uint32_t __gj_batch_id_5555(void)
    __attribute__((alias("gj_batch_id_5555")));
