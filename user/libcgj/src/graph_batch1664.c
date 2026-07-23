/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1664: nice-value clamp.
 *
 * Surface (unique symbols):
 *   int32_t gj_nice_clamp(int32_t nice);
 *     — Clamp nice into the portable range [-20, 19]. Values below
 *       -20 become -20; values above 19 become 19.
 *   int32_t __gj_nice_clamp  (alias)
 *   __libcgj_batch1664_marker = "libcgj-batch1664"
 *
 * Distinct from gj_timeslice_from_nice (batch1615) which scales a
 * quantum. Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1664_marker[] = "libcgj-batch1664";

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b1664_clamp(int32_t iNice)
{
	if (iNice < -20) {
		return -20;
	}
	if (iNice > 19) {
		return 19;
	}
	return iNice;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nice_clamp — portable nice-range clamp for pure data paths.
 *
 * Matches classic Unix nice bounds used by setpriority/getpriority
 * consumers. No scheduler objects, no syscalls.
 */
int32_t
gj_nice_clamp(int32_t iNice)
{
	return b1664_clamp(iNice);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_nice_clamp(int32_t iNice)
    __attribute__((alias("gj_nice_clamp")));
