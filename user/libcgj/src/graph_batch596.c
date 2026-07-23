/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch596: uint64 binary-reflected Gray code encode.
 *
 * Surface (unique symbols):
 *   uint64_t gj_gray_encode_u64(uint64_t x);
 *     — binary-reflected Gray code of x: g = x ^ (x >> 1).
 *   __gj_gray_encode_u64  (alias)
 *   __libcgj_batch596_marker = "libcgj-batch596"
 *
 * Does not redefine gj_gray_encode / gj_gray_decode (batch182, uint32_t)
 * or gj_gray_seq (batch244). Unique u64 surface only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch596_marker[] = "libcgj-batch596";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gray_encode_u64 — convert binary x to binary-reflected Gray code.
 * Adjacent integers map to codes differing by exactly one bit.
 * Same map as gj_gray_encode (batch182) widened to 64 bits.
 */
uint64_t
gj_gray_encode_u64(uint64_t x)
{
	return x ^ (x >> 1);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_gray_encode_u64(uint64_t x)
    __attribute__((alias("gj_gray_encode_u64")));
