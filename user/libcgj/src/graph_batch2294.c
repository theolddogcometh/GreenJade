/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2294: continuum decade span soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_continuum_span_ok(uint32_t lo, uint32_t hi);
 *     - Soft check that exclusive-wave continuum [lo, hi] fits a decade
 *       product span: lo != 0, lo <= hi, and (hi - lo) <= 9 (at most
 *       ten TUs inclusive). Distinct from gj_continuum_ok_hint
 *       (batch2197, hi - lo < 100).
 *   int __gj_continuum_span_ok  (alias)
 *   __libcgj_batch2294_marker = "libcgj-batch2294"
 *
 * Milestone 2300 exclusive product helpers (2291-2300). Unique
 * gj_continuum_span_ok surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2294_marker[] = "libcgj-batch2294";

/* Soft max decade span width (hi - lo inclusive of ten TUs). */
#define B2294_SPAN_MAX  9u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if [lo, hi] is a plausible decade exclusive continuum. */
static int
b2294_span_ok(uint32_t uLo, uint32_t uHi)
{
	if (uLo == 0u) {
		return 0;
	}
	if (uLo > uHi) {
		return 0;
	}
	if ((uHi - uLo) > B2294_SPAN_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_span_ok - soft decade-span check for continuum [lo, hi].
 *
 * lo: inclusive continuum start batch id (must be nonzero)
 * hi: inclusive continuum end batch id
 *
 * Returns 1 when lo != 0, lo <= hi, and hi - lo <= 9, else 0.
 * Example: lo=2291, hi=2300 -> 1. Does not call libc.
 */
int
gj_continuum_span_ok(uint32_t uLo, uint32_t uHi)
{
	(void)NULL;
	return b2294_span_ok(uLo, uHi);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_continuum_span_ok(uint32_t uLo, uint32_t uHi)
    __attribute__((alias("gj_continuum_span_ok")));
