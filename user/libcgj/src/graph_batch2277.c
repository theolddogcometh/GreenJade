/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2277: preferred display refresh-rate chooser.
 *
 * Surface (unique symbols):
 *   uint32_t gj_refresh_prefer_hz(uint32_t a_hz, uint32_t b_hz);
 *     - Choose a preferred refresh rate among two candidates. A rate is
 *       soft-valid when it is in [1, 500] Hz. Prefer the larger valid
 *       candidate; if only one is valid return that one; if neither is
 *       valid return 0. Soft display-mode helper only.
 *   uint32_t __gj_refresh_prefer_hz  (alias)
 *   __libcgj_batch2277_marker = "libcgj-batch2277"
 *
 * Post-2270 product exclusive wave (2271-2280). Unique
 * gj_refresh_prefer_hz surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2277_marker[] = "libcgj-batch2277";

/* Soft exclusive upper band for consumer display refresh rates (Hz). */
#define B2277_HZ_MAX  500u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if hz is in [1, B2277_HZ_MAX]. */
static int
b2277_hz_ok(uint32_t uHz)
{
	if (uHz == 0u) {
		return 0;
	}
	if (uHz > B2277_HZ_MAX) {
		return 0;
	}
	return 1;
}

/* Prefer larger valid of a,b; single valid wins; else 0. */
static uint32_t
b2277_prefer(uint32_t uA, uint32_t uB)
{
	int fA;
	int fB;

	fA = b2277_hz_ok(uA);
	fB = b2277_hz_ok(uB);
	if (fA != 0 && fB != 0) {
		if (uA >= uB) {
			return uA;
		}
		return uB;
	}
	if (fA != 0) {
		return uA;
	}
	if (fB != 0) {
		return uB;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_refresh_prefer_hz - prefer the better of two refresh candidates.
 *
 * a_hz, b_hz: candidate rates in Hz (0 or >500 treated as invalid)
 *
 * Returns the larger valid candidate, the only valid candidate, or 0.
 * Example: (60, 90) -> 90; (0, 120) -> 120; (0, 0) -> 0.
 * Does not call libc.
 */
uint32_t
gj_refresh_prefer_hz(uint32_t uAHz, uint32_t uBHz)
{
	(void)NULL;
	return b2277_prefer(uAHz, uBHz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_refresh_prefer_hz(uint32_t uAHz, uint32_t uBHz)
    __attribute__((alias("gj_refresh_prefer_hz")));
