/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1938: GPU clock MHz soft-ok predicate.
 *
 * Surface (unique symbols):
 *   int gj_gpu_clock_mhz_ok(int mhz);
 *     — Return 1 if mhz is in the soft-valid GPU core/memory clock
 *       range [1, 5000] MHz, else 0. Pure-data bound only.
 *   int __gj_gpu_clock_mhz_ok  (alias)
 *   __libcgj_batch1938_marker = "libcgj-batch1938"
 *
 * Power/thermal exclusive pure-data wave (1931–1940). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1938_marker[] = "libcgj-batch1938";

/* Soft GPU clock range in megahertz. */
#define B1938_MHZ_MIN  1
#define B1938_MHZ_MAX  5000

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1938_gpu_mhz_ok(int nMhz)
{
	if (nMhz < B1938_MHZ_MIN) {
		return 0;
	}
	if (nMhz > B1938_MHZ_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_clock_mhz_ok — 1 iff mhz is in [1, 5000].
 *
 * mhz: GPU clock frequency in megahertz
 * Returns 1 when in soft range, else 0.
 */
int
gj_gpu_clock_mhz_ok(int nMhz)
{
	(void)NULL;
	return b1938_gpu_mhz_ok(nMhz);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_gpu_clock_mhz_ok(int nMhz)
    __attribute__((alias("gj_gpu_clock_mhz_ok")));
