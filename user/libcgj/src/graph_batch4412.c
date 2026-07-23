/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4412: binary-reflected Gray to binary (u32).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_from_gray_u(uint32_t x);
 *     - Inverse of binary-reflected Gray encode for 32-bit words.
 *       Recover binary rank via successive XOR of right shifts.
 *   uint32_t __gj_u32_from_gray_u  (alias)
 *   __libcgj_batch4412_marker = "libcgj-batch4412"
 *
 * Exclusive continuum CREATE-ONLY (4411-4420: gray code unique). Unique
 * gj_u32_from_gray_u surface only; no multi-def. Distinct from
 * gj_gray_decode (batch182) — wave-unique _u name. Private helpers only.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4412_marker[] = "libcgj-batch4412";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-reflected Gray decode (private; same map as batch182). */
static uint32_t
b4412_from_gray(uint32_t u32G)
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
 * gj_u32_from_gray_u - convert binary-reflected Gray code to binary.
 *
 * x: Gray-coded 32-bit value
 *
 * Returns the binary rank n such that to_gray(n) == x.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_from_gray_u(uint32_t u32X)
{
	(void)NULL;
	return b4412_from_gray(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_from_gray_u(uint32_t u32X)
    __attribute__((alias("gj_u32_from_gray_u")));
