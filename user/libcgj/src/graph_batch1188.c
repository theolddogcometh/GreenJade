/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1188: test one capability bit in a 32-bit mask.
 *
 * Surface (unique symbols):
 *   int gj_cap_has_bit(uint32_t caps, unsigned bit);
 *     — Return 1 if bit < 32 and (caps & (1<<bit)) != 0; else 0.
 *   int __gj_cap_has_bit  (alias)
 *   __libcgj_batch1188_marker = "libcgj-batch1188"
 *
 * Pairs with raise/drop (batch1186/1187); unique public name — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1188_marker[] = "libcgj-batch1188";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cap_has_bit — 1 if capability bit is present.
 */
int
gj_cap_has_bit(uint32_t u32Caps, unsigned uBit)
{
	if (uBit >= 32u) {
		return 0;
	}
	return ((u32Caps & (1u << uBit)) != 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cap_has_bit(uint32_t u32Caps, unsigned uBit)
    __attribute__((alias("gj_cap_has_bit")));
