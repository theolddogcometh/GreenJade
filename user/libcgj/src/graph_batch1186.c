/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1186: raise one capability bit in a 32-bit mask.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cap_raise_bit(uint32_t caps, unsigned bit);
 *     — Return caps with bit set when bit < 32; else return caps
 *       unchanged (out-of-range raise is a no-op).
 *   uint32_t __gj_cap_raise_bit  (alias)
 *   __libcgj_batch1186_marker = "libcgj-batch1186"
 *
 * Userspace integer capability mask helper; does not redefine kernel cap/cnode surfaces — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1186_marker[] = "libcgj-batch1186";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cap_raise_bit — set bit in capability mask.
 */
uint32_t
gj_cap_raise_bit(uint32_t u32Caps, unsigned uBit)
{
	if (uBit >= 32u) {
		return u32Caps;
	}
	return u32Caps | (1u << uBit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cap_raise_bit(uint32_t u32Caps, unsigned uBit)
    __attribute__((alias("gj_cap_raise_bit")));
