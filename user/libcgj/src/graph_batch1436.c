/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1436: freestanding weekday from serial day.
 *
 * Surface (unique symbols):
 *   int gj_weekday_from_days(int64_t i64Z);
 *     — Weekday for serial day i64Z (days since civil 1970-01-01).
 *       Encoding: 0 = Monday, 1 = Tuesday, …, 6 = Sunday (ISO 8601).
 *       1970-01-01 was a Thursday → gj_weekday_from_days(0) == 3.
 *       Negative z is handled with a positive Euclidean remainder.
 *   int __gj_weekday_from_days  (alias)
 *   __libcgj_batch1436_marker = "libcgj-batch1436"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1436_marker[] = "libcgj-batch1436";

/*
 * b1436_mod7 — Euclidean remainder in 0..6 for any int64_t.
 * C99 % truncates toward zero; adjust negative remainders.
 */
static int
b1436_mod7(int64_t i64X)
{
	int64_t i64R;

	i64R = i64X % 7;
	if (i64R < 0) {
		i64R += 7;
	}
	return (int)i64R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_weekday_from_days — ISO weekday from days since 1970-01-01.
 *
 * i64Z: serial day (0 = 1970-01-01 Thursday)
 *
 * Returns 0..6 where 0 = Monday and 6 = Sunday.
 * Examples: 0 → 3 (Thu), 1 → 4 (Fri), -1 → 2 (Wed), 3 → 6 (Sun), 4 → 0 (Mon).
 */
int
gj_weekday_from_days(int64_t i64Z)
{
	/* Thursday = 3 when Monday = 0 → offset +3 from epoch day. */
	return b1436_mod7(i64Z + 3);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_weekday_from_days(int64_t i64Z)
    __attribute__((alias("gj_weekday_from_days")));
