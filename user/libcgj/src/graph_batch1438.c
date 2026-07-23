/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1438: freestanding h/m/s → total seconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_hms_to_secs(uint64_t uH, unsigned uM, unsigned uS);
 *     — Combine hours, minutes, and seconds into a total second count:
 *       uH * 3600 + uM * 60 + uS. Saturates at UINT64_MAX on overflow.
 *       If uM >= 60 or uS >= 60, returns 0 (invalid field).
 *       Hours are not limited to 0..23 (duration-style).
 *   uint64_t __gj_hms_to_secs  (alias)
 *   __libcgj_batch1438_marker = "libcgj-batch1438"
 *
 * Inverse companion to gj_secs_to_hms (batch1437). Distinct from
 * gj_ms_to_secs (batch485).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1438_marker[] = "libcgj-batch1438";

/* Floor(UINT64_MAX / 3600): largest hours with h*3600 still in range. */
#define B1438_MAX_H  (UINT64_C(0xffffffffffffffff) / 3600u)

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hms_to_secs — hours/minutes/seconds to total seconds.
 *
 * uH: hours (unrestricted range; duration-style)
 * uM: minutes 0..59
 * uS: seconds 0..59
 *
 * Returns total seconds, or 0 if uM/uS out of range. Saturates to
 * UINT64_MAX if the product would overflow.
 * Examples: (1,1,1) → 3661; (0,0,0) → 0; (0,60,0) → 0 (invalid).
 */
uint64_t
gj_hms_to_secs(uint64_t uH, unsigned uM, unsigned uS)
{
	uint64_t uBase;
	uint64_t uAdd;

	if (uM >= 60u || uS >= 60u) {
		return 0u;
	}

	if (uH > B1438_MAX_H) {
		return UINT64_C(0xffffffffffffffff);
	}

	uBase = uH * 3600u;
	uAdd = (uint64_t)uM * 60u + (uint64_t)uS;
	if (uBase > UINT64_C(0xffffffffffffffff) - uAdd) {
		return UINT64_C(0xffffffffffffffff);
	}
	return uBase + uAdd;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hms_to_secs(uint64_t uH, unsigned uM, unsigned uS)
    __attribute__((alias("gj_hms_to_secs")));
