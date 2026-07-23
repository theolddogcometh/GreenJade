/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4979: PE FileAlignment/SectionAlignment round-up.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pe_align_u(uint32_t val, uint32_t align);
 *     - Round val up to a multiple of align (PE FileAlignment /
 *       SectionAlignment style). align == 0 or 1 → return val.
 *       Power-of-two align uses the mask path; else general multiple.
 *       Arithmetic overflow of the round-up → 0.
 *   uint32_t __gj_pe_align_u  (alias)
 *   __libcgj_batch4979_marker = "libcgj-batch4979"
 *
 * Exclusive continuum CREATE-ONLY PE unique wave (4971-4980). Distinct
 * from gj_u32_align_up / gj_align_up_u64 / gj_cacheline_align_up_u —
 * unique gj_pe_align_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4979_marker[] = "libcgj-batch4979";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Round uVal up to a multiple of uAlign.
 * uAlign <= 1 → identity. Overflow of the rounded value → 0.
 */
static uint32_t
b4979_align(uint32_t uVal, uint32_t uAlign)
{
	uint32_t uMask;
	uint32_t uAdj;
	uint32_t uRem;

	if (uAlign <= 1u) {
		return uVal;
	}

	/* Power-of-two fast path (PE alignments are typically pow2). */
	if ((uAlign & (uAlign - 1u)) == 0u) {
		uMask = uAlign - 1u;
		uAdj = (uVal + uMask) & ~uMask;
		if (uAdj < uVal) {
			return 0u;
		}
		return uAdj;
	}

	/* General multiple: val + (align - val % align) when needed. */
	uRem = uVal % uAlign;
	if (uRem == 0u) {
		return uVal;
	}
	uAdj = uVal + (uAlign - uRem);
	if (uAdj < uVal) {
		return 0u;
	}
	return uAdj;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pe_align_u - round val up to a multiple of align (PE style).
 *
 * val:   value to align (RVA, file offset, size, etc.)
 * align: alignment / stride (0 or 1 → identity; pow2 uses mask path)
 *
 * Overflow (rounded value wraps below val) → 0.
 * Distinct from gj_u32_align_up. No parent wires.
 */
uint32_t
gj_pe_align_u(uint32_t uVal, uint32_t uAlign)
{
	(void)NULL;
	return b4979_align(uVal, uAlign);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pe_align_u(uint32_t uVal, uint32_t uAlign)
    __attribute__((alias("gj_pe_align_u")));
