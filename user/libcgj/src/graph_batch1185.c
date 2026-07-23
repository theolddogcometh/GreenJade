/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1185: test one bit in a packed feature mask.
 *
 * Surface (unique symbols):
 *   int gj_feature_test_bit(uint32_t pack, unsigned bit);
 *     — Return 1 if bit < 32 and (pack & (1<<bit)) != 0; else 0.
 *       bit >= 32 → 0 (out of range is absent).
 *   int __gj_feature_test_bit  (alias)
 *   __libcgj_batch1185_marker = "libcgj-batch1185"
 *
 * Pairs with gj_feature_pack_u32 (batch1184); unique public name — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1185_marker[] = "libcgj-batch1185";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_feature_test_bit — 1 if bit is set in pack (bit must be < 32).
 */
int
gj_feature_test_bit(uint32_t u32Pack, unsigned uBit)
{
	if (uBit >= 32u) {
		return 0;
	}
	return ((u32Pack & (1u << uBit)) != 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_feature_test_bit(uint32_t u32Pack, unsigned uBit)
    __attribute__((alias("gj_feature_test_bit")));
