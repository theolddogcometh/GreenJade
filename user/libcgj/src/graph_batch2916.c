/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2916: binary-reflected Gray encode u32
 * (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gray_encode_u32_u(uint32_t n);
 *     - Binary-reflected Gray code of n: g = n ^ (n >> 1).
 *   uint32_t __gj_gray_encode_u32_u  (alias)
 *   __libcgj_batch2916_marker = "libcgj-batch2916"
 *
 * Exclusive continuum wave 2911-2920. Distinct from gj_gray_encode
 * (batch182) and gj_gray_encode_u64 (batch596). Unique surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2916_marker[] = "libcgj-batch2916";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2916_encode(uint32_t u32N)
{
	return u32N ^ (u32N >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gray_encode_u32_u - convert binary n to binary-reflected Gray code.
 *
 * n: binary value
 *
 * Returns n ^ (n >> 1). Adjacent integers map to codes differing by
 * exactly one bit. Does not call libc.
 */
uint32_t
gj_gray_encode_u32_u(uint32_t u32N)
{
	(void)NULL;
	return b2916_encode(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gray_encode_u32_u(uint32_t u32N)
    __attribute__((alias("gj_gray_encode_u32_u")));
