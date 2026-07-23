/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5554: gyro scale, deadzone, and clamp.
 *
 * Surface (unique symbols):
 *   int32_t gj_gyro_scale_5554(int32_t sample, uint32_t scale_q16);
 *     - sample * scale_q16 / 65536 with saturating int32 result.
 *   int32_t gj_gyro_deadzone_5554(int32_t sample, int32_t dz);
 *     - Zero output when |sample| <= |dz|; else sample unchanged.
 *   int32_t gj_gyro_clamp_5554(int32_t sample, int32_t lim);
 *     - Clamp sample to [-|lim|, +|lim|]. lim==0 → 0.
 *   uint32_t gj_batch_id_5554(void);
 *   __ aliases
 *   __libcgj_batch5554_marker = "libcgj-batch5554"
 *
 * Exclusive continuum CREATE-ONLY (5551-5560: controller layouts —
 * glyphs, gyro calib, trackpad; steam input stubs). Unique *_5554
 * surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5554_marker[] = "libcgj-batch5554";

#define B5554_BATCH_ID 5554u
#define B5554_Q16_ONE  65536u

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b5554_abs_i32(int32_t i32V)
{
	if (i32V == INT32_MIN) {
		return INT32_MAX;
	}
	return (i32V < 0) ? -i32V : i32V;
}

static int32_t
b5554_sat_from_i64(int64_t i64V)
{
	if (i64V > (int64_t)INT32_MAX) {
		return INT32_MAX;
	}
	if (i64V < (int64_t)INT32_MIN) {
		return INT32_MIN;
	}
	return (int32_t)i64V;
}

static int32_t
b5554_scale(int32_t i32Sample, uint32_t u32ScaleQ16)
{
	int64_t i64P;

	i64P = (int64_t)i32Sample * (int64_t)u32ScaleQ16;
	/* Arithmetic right-ish divide by Q16; toward zero via cast path. */
	if (i64P >= 0) {
		return b5554_sat_from_i64(i64P / (int64_t)B5554_Q16_ONE);
	}
	return b5554_sat_from_i64(-((-i64P) / (int64_t)B5554_Q16_ONE));
}

static int32_t
b5554_deadzone(int32_t i32Sample, int32_t i32Dz)
{
	int32_t i32Adz;

	i32Adz = b5554_abs_i32(i32Dz);
	if (b5554_abs_i32(i32Sample) <= i32Adz) {
		return 0;
	}
	return i32Sample;
}

static int32_t
b5554_clamp(int32_t i32Sample, int32_t i32Lim)
{
	int32_t i32A;

	i32A = b5554_abs_i32(i32Lim);
	if (i32A == 0) {
		return 0;
	}
	if (i32Sample > i32A) {
		return i32A;
	}
	if (i32Sample < -i32A) {
		return -i32A;
	}
	return i32Sample;
}

/* ---- public surface ---------------------------------------------------- */

int32_t
gj_gyro_scale_5554(int32_t sample, uint32_t scale_q16)
{
	(void)NULL;
	return b5554_scale(sample, scale_q16);
}

int32_t
gj_gyro_deadzone_5554(int32_t sample, int32_t dz)
{
	return b5554_deadzone(sample, dz);
}

int32_t
gj_gyro_clamp_5554(int32_t sample, int32_t lim)
{
	return b5554_clamp(sample, lim);
}

uint32_t
gj_batch_id_5554(void)
{
	return B5554_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

int32_t __gj_gyro_scale_5554(int32_t sample, uint32_t scale_q16)
    __attribute__((alias("gj_gyro_scale_5554")));

int32_t __gj_gyro_deadzone_5554(int32_t sample, int32_t dz)
    __attribute__((alias("gj_gyro_deadzone_5554")));

int32_t __gj_gyro_clamp_5554(int32_t sample, int32_t lim)
    __attribute__((alias("gj_gyro_clamp_5554")));

uint32_t __gj_batch_id_5554(void)
    __attribute__((alias("gj_batch_id_5554")));
