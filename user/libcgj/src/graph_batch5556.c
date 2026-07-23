/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5556: trackpad region / quadrant hit-test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_trackpad_quadrant_5556(int32_t x_q15, int32_t y_q15);
 *     - 0=center (inside dead radius 4096), else 1=NE 2=NW 3=SW 4=SE
 *       by sign of (x,y) with y-up convention (positive y = up).
 *   int gj_trackpad_in_circle_5556(int32_t x_q15, int32_t y_q15,
 *                                  int32_t cx, int32_t cy, uint32_t r);
 *     - 1 if (x-cx)^2+(y-cy)^2 <= r^2, else 0. r==0 → only exact center.
 *   uint32_t gj_trackpad_click_zone_5556(int32_t x_q15, int32_t y_q15);
 *     - Soft zones: 0 none, 1 left half click, 2 right half click,
 *       3 both (near center pad press band |y| < 8192).
 *   uint32_t gj_batch_id_5556(void);
 *   __ aliases
 *   __libcgj_batch5556_marker = "libcgj-batch5556"
 *
 * Exclusive continuum CREATE-ONLY (5551-5560: controller layouts —
 * glyphs, gyro calib, trackpad; steam input stubs). Unique *_5556
 * surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5556_marker[] = "libcgj-batch5556";

#define B5556_BATCH_ID   5556u
#define B5556_DEAD_R     4096
#define B5556_CLICK_BAND 8192

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b5556_abs_i32(int32_t i32V)
{
	if (i32V == INT32_MIN) {
		return INT32_MAX;
	}
	return (i32V < 0) ? -i32V : i32V;
}

static uint32_t
b5556_quadrant(int32_t i32X, int32_t i32Y)
{
	int64_t i64R2;

	i64R2 = (int64_t)i32X * (int64_t)i32X
	    + (int64_t)i32Y * (int64_t)i32Y;
	if (i64R2 <= (int64_t)B5556_DEAD_R * (int64_t)B5556_DEAD_R) {
		return 0u;
	}

	/* y-up: +y north. */
	if (i32X >= 0 && i32Y >= 0) {
		return 1u; /* NE */
	}
	if (i32X < 0 && i32Y >= 0) {
		return 2u; /* NW */
	}
	if (i32X < 0 && i32Y < 0) {
		return 3u; /* SW */
	}
	return 4u; /* SE */
}

static int
b5556_in_circle(int32_t i32X, int32_t i32Y, int32_t i32Cx, int32_t i32Cy,
    uint32_t u32R)
{
	int64_t i64Dx;
	int64_t i64Dy;
	int64_t i64R2;
	int64_t i64Lim;

	i64Dx = (int64_t)i32X - (int64_t)i32Cx;
	i64Dy = (int64_t)i32Y - (int64_t)i32Cy;
	i64R2 = i64Dx * i64Dx + i64Dy * i64Dy;
	i64Lim = (int64_t)u32R * (int64_t)u32R;
	return (i64R2 <= i64Lim) ? 1 : 0;
}

static uint32_t
b5556_click_zone(int32_t i32X, int32_t i32Y)
{
	int32_t i32Ay;

	i32Ay = b5556_abs_i32(i32Y);
	if (i32Ay >= B5556_CLICK_BAND) {
		/* Outer vertical band: left/right only by x sign. */
		if (i32X < 0) {
			return 1u;
		}
		if (i32X > 0) {
			return 2u;
		}
		return 0u;
	}

	/* Center horizontal band — both half-press zone. */
	if (b5556_abs_i32(i32X) <= B5556_DEAD_R) {
		return 3u;
	}
	if (i32X < 0) {
		return 1u;
	}
	return 2u;
}

/* ---- public surface ---------------------------------------------------- */

uint32_t
gj_trackpad_quadrant_5556(int32_t x_q15, int32_t y_q15)
{
	(void)NULL;
	return b5556_quadrant(x_q15, y_q15);
}

int
gj_trackpad_in_circle_5556(int32_t x_q15, int32_t y_q15, int32_t cx,
    int32_t cy, uint32_t r)
{
	return b5556_in_circle(x_q15, y_q15, cx, cy, r);
}

uint32_t
gj_trackpad_click_zone_5556(int32_t x_q15, int32_t y_q15)
{
	return b5556_click_zone(x_q15, y_q15);
}

uint32_t
gj_batch_id_5556(void)
{
	return B5556_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_trackpad_quadrant_5556(int32_t x_q15, int32_t y_q15)
    __attribute__((alias("gj_trackpad_quadrant_5556")));

int __gj_trackpad_in_circle_5556(int32_t x_q15, int32_t y_q15, int32_t cx,
    int32_t cy, uint32_t r)
    __attribute__((alias("gj_trackpad_in_circle_5556")));

uint32_t __gj_trackpad_click_zone_5556(int32_t x_q15, int32_t y_q15)
    __attribute__((alias("gj_trackpad_click_zone_5556")));

uint32_t __gj_batch_id_5556(void)
    __attribute__((alias("gj_batch_id_5556")));
