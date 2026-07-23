/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2296: product smoke result band soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_smoke_band_ok(int code);
 *     - Return 1 if code is in the expanded product smoke band [0, 5]
 *       (PASS through soft-warn and extended skip style codes), else 0.
 *       Negatives and codes > 5 fail. Distinct from
 *       gj_smoke_gate_soft_p (batch1995 band [0, 3]).
 *   int __gj_smoke_band_ok  (alias)
 *   __libcgj_batch2296_marker = "libcgj-batch2296"
 *
 * Milestone 2300 exclusive product helpers (2291-2300). Unique
 * gj_smoke_band_ok surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2296_marker[] = "libcgj-batch2296";

/* Inclusive upper bound of expanded product smoke band. */
#define B2296_BAND_MAX  5

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if code is in soft band [0, B2296_BAND_MAX]. */
static int
b2296_band_ok(int nCode)
{
	if (nCode < 0) {
		return 0;
	}
	if (nCode > B2296_BAND_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_smoke_band_ok - 1 iff smoke code is in the product smoke band.
 *
 * code: integer smoke result code (signed so negatives reject).
 * Band is [0, 5]. Distinct from gj_smoke_gate_soft_p ([0, 3]) and
 * gj_smoke_hard_fail_p. Does not call libc.
 */
int
gj_smoke_band_ok(int nCode)
{
	(void)NULL;
	return b2296_band_ok(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_smoke_band_ok(int nCode)
    __attribute__((alias("gj_smoke_band_ok")));
