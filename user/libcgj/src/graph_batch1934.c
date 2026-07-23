/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1934: critical Celsius temperature hint.
 *
 * Surface (unique symbols):
 *   int gj_temp_c_is_critical_hint(int temp_c);
 *     — Return 1 if temp_c is greater than or equal to 100 degrees
 *       Celsius, else 0. Soft thermal critical bound only.
 *   int __gj_temp_c_is_critical_hint  (alias)
 *   __libcgj_batch1934_marker = "libcgj-batch1934"
 *
 * Power/thermal exclusive pure-data wave (1931–1940). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1934_marker[] = "libcgj-batch1934";

/* Soft critical temperature threshold in Celsius. */
#define B1934_CRIT_C  100

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1934_is_critical(int nTempC)
{
	return (nTempC >= B1934_CRIT_C) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_temp_c_is_critical_hint — 1 if temp_c >= 100.
 *
 * temp_c: temperature in whole degrees Celsius
 * Returns 1 when at or above the soft critical bound, else 0.
 */
int
gj_temp_c_is_critical_hint(int nTempC)
{
	(void)NULL;
	return b1934_is_critical(nTempC);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_temp_c_is_critical_hint(int nTempC)
    __attribute__((alias("gj_temp_c_is_critical_hint")));
