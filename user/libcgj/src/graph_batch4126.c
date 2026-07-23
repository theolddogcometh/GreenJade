/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4126: clear an unsigned bitfield in uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_bf_clear_u(uint64_t x, unsigned lo, unsigned w);
 *     - Clear bits [lo, lo+w) of x (set them to 0); other bits preserved.
 *       Soft-guard: w==0 leaves x unchanged. w is capped by remaining
 *       width above lo (type width 64). lo >= 64 is a no-op. Never forms
 *       a shift-by-64.
 *   uint64_t __gj_u64_bf_clear_u  (alias)
 *   __libcgj_batch4126_marker = "libcgj-batch4126"
 *
 * Exclusive continuum CREATE-ONLY (4121-4130: u32_bf_extract_u,
 * u64_bf_extract_u, u32_bf_deposit_u, u64_bf_deposit_u, u32_bf_clear_u,
 * u64_bf_clear_u, u32_bf_set_all_u, u64_bf_set_all_u, u32_bf_width_mask_u,
 * batch_id_4130). Unique gj_u64_bf_clear_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4126_marker[] = "libcgj-batch4126";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Clear bits [lo, lo+w) of u64X.
 * Soft-guard w==0 / lo>=64 → x unchanged; cap w to remaining width.
 */
static uint64_t
b4126_clear(uint64_t u64X, unsigned uLo, unsigned uW)
{
	unsigned uRem;
	uint64_t u64Mask;

	if (uW == 0u || uLo >= 64u) {
		return u64X;
	}

	uRem = 64u - uLo;
	if (uW > uRem) {
		uW = uRem;
	}

	if (uW == 64u) {
		return 0ULL;
	}

	u64Mask = (1ULL << uW) - 1ULL;
	return u64X & ~(u64Mask << uLo);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_bf_clear_u - clear bits [lo, lo+w) of x.
 *
 * x:  host word
 * lo: starting bit index (0 = LSB)
 * w:  field width in bits
 *
 * Returns x with the field zeroed; x unchanged when w==0 or lo>=64.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_bf_clear_u(uint64_t u64X, unsigned uLo, unsigned uW)
{
	(void)NULL;
	return b4126_clear(u64X, uLo, uW);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_bf_clear_u(uint64_t u64X, unsigned uLo, unsigned uW)
    __attribute__((alias("gj_u64_bf_clear_u")));
