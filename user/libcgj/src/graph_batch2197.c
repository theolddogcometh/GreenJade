/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2197: continuum span soft-ok hint.
 *
 * Surface (unique symbols):
 *   int gj_continuum_ok_hint(uint32_t lo, uint32_t hi);
 *     - Soft check that a product continuum span [lo, hi] is well
 *       formed: lo != 0, lo <= hi, and (hi - lo) < 100 (exclusive-wave
 *       scale). Else 0. Not a graph topology authority.
 *   int __gj_continuum_ok_hint  (alias)
 *   __libcgj_batch2197_marker = "libcgj-batch2197"
 *
 * Milestone 2200 exclusive product helpers (2191-2200). Unique
 * gj_continuum_ok_hint surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2197_marker[] = "libcgj-batch2197";

/* Soft max exclusive-wave span width (hi - lo must be strictly less). */
#define B2197_SPAN_MAX  100u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if [lo, hi] is a plausible product exclusive continuum. */
static int
b2197_ok(uint32_t uLo, uint32_t uHi)
{
	if (uLo == 0u) {
		return 0;
	}
	if (uLo > uHi) {
		return 0;
	}
	if ((uHi - uLo) >= B2197_SPAN_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_ok_hint - soft well-formed check for continuum [lo, hi].
 *
 * lo: inclusive continuum start batch id (must be nonzero)
 * hi: inclusive continuum end batch id
 *
 * Returns 1 when lo != 0, lo <= hi, and hi - lo < 100, else 0.
 * Example: lo=2191, hi=2200 -> 1. Does not call libc.
 */
int
gj_continuum_ok_hint(uint32_t uLo, uint32_t uHi)
{
	(void)NULL;
	return b2197_ok(uLo, uHi);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_continuum_ok_hint(uint32_t uLo, uint32_t uHi)
    __attribute__((alias("gj_continuum_ok_hint")));
