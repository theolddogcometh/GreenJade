/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2599: continuum wave soft-ok predicate.
 *
 * Surface (unique symbols):
 *   int gj_continuum_wave_ok(uint32_t lo, uint32_t hi);
 *     - Soft check that continuum wave [lo, hi] is well-formed for the
 *       exclusive product wave window: lo != 0, lo <= hi, and
 *       (hi - lo) <= 99 (at most a 100-slot inclusive continuum).
 *   int __gj_continuum_wave_ok  (alias)
 *   __libcgj_batch2599_marker = "libcgj-batch2599"
 *
 * Milestone 2600 exclusive product helpers (2591-2600). Unique
 * gj_continuum_wave_ok surface only; no multi-def. Distinct from
 * gj_batch_chain_ok (batch2593 decade), gj_shell_product_ok
 * (batch2497), gj_shell_continuum_ok (batch2397), gj_continuum_span_ok
 * (batch2294), and gj_continuum_ok_hint (batch2197).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2599_marker[] = "libcgj-batch2599";

/* Soft max continuum wave width (hi - lo, 100 slots inclusive). */
#define B2599_WAVE_SPAN_MAX  99u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if [lo, hi] is a plausible continuum wave window. */
static int
b2599_wave_ok(uint32_t uLo, uint32_t uHi)
{
	if (uLo == 0u) {
		return 0;
	}
	if (uLo > uHi) {
		return 0;
	}
	if ((uHi - uLo) > B2599_WAVE_SPAN_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_ok - soft check for continuum wave [lo, hi].
 *
 * lo: inclusive continuum wave start id (must be nonzero)
 * hi: inclusive continuum wave end id
 *
 * Returns 1 when lo != 0, lo <= hi, and hi - lo <= 99, else 0.
 * Example: lo=2591, hi=2600 -> 1. Does not call libc.
 */
int
gj_continuum_wave_ok(uint32_t uLo, uint32_t uHi)
{
	(void)NULL;
	return b2599_wave_ok(uLo, uHi);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_continuum_wave_ok(uint32_t uLo, uint32_t uHi)
    __attribute__((alias("gj_continuum_wave_ok")));
