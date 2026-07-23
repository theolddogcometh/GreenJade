/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3461: freestanding NUL-terminated FNV-1a 64-bit
 * string hash (wave-unique rename).
 *
 * Surface (unique symbols):
 *   uint64_t gj_str_hash_fnv1a64_u2(const char *s);
 *     - FNV-1a 64-bit over the C string at s until NUL. Offset basis
 *       14695981039346656037, prime 1099511628211. NULL s -> 0.
 *       Empty string -> offset basis. Renamed from gj_str_hash_fnv1a64_u
 *       (collision with batch2324).
 *   uint64_t __gj_str_hash_fnv1a64_u2  (alias)
 *   __libcgj_batch3461_marker = "libcgj-batch3461"
 *
 * CREATE-ONLY exclusive continuum wave (3461-3470). Unique
 * gj_str_hash_fnv1a64_u2 surface only; no multi-def. Distinct from
 * gj_str_hash_fnv1a64_u (batch2324). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3461_marker[] = "libcgj-batch3461";

/* FNV-1a 64-bit parameters (public domain constants). */
#define B3461_FNV64_OFFSET 14695981039346656037ULL
#define B3461_FNV64_PRIME  1099511628211ULL

/* ---- freestanding helpers ---------------------------------------------- */

/* FNV-1a 64 walk of a NUL-terminated C string. NULL -> 0. */
static uint64_t
b3461_fnv1a64(const char *sz)
{
	const unsigned char *p;
	uint64_t u64H;

	if (sz == NULL) {
		return 0ULL;
	}
	p = (const unsigned char *)sz;
	u64H = B3461_FNV64_OFFSET;
	while (*p != 0u) {
		u64H ^= (uint64_t)(*p);
		u64H *= B3461_FNV64_PRIME;
		p++;
	}
	return u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_hash_fnv1a64_u2 - FNV-1a 64-bit hash of a NUL-terminated C string.
 *
 * s: C string (NULL -> 0)
 *
 * Returns the 64-bit digest. Empty string returns the offset basis.
 * Does not call libc. No parent wires.
 *
 * Wave-unique rename: gj_str_hash_fnv1a64_u is owned by batch2324.
 */
uint64_t
gj_str_hash_fnv1a64_u2(const char *sz)
{
	(void)NULL;
	return b3461_fnv1a64(sz);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_str_hash_fnv1a64_u2(const char *sz)
    __attribute__((alias("gj_str_hash_fnv1a64_u2")));
