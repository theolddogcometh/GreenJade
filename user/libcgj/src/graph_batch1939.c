/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1939: CPU clock MHz soft-ok predicate.
 *
 * Surface (unique symbols):
 *   int gj_cpu_clock_mhz_ok(int mhz);
 *     — Return 1 if mhz is in the soft-valid CPU core clock range
 *       [1, 10000] MHz, else 0. Pure-data bound only.
 *   int __gj_cpu_clock_mhz_ok  (alias)
 *   __libcgj_batch1939_marker = "libcgj-batch1939"
 *
 * Power/thermal exclusive pure-data wave (1931–1940). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1939_marker[] = "libcgj-batch1939";

/* Soft CPU clock range in megahertz. */
#define B1939_MHZ_MIN  1
#define B1939_MHZ_MAX  10000

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1939_cpu_mhz_ok(int nMhz)
{
	if (nMhz < B1939_MHZ_MIN) {
		return 0;
	}
	if (nMhz > B1939_MHZ_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cpu_clock_mhz_ok — 1 iff mhz is in [1, 10000].
 *
 * mhz: CPU clock frequency in megahertz
 * Returns 1 when in soft range, else 0.
 */
int
gj_cpu_clock_mhz_ok(int nMhz)
{
	(void)NULL;
	return b1939_cpu_mhz_ok(nMhz);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cpu_clock_mhz_ok(int nMhz)
    __attribute__((alias("gj_cpu_clock_mhz_ok")));
