/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4124: unsigned bitfield deposit into uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_bf_deposit_u(uint64_t base, uint64_t val,
 *                                unsigned lo, unsigned w);
 *     - Replace bits [lo, lo+w) of base with the low w bits of val.
 *       Bits of base outside that range are preserved. Soft-guard:
 *       w==0 leaves base unchanged. w is capped by remaining width
 *       above lo (type width 64). lo >= 64 is a no-op. Never forms
 *       a shift-by-64.
 *   uint64_t __gj_u64_bf_deposit_u  (alias)
 *   __libcgj_batch4124_marker = "libcgj-batch4124"
 *
 * Exclusive continuum CREATE-ONLY (4121-4130: u32_bf_extract_u,
 * u64_bf_extract_u, u32_bf_deposit_u, u64_bf_deposit_u, u32_bf_clear_u,
 * u64_bf_clear_u, u32_bf_set_all_u, u64_bf_set_all_u, u32_bf_width_mask_u,
 * batch_id_4130). Distinct from gj_deposit_bits_u64 (batch594) —
 * unique gj_u64_bf_deposit_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4124_marker[] = "libcgj-batch4124";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Deposit low w bits of u64Val into bits [lo, lo+w) of u64Base.
 * Soft-guard w==0 / lo>=64 → base unchanged; cap w to remaining width.
 */
static uint64_t
b4124_deposit(uint64_t u64Base, uint64_t u64Val, unsigned uLo, unsigned uW)
{
	unsigned uRem;
	uint64_t u64Mask;

	if (uW == 0u || uLo >= 64u) {
		return u64Base;
	}

	uRem = 64u - uLo;
	if (uW > uRem) {
		uW = uRem;
	}

	if (uW == 64u) {
		return u64Val;
	}

	u64Mask = (1ULL << uW) - 1ULL;
	return (u64Base & ~(u64Mask << uLo)) | ((u64Val & u64Mask) << uLo);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_bf_deposit_u - insert val into bits [lo, lo+w) of base.
 *
 * base: host word to modify
 * val:  source; only its low (clipped) w bits are deposited
 * lo:   starting bit index (0 = LSB)
 * w:    field width in bits
 *
 * Returns updated word; base unchanged when w==0 or lo>=64.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_bf_deposit_u(uint64_t u64Base, uint64_t u64Val, unsigned uLo,
    unsigned uW)
{
	(void)NULL;
	return b4124_deposit(u64Base, u64Val, uLo, uW);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_bf_deposit_u(uint64_t u64Base, uint64_t u64Val,
    unsigned uLo, unsigned uW)
    __attribute__((alias("gj_u64_bf_deposit_u")));
