/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2276: HDR display mode soft-ok predicate
 * (unsigned).
 *
 * Surface (unique symbols):
 *   int gj_hdr_mode_ok_u(uint32_t mode);
 *     - Return 1 if mode is a soft-valid HDR display mode code in
 *       [0, 3]:
 *         0 = SDR / off
 *         1 = HDR10
 *         2 = HDR10+
 *         3 = Dolby Vision-class
 *       Else 0. Soft enum-band hint only; not a compositor authority.
 *   int __gj_hdr_mode_ok_u  (alias)
 *   __libcgj_batch2276_marker = "libcgj-batch2276"
 *
 * Post-2270 product exclusive wave (2271-2280). Unique gj_hdr_mode_ok_u
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2276_marker[] = "libcgj-batch2276";

/* Inclusive soft max HDR mode code. */
#define B2276_MODE_MAX  3u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if mode is in [0, B2276_MODE_MAX]. */
static int
b2276_mode_ok(uint32_t uMode)
{
	if (uMode > B2276_MODE_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hdr_mode_ok_u - soft check that mode is a known HDR mode band.
 *
 * mode: HDR display mode code (0..3 accepted)
 *
 * Returns 1 when mode <= 3, else 0. Does not call libc.
 */
int
gj_hdr_mode_ok_u(uint32_t uMode)
{
	(void)NULL;
	return b2276_mode_ok(uMode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hdr_mode_ok_u(uint32_t uMode)
    __attribute__((alias("gj_hdr_mode_ok_u")));
