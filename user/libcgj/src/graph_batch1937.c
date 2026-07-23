/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1937: TDP milliwatt soft-ok hint.
 *
 * Surface (unique symbols):
 *   int gj_tdp_mw_ok_hint(int tdp_mw);
 *     — Return 1 if tdp_mw is in the soft-valid thermal design power
 *       range [1, 1000000] milliwatts (1 mW through 1000 W), else 0.
 *   int __gj_tdp_mw_ok_hint  (alias)
 *   __libcgj_batch1937_marker = "libcgj-batch1937"
 *
 * Power/thermal exclusive pure-data wave (1931–1940). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1937_marker[] = "libcgj-batch1937";

/* Soft TDP range in milliwatts. */
#define B1937_TDP_MW_MIN  1
#define B1937_TDP_MW_MAX  1000000

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1937_tdp_ok(int nTdpMw)
{
	if (nTdpMw < B1937_TDP_MW_MIN) {
		return 0;
	}
	if (nTdpMw > B1937_TDP_MW_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tdp_mw_ok_hint — 1 iff tdp_mw is in [1, 1000000].
 *
 * tdp_mw: thermal design power in milliwatts
 * Returns 1 when in soft range, else 0.
 */
int
gj_tdp_mw_ok_hint(int nTdpMw)
{
	(void)NULL;
	return b1937_tdp_ok(nTdpMw);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_tdp_mw_ok_hint(int nTdpMw)
    __attribute__((alias("gj_tdp_mw_ok_hint")));
