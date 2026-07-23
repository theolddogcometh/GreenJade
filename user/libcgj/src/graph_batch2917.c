/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2917: binary-reflected Gray decode u32
 * (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gray_decode_u32_u(uint32_t g);
 *     - Inverse of gj_gray_encode_u32_u: recover binary via successive
 *       XOR of right shifts (1, 2, 4, 8, 16). Encode then decode is
 *       identity for all uint32_t.
 *   uint32_t __gj_gray_decode_u32_u  (alias)
 *   __libcgj_batch2917_marker = "libcgj-batch2917"
 *
 * Exclusive continuum wave 2911-2920. Distinct from gj_gray_decode
 * (batch182) and gj_gray_decode_u64 (batch597). Unique surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2917_marker[] = "libcgj-batch2917";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2917_decode(uint32_t u32G)
{
	u32G ^= u32G >> 1;
	u32G ^= u32G >> 2;
	u32G ^= u32G >> 4;
	u32G ^= u32G >> 8;
	u32G ^= u32G >> 16;
	return u32G;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gray_decode_u32_u - convert binary-reflected Gray code g to binary.
 *
 * g: Gray code word
 *
 * Unfolds the prefix-XOR structure of the encode map over 32 bits.
 * Does not call libc or parent gj_*.
 */
uint32_t
gj_gray_decode_u32_u(uint32_t u32G)
{
	(void)NULL;
	return b2917_decode(u32G);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gray_decode_u32_u(uint32_t u32G)
    __attribute__((alias("gj_gray_decode_u32_u")));
