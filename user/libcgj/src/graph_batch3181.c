/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3181: freestanding NUL-terminated FNV-1a 32-bit
 * string hash (wave-unique rename).
 *
 * Surface (unique symbols):
 *   uint32_t gj_str_hash_fnv1a32_u2(const char *s);
 *     - FNV-1a 32-bit over the C string at s until NUL. Offset basis
 *       2166136261, prime 16777619. NULL s -> 0. Empty string ->
 *       offset basis. Renamed from gj_str_hash_fnv1a32_u (collision
 *       with batch2323).
 *   uint32_t __gj_str_hash_fnv1a32_u2  (alias)
 *   __libcgj_batch3181_marker = "libcgj-batch3181"
 *
 * CREATE-ONLY exclusive continuum wave (3181-3190). Unique
 * gj_str_hash_fnv1a32_u2 surface only; no multi-def. Distinct from
 * gj_str_hash_fnv1a32_u (batch2323). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3181_marker[] = "libcgj-batch3181";

/* FNV-1a 32-bit parameters (public domain constants). */
#define B3181_FNV32_OFFSET 2166136261u
#define B3181_FNV32_PRIME  16777619u

/* ---- freestanding helpers ---------------------------------------------- */

/* FNV-1a 32 walk of a NUL-terminated C string. NULL -> 0. */
static uint32_t
b3181_fnv1a32(const char *sz)
{
	const unsigned char *p;
	uint32_t u32H;

	if (sz == NULL) {
		return 0u;
	}
	p = (const unsigned char *)sz;
	u32H = B3181_FNV32_OFFSET;
	while (*p != 0u) {
		u32H ^= (uint32_t)(*p);
		u32H *= B3181_FNV32_PRIME;
		p++;
	}
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_hash_fnv1a32_u2 - FNV-1a 32-bit hash of a NUL-terminated C string.
 *
 * s: C string (NULL -> 0)
 *
 * Returns the 32-bit digest. Empty string returns the offset basis.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_str_hash_fnv1a32_u2(const char *sz)
{
	(void)NULL;
	return b3181_fnv1a32(sz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_str_hash_fnv1a32_u2(const char *sz)
    __attribute__((alias("gj_str_hash_fnv1a32_u2")));
