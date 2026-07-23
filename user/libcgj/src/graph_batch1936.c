/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1936: fan RPM soft-ok hint.
 *
 * Surface (unique symbols):
 *   int gj_fan_rpm_ok_hint(int rpm);
 *     — Return 1 if rpm is in the soft-valid fan speed range
 *       [0, 30000], else 0. Pure-data catalog-free bound only.
 *   int __gj_fan_rpm_ok_hint  (alias)
 *   __libcgj_batch1936_marker = "libcgj-batch1936"
 *
 * Power/thermal exclusive pure-data wave (1931–1940). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1936_marker[] = "libcgj-batch1936";

/* Soft fan RPM range covering stalled through high-speed chassis fans. */
#define B1936_RPM_MIN  0
#define B1936_RPM_MAX  30000

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1936_rpm_ok(int nRpm)
{
	if (nRpm < B1936_RPM_MIN) {
		return 0;
	}
	if (nRpm > B1936_RPM_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fan_rpm_ok_hint — 1 iff rpm is in [0, 30000].
 *
 * rpm: fan revolutions per minute (signed so negatives reject)
 * Returns 1 when in soft range, else 0.
 */
int
gj_fan_rpm_ok_hint(int nRpm)
{
	(void)NULL;
	return b1936_rpm_ok(nRpm);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fan_rpm_ok_hint(int nRpm)
    __attribute__((alias("gj_fan_rpm_ok_hint")));
