/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8426: 32-bit rotate-left (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hash_rotl32_u_8426(uint32_t x, uint32_t r);
 *     - Rotate x left by (r & 31) bits. r==0 (mod 32) is identity.
 *       Pure; used by mix/hash steps that need barrel rotate.
 *   uint32_t __gj_hash_rotl32_u_8426  (alias)
 *   __libcgj_batch8426_marker = "libcgj-batch8426"
 *
 * Hash/fnv-ish integer stubs wave (8421-8430). Distinct from file-static
 * b41_rotl32 / b40_rotl32 helpers — unique _u_8426 surface; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8426_marker[] = "libcgj-batch8426";

/* ---- freestanding helpers ---------------------------------------------- */

/* Rotate u32X left by u32R mod 32. */
static uint32_t
b8426_rotl32(uint32_t u32X, uint32_t u32R)
{
	uint32_t u32N = u32R & 31u;

	if (u32N == 0u) {
		return u32X;
	}
	return (u32X << u32N) | (u32X >> (32u - u32N));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_rotl32_u_8426 - 32-bit left rotate of x by r bits.
 *
 * x: value to rotate
 * r: rotate count; only low 5 bits participate (mod 32)
 *
 * Returns the rotated value. Stateless pure function.
 */
uint32_t
gj_hash_rotl32_u_8426(uint32_t u32X, uint32_t u32R)
{
	(void)NULL;
	return b8426_rotl32(u32X, u32R);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hash_rotl32_u_8426(uint32_t u32X, uint32_t u32R)
    __attribute__((alias("gj_hash_rotl32_u_8426")));
