/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4123: unsigned bitfield deposit into uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_bf_deposit_u(uint32_t base, uint32_t val,
 *                                unsigned lo, unsigned w);
 *     - Replace bits [lo, lo+w) of base with the low w bits of val.
 *       Bits of base outside that range are preserved. Soft-guard:
 *       w==0 leaves base unchanged. w is capped by remaining width
 *       above lo (type width 32). lo >= 32 is a no-op. Never forms
 *       a shift-by-32.
 *   uint32_t __gj_u32_bf_deposit_u  (alias)
 *   __libcgj_batch4123_marker = "libcgj-batch4123"
 *
 * Exclusive continuum CREATE-ONLY (4121-4130: u32_bf_extract_u,
 * u64_bf_extract_u, u32_bf_deposit_u, u64_bf_deposit_u, u32_bf_clear_u,
 * u64_bf_clear_u, u32_bf_set_all_u, u64_bf_set_all_u, u32_bf_width_mask_u,
 * batch_id_4130). Distinct from gj_deposit_bits_u64 (batch594) —
 * unique gj_u32_bf_deposit_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4123_marker[] = "libcgj-batch4123";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Deposit low w bits of u32Val into bits [lo, lo+w) of u32Base.
 * Soft-guard w==0 / lo>=32 → base unchanged; cap w to remaining width.
 */
static uint32_t
b4123_deposit(uint32_t u32Base, uint32_t u32Val, unsigned uLo, unsigned uW)
{
	unsigned uRem;
	uint32_t u32Mask;

	if (uW == 0u || uLo >= 32u) {
		return u32Base;
	}

	uRem = 32u - uLo;
	if (uW > uRem) {
		uW = uRem;
	}

	if (uW == 32u) {
		return u32Val;
	}

	u32Mask = ((uint32_t)1u << uW) - 1u;
	return (u32Base & ~(u32Mask << uLo)) | ((u32Val & u32Mask) << uLo);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_bf_deposit_u - insert val into bits [lo, lo+w) of base.
 *
 * base: host word to modify
 * val:  source; only its low (clipped) w bits are deposited
 * lo:   starting bit index (0 = LSB)
 * w:    field width in bits
 *
 * Returns updated word; base unchanged when w==0 or lo>=32.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_bf_deposit_u(uint32_t u32Base, uint32_t u32Val, unsigned uLo,
    unsigned uW)
{
	(void)NULL;
	return b4123_deposit(u32Base, u32Val, uLo, uW);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_bf_deposit_u(uint32_t u32Base, uint32_t u32Val,
    unsigned uLo, unsigned uW)
    __attribute__((alias("gj_u32_bf_deposit_u")));
