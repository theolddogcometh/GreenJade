/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5486: game overlay enable stub.
 *
 * Surface (unique symbols):
 *   int gj_overlay_enable_5486(uint32_t want);
 *     - Return 1 if want is non-zero (overlay requested/enabled stub),
 *       else 0. Soft pure-data enable gate only; no compositor touch.
 *   int __gj_overlay_enable_5486  (alias)
 *   __libcgj_batch5486_marker = "libcgj-batch5486"
 *
 * Deck Top 50 / launcher / overlay / FPS / FSR exclusive CREATE-ONLY
 * wave (5481-5490). Unique gj_overlay_enable_5486 surface only; no
 * multi-def. Distinct from launcher-hook surfaces. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5486_marker[] = "libcgj-batch5486";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5486_overlay_enable(uint32_t uWant)
{
	if (uWant != 0u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_overlay_enable_5486 - soft overlay-enable request gate.
 *
 * want: non-zero means overlay should be considered enabled
 * Returns 1 when want != 0, else 0.
 * Does not call libc. No parent wires.
 */
int
gj_overlay_enable_5486(uint32_t uWant)
{
	(void)NULL;
	return b5486_overlay_enable(uWant);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_overlay_enable_5486(uint32_t uWant)
    __attribute__((alias("gj_overlay_enable_5486")));
