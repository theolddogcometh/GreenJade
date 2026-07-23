/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4413: binary to binary-reflected Gray (u64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_to_gray_u(uint64_t x);
 *     - Binary-reflected Gray code of x: g = x ^ (x >> 1).
 *   uint64_t __gj_u64_to_gray_u  (alias)
 *   __libcgj_batch4413_marker = "libcgj-batch4413"
 *
 * Exclusive continuum CREATE-ONLY (4411-4420: gray code unique). Unique
 * gj_u64_to_gray_u surface only; no multi-def. Distinct from
 * gj_gray_encode_u64 (batch596) and gj_u32_to_gray_u (batch4411).
 * Private helpers only. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4413_marker[] = "libcgj-batch4413";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-reflected Gray encode u64 (private). */
static uint64_t
b4413_to_gray(uint64_t u64X)
{
	return u64X ^ (u64X >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_to_gray_u - convert binary uint64_t to binary-reflected Gray.
 *
 * x: binary rank
 *
 * Returns g = x ^ (x >> 1). Adjacent ranks differ by one Gray bit.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_to_gray_u(uint64_t u64X)
{
	(void)NULL;
	return b4413_to_gray(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_to_gray_u(uint64_t u64X)
    __attribute__((alias("gj_u64_to_gray_u")));
