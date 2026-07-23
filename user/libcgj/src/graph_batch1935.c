/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1935: hot Celsius temperature hint.
 *
 * Surface (unique symbols):
 *   int gj_temp_c_is_hot_hint(int temp_c);
 *     — Return 1 if temp_c is greater than or equal to 80 degrees
 *       Celsius, else 0. Soft thermal hot bound only.
 *   int __gj_temp_c_is_hot_hint  (alias)
 *   __libcgj_batch1935_marker = "libcgj-batch1935"
 *
 * Power/thermal exclusive pure-data wave (1931–1940). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1935_marker[] = "libcgj-batch1935";

/* Soft hot temperature threshold in Celsius. */
#define B1935_HOT_C  80

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1935_is_hot(int nTempC)
{
	return (nTempC >= B1935_HOT_C) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_temp_c_is_hot_hint — 1 if temp_c >= 80.
 *
 * temp_c: temperature in whole degrees Celsius
 * Returns 1 when at or above the soft hot bound, else 0.
 */
int
gj_temp_c_is_hot_hint(int nTempC)
{
	(void)NULL;
	return b1935_is_hot(nTempC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_temp_c_is_hot_hint(int nTempC)
    __attribute__((alias("gj_temp_c_is_hot_hint")));
