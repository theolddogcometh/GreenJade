/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5553: gyro bias calibration (integer mean).
 *
 * Surface (unique symbols):
 *   int32_t gj_gyro_bias_update_5553(int32_t bias, int32_t sample,
 *                                    uint32_t n);
 *     - Online mean update: bias' = bias + (sample - bias) / n
 *       with n clamped to [1, 65536]. Integer truncation toward 0.
 *   int32_t gj_gyro_bias_apply_5553(int32_t raw, int32_t bias);
 *     - raw - bias with saturating int32 arithmetic.
 *   uint32_t gj_batch_id_5553(void);
 *   __ aliases
 *   __libcgj_batch5553_marker = "libcgj-batch5553"
 *
 * Exclusive continuum CREATE-ONLY (5551-5560: controller layouts —
 * glyphs, gyro calib, trackpad; steam input stubs). Unique *_5553
 * surfaces only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5553_marker[] = "libcgj-batch5553";

#define B5553_BATCH_ID 5553u
#define B5553_N_MAX    65536u

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b5553_sat_sub(int32_t i32A, int32_t i32B)
{
	int64_t i64R;

	i64R = (int64_t)i32A - (int64_t)i32B;
	if (i64R > (int64_t)INT32_MAX) {
		return INT32_MAX;
	}
	if (i64R < (int64_t)INT32_MIN) {
		return INT32_MIN;
	}
	return (int32_t)i64R;
}

static int32_t
b5553_bias_update(int32_t i32Bias, int32_t i32Sample, uint32_t u32N)
{
	int64_t i64Delta;
	int64_t i64Step;

	if (u32N == 0u) {
		u32N = 1u;
	}
	if (u32N > B5553_N_MAX) {
		u32N = B5553_N_MAX;
	}

	i64Delta = (int64_t)i32Sample - (int64_t)i32Bias;
	/* Truncate toward zero. */
	i64Step = i64Delta / (int64_t)u32N;
	i64Delta = (int64_t)i32Bias + i64Step;
	if (i64Delta > (int64_t)INT32_MAX) {
		return INT32_MAX;
	}
	if (i64Delta < (int64_t)INT32_MIN) {
		return INT32_MIN;
	}
	return (int32_t)i64Delta;
}

/* ---- public surface ---------------------------------------------------- */

int32_t
gj_gyro_bias_update_5553(int32_t bias, int32_t sample, uint32_t n)
{
	(void)NULL;
	return b5553_bias_update(bias, sample, n);
}

int32_t
gj_gyro_bias_apply_5553(int32_t raw, int32_t bias)
{
	return b5553_sat_sub(raw, bias);
}

uint32_t
gj_batch_id_5553(void)
{
	return B5553_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

int32_t __gj_gyro_bias_update_5553(int32_t bias, int32_t sample, uint32_t n)
    __attribute__((alias("gj_gyro_bias_update_5553")));

int32_t __gj_gyro_bias_apply_5553(int32_t raw, int32_t bias)
    __attribute__((alias("gj_gyro_bias_apply_5553")));

uint32_t __gj_batch_id_5553(void)
    __attribute__((alias("gj_batch_id_5553")));
