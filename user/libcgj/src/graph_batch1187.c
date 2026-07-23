/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1187: drop one capability bit from a 32-bit mask.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cap_drop_bit(uint32_t caps, unsigned bit);
 *     — Return caps with bit cleared when bit < 32; else return caps
 *       unchanged.
 *   uint32_t __gj_cap_drop_bit  (alias)
 *   __libcgj_batch1187_marker = "libcgj-batch1187"
 *
 * Pairs with gj_cap_raise_bit (batch1186); unique public name — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1187_marker[] = "libcgj-batch1187";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cap_drop_bit — clear bit in capability mask.
 */
uint32_t
gj_cap_drop_bit(uint32_t u32Caps, unsigned uBit)
{
	if (uBit >= 32u) {
		return u32Caps;
	}
	return u32Caps & ~(1u << uBit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cap_drop_bit(uint32_t u32Caps, unsigned uBit)
    __attribute__((alias("gj_cap_drop_bit")));
