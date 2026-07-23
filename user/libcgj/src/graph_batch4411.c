/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4411: binary to binary-reflected Gray (u32).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_to_gray_u(uint32_t x);
 *     - Binary-reflected Gray code of x: g = x ^ (x >> 1).
 *   uint32_t __gj_u32_to_gray_u  (alias)
 *   __libcgj_batch4411_marker = "libcgj-batch4411"
 *
 * Exclusive continuum CREATE-ONLY (4411-4420: gray code unique). Unique
 * gj_u32_to_gray_u surface only; no multi-def. Distinct from
 * gj_gray_encode (batch182) — wave-unique _u name. Private helpers only.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4411_marker[] = "libcgj-batch4411";

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary-reflected Gray encode (private; same map as batch182). */
static uint32_t
b4411_to_gray(uint32_t u32X)
{
	return u32X ^ (u32X >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_to_gray_u - convert binary uint32_t to binary-reflected Gray.
 *
 * x: binary rank
 *
 * Returns g = x ^ (x >> 1). Adjacent ranks differ by one Gray bit.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_to_gray_u(uint32_t u32X)
{
	(void)NULL;
	return b4411_to_gray(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_to_gray_u(uint32_t u32X)
    __attribute__((alias("gj_u32_to_gray_u")));
