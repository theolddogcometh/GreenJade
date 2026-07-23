/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4125: clear an unsigned bitfield in uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_bf_clear_u(uint32_t x, unsigned lo, unsigned w);
 *     - Clear bits [lo, lo+w) of x (set them to 0); other bits preserved.
 *       Soft-guard: w==0 leaves x unchanged. w is capped by remaining
 *       width above lo (type width 32). lo >= 32 is a no-op. Never forms
 *       a shift-by-32.
 *   uint32_t __gj_u32_bf_clear_u  (alias)
 *   __libcgj_batch4125_marker = "libcgj-batch4125"
 *
 * Exclusive continuum CREATE-ONLY (4121-4130: u32_bf_extract_u,
 * u64_bf_extract_u, u32_bf_deposit_u, u64_bf_deposit_u, u32_bf_clear_u,
 * u64_bf_clear_u, u32_bf_set_all_u, u64_bf_set_all_u, u32_bf_width_mask_u,
 * batch_id_4130). Unique gj_u32_bf_clear_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4125_marker[] = "libcgj-batch4125";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Clear bits [lo, lo+w) of u32X.
 * Soft-guard w==0 / lo>=32 → x unchanged; cap w to remaining width.
 */
static uint32_t
b4125_clear(uint32_t u32X, unsigned uLo, unsigned uW)
{
	unsigned uRem;
	uint32_t u32Mask;

	if (uW == 0u || uLo >= 32u) {
		return u32X;
	}

	uRem = 32u - uLo;
	if (uW > uRem) {
		uW = uRem;
	}

	if (uW == 32u) {
		return 0u;
	}

	u32Mask = ((uint32_t)1u << uW) - 1u;
	return u32X & ~(u32Mask << uLo);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bf_clear_u - clear bits [lo, lo+w) of x.
 *
 * x:  host word
 * lo: starting bit index (0 = LSB)
 * w:  field width in bits
 *
 * Returns x with the field zeroed; x unchanged when w==0 or lo>=32.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_bf_clear_u(uint32_t u32X, unsigned uLo, unsigned uW)
{
	(void)NULL;
	return b4125_clear(u32X, uLo, uW);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_bf_clear_u(uint32_t u32X, unsigned uLo, unsigned uW)
    __attribute__((alias("gj_u32_bf_clear_u")));
