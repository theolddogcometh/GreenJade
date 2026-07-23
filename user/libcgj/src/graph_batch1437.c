/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1437: freestanding seconds → h/m/s fields.
 *
 * Surface (unique symbols):
 *   int gj_secs_to_hms(uint64_t uSecs, uint64_t *pH, unsigned *pM,
 *                      unsigned *pS);
 *     — Decompose a non-negative second count into hours, minutes, and
 *       seconds. Hours are NOT reduced mod 24 (duration-style). Minutes
 *       and seconds are always in 0..59. NULL out-params → -1, else 0.
 *   int __gj_secs_to_hms  (alias)
 *   __libcgj_batch1437_marker = "libcgj-batch1437"
 *
 * Distinct from gj_secs_to_ms (batch484). No libc time APIs.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>
#include <stddef.h>

const char __libcgj_batch1437_marker[] = "libcgj-batch1437";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_secs_to_hms — split seconds into hours / minutes / seconds.
 *
 * uSecs: total seconds (unsigned)
 * pH: out hours (may exceed 23)
 * pM: out minutes 0..59
 * pS: out seconds 0..59
 *
 * Returns 0 on success, -1 if any out pointer is NULL.
 * Examples: 0 → 0:0:0; 3661 → 1:1:1; 86400 → 24:0:0.
 */
int
gj_secs_to_hms(uint64_t uSecs, uint64_t *pH, unsigned *pM, unsigned *pS)
{
	uint64_t uRem;

	if (pH == NULL || pM == NULL || pS == NULL) {
		return -1;
	}

	*pH = uSecs / 3600u;
	uRem = uSecs % 3600u;
	*pM = (unsigned)(uRem / 60u);
	*pS = (unsigned)(uRem % 60u);
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_secs_to_hms(uint64_t uSecs, uint64_t *pH, unsigned *pM, unsigned *pS)
    __attribute__((alias("gj_secs_to_hms")));
