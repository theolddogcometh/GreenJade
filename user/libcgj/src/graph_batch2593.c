/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2593: exclusive-wave batch chain soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_batch_chain_ok(uint32_t lo, uint32_t hi);
 *     - Soft check that exclusive-wave batch chain [lo, hi] fits a
 *       decade product span: lo != 0, lo <= hi, and (hi - lo) <= 9
 *       (at most ten TUs inclusive). Unsigned pure-data form.
 *   int __gj_batch_chain_ok  (alias)
 *   __libcgj_batch2593_marker = "libcgj-batch2593"
 *
 * Milestone 2600 exclusive product helpers (2591-2600). Unique
 * gj_batch_chain_ok surface only; no multi-def. Distinct from
 * gj_batch_id_ok_range (batch2493), gj_batch_span_ok_u (batch2393),
 * and gj_continuum_span_ok (batch2294).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2593_marker[] = "libcgj-batch2593";

/* Soft max decade chain width (hi - lo inclusive of ten TUs). */
#define B2593_CHAIN_MAX  9u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if [lo, hi] is a plausible decade exclusive batch chain. */
static int
b2593_chain_ok(uint32_t uLo, uint32_t uHi)
{
	if (uLo == 0u) {
		return 0;
	}
	if (uLo > uHi) {
		return 0;
	}
	if ((uHi - uLo) > B2593_CHAIN_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_chain_ok - soft decade-chain check for batch span [lo, hi].
 *
 * lo: inclusive chain start batch id (must be nonzero)
 * hi: inclusive chain end batch id
 *
 * Returns 1 when lo != 0, lo <= hi, and hi - lo <= 9, else 0.
 * Example: lo=2591, hi=2600 -> 1. Does not call libc.
 */
int
gj_batch_chain_ok(uint32_t uLo, uint32_t uHi)
{
	(void)NULL;
	return b2593_chain_ok(uLo, uHi);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_batch_chain_ok(uint32_t uLo, uint32_t uHi)
    __attribute__((alias("gj_batch_chain_ok")));
