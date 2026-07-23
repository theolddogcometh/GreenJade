/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4121: unsigned bitfield extract from uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_bf_extract_u(uint32_t x, unsigned lo, unsigned w);
 *     - Extract the half-open bit range [lo, lo+w) from x, right-aligned
 *       into the low bits of the result. Soft-guard: w==0 yields 0.
 *       w is capped by the remaining width above lo (type width 32).
 *       lo >= 32 yields 0. Never forms a shift-by-32.
 *   uint32_t __gj_u32_bf_extract_u  (alias)
 *   __libcgj_batch4121_marker = "libcgj-batch4121"
 *
 * Exclusive continuum CREATE-ONLY (4121-4130: u32_bf_extract_u,
 * u64_bf_extract_u, u32_bf_deposit_u, u64_bf_deposit_u, u32_bf_clear_u,
 * u64_bf_clear_u, u32_bf_set_all_u, u64_bf_set_all_u, u32_bf_width_mask_u,
 * batch_id_4130). Distinct from gj_extract_bits_u64 (batch593) —
 * unique gj_u32_bf_extract_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4121_marker[] = "libcgj-batch4121";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Extract bits [lo, lo+w) from u32X, right-aligned.
 * Soft-guard w==0 / lo>=32 → 0; cap w to remaining width.
 */
static uint32_t
b4121_extract(uint32_t u32X, unsigned uLo, unsigned uW)
{
	unsigned uRem;
	uint32_t u32Mask;

	if (uW == 0u || uLo >= 32u) {
		return 0u;
	}

	uRem = 32u - uLo;
	if (uW > uRem) {
		uW = uRem;
	}

	u32X >>= uLo;
	if (uW == 32u) {
		/* lo was 0 and full width requested — whole word */
		return u32X;
	}

	u32Mask = ((uint32_t)1u << uW) - 1u;
	return u32X & u32Mask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bf_extract_u - right-aligned extract of bits [lo, lo+w) from x.
 *
 * x:  source word
 * lo: starting bit index (0 = LSB)
 * w:  field width in bits
 *
 * Returns the extracted field in the low bits, or 0 when w==0 or lo>=32.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_bf_extract_u(uint32_t u32X, unsigned uLo, unsigned uW)
{
	(void)NULL;
	return b4121_extract(u32X, uLo, uW);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_bf_extract_u(uint32_t u32X, unsigned uLo, unsigned uW)
    __attribute__((alias("gj_u32_bf_extract_u")));
