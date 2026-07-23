/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4414: binary-reflected Gray to binary (u64).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_from_gray_u(uint64_t x);
 *     - Inverse of binary-reflected Gray encode for 64-bit words.
 *       Recover binary rank via successive XOR of right shifts.
 *   uint64_t __gj_u64_from_gray_u  (alias)
 *   __libcgj_batch4414_marker = "libcgj-batch4414"
 *
 * Exclusive continuum CREATE-ONLY (4411-4420: gray code unique). Unique
 * gj_u64_from_gray_u surface only; no multi-def. Distinct from
 * gj_gray_decode_u64 (batch597) and gj_u32_from_gray_u (batch4412).
 * Private helpers only. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4414_marker[] = "libcgj-batch4414";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-reflected Gray decode u64 (private). */
static uint64_t
b4414_from_gray(uint64_t u64G)
{
	u64G ^= u64G >> 1;
	u64G ^= u64G >> 2;
	u64G ^= u64G >> 4;
	u64G ^= u64G >> 8;
	u64G ^= u64G >> 16;
	u64G ^= u64G >> 32;
	return u64G;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_from_gray_u - convert binary-reflected Gray code to binary.
 *
 * x: Gray-coded 64-bit value
 *
 * Returns the binary rank n such that to_gray(n) == x.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_from_gray_u(uint64_t u64X)
{
	(void)NULL;
	return b4414_from_gray(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_from_gray_u(uint64_t u64X)
    __attribute__((alias("gj_u64_from_gray_u")));
