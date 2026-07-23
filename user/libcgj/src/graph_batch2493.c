/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2493: batch-id in exclusive-wave range soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_batch_id_ok_range(uint32_t id, uint32_t lo, uint32_t hi);
 *     - Soft check that batch id is inside inclusive exclusive-wave
 *       range [lo, hi]: lo != 0, lo <= hi, and lo <= id <= hi.
 *       Unsigned pure-data form for product continuum gates.
 *   int __gj_batch_id_ok_range  (alias)
 *   __libcgj_batch2493_marker = "libcgj-batch2493"
 *
 * Milestone 2500 exclusive product helpers (2491-2500). Unique
 * gj_batch_id_ok_range surface only; no multi-def. Distinct from
 * gj_batch_span_ok_u (batch2393 decade span only) and
 * gj_continuum_span_ok (batch2294).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2493_marker[] = "libcgj-batch2493";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if id is inside a well-formed inclusive range [lo, hi]. */
static int
b2493_id_ok(uint32_t uId, uint32_t uLo, uint32_t uHi)
{
	if (uLo == 0u) {
		return 0;
	}
	if (uLo > uHi) {
		return 0;
	}
	if (uId < uLo) {
		return 0;
	}
	if (uId > uHi) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_ok_range - soft check that batch id is in [lo, hi].
 *
 * id: batch id under test
 * lo: inclusive range start (must be nonzero)
 * hi: inclusive range end
 *
 * Returns 1 when lo != 0, lo <= hi, and lo <= id <= hi, else 0.
 * Example: id=2493, lo=2491, hi=2500 -> 1. Does not call libc.
 */
int
gj_batch_id_ok_range(uint32_t uId, uint32_t uLo, uint32_t uHi)
{
	(void)NULL;
	return b2493_id_ok(uId, uLo, uHi);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_batch_id_ok_range(uint32_t uId, uint32_t uLo, uint32_t uHi)
    __attribute__((alias("gj_batch_id_ok_range")));
