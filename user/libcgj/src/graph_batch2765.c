/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2765: uint32_t three-way minimum (_u surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_min3_u(uint32_t a, uint32_t b, uint32_t c);
 *     - Least of a, b, and c (unsigned compare). Pairwise fold:
 *       min(a, b) then min(that, c). On equality the later operand of
 *       each comparison is kept.
 *   uint32_t __gj_u32_min3_u  (alias)
 *   __libcgj_batch2765_marker = "libcgj-batch2765"
 *
 * Post-2760 str/mem/int exclusive wave (2761-2770). Distinct from
 * gj_u32_min3 (batch2354) / gj_min3_u64 (batch588) - unique
 * gj_u32_min3_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2765_marker[] = "libcgj-batch2765";

/* ---- freestanding helpers ---------------------------------------------- */

/* Least of three uint32 values. */
static uint32_t
b2765_min3(uint32_t uA, uint32_t uB, uint32_t uC)
{
	uint32_t uM;

	if (uA < uB) {
		uM = uA;
	} else {
		uM = uB;
	}
	if (uC < uM) {
		return uC;
	}
	return uM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_min3_u - return the least of three uint32_t values.
 *
 * a, b, c: candidates
 *
 * Pairwise reduction under unsigned compare. Does not call libc.
 */
uint32_t
gj_u32_min3_u(uint32_t uA, uint32_t uB, uint32_t uC)
{
	(void)NULL;
	return b2765_min3(uA, uB, uC);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_min3_u(uint32_t uA, uint32_t uB, uint32_t uC)
    __attribute__((alias("gj_u32_min3_u")));
