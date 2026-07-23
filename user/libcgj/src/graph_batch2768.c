/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2768: uint64_t three-way maximum (_u surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_max3_u(uint64_t a, uint64_t b, uint64_t c);
 *     - Greatest of a, b, and c (unsigned compare). Pairwise fold:
 *       max(a, b) then max(that, c). On equality the later operand of
 *       each comparison is kept.
 *   uint64_t __gj_u64_max3_u  (alias)
 *   __libcgj_batch2768_marker = "libcgj-batch2768"
 *
 * Post-2760 str/mem/int exclusive wave (2761-2770). Distinct from
 * gj_u64_max3 (batch2357) / gj_max3_u64 (batch589) - unique
 * gj_u64_max3_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2768_marker[] = "libcgj-batch2768";

/* ---- freestanding helpers ---------------------------------------------- */

/* Greatest of three uint64 values. */
static uint64_t
b2768_max3(uint64_t uA, uint64_t uB, uint64_t uC)
{
	uint64_t uM;

	if (uA > uB) {
		uM = uA;
	} else {
		uM = uB;
	}
	if (uM > uC) {
		return uM;
	}
	return uC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_max3_u - return the greatest of three uint64_t values.
 *
 * a, b, c: candidates
 *
 * Pairwise reduction under unsigned compare. Does not call libc.
 */
uint64_t
gj_u64_max3_u(uint64_t uA, uint64_t uB, uint64_t uC)
{
	(void)NULL;
	return b2768_max3(uA, uB, uC);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_max3_u(uint64_t uA, uint64_t uB, uint64_t uC)
    __attribute__((alias("gj_u64_max3_u")));
