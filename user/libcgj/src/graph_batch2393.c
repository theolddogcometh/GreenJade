/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2393: unsigned graph batch span soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_batch_span_ok_u(uint32_t lo, uint32_t hi);
 *     - Soft check that exclusive-wave batch span [lo, hi] fits a decade
 *       product span: lo != 0, lo <= hi, and (hi - lo) <= 9 (at most
 *       ten TUs inclusive). Unsigned pure-data form.
 *   int __gj_batch_span_ok_u  (alias)
 *   __libcgj_batch2393_marker = "libcgj-batch2393"
 *
 * Milestone 2400 exclusive product helpers (2391-2400). Unique
 * gj_batch_span_ok_u surface only; no multi-def. Distinct from
 * gj_continuum_span_ok (batch2294) and gj_continuum_ok_hint (batch2197).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2393_marker[] = "libcgj-batch2393";

/* Soft max decade span width (hi - lo inclusive of ten TUs). */
#define B2393_SPAN_MAX  9u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if [lo, hi] is a plausible decade exclusive batch span. */
static int
b2393_span_ok(uint32_t uLo, uint32_t uHi)
{
	if (uLo == 0u) {
		return 0;
	}
	if (uLo > uHi) {
		return 0;
	}
	if ((uHi - uLo) > B2393_SPAN_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_span_ok_u - soft decade-span check for batch span [lo, hi].
 *
 * lo: inclusive span start batch id (must be nonzero)
 * hi: inclusive span end batch id
 *
 * Returns 1 when lo != 0, lo <= hi, and hi - lo <= 9, else 0.
 * Example: lo=2391, hi=2400 -> 1. Does not call libc.
 */
int
gj_batch_span_ok_u(uint32_t uLo, uint32_t uHi)
{
	(void)NULL;
	return b2393_span_ok(uLo, uHi);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_batch_span_ok_u(uint32_t uLo, uint32_t uHi)
    __attribute__((alias("gj_batch_span_ok_u")));
