/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2324: freestanding NUL-terminated FNV-1a 64-bit
 * string hash (post-2320 hash exclusive surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_str_hash_fnv1a64_u(const char *s);
 *     - FNV-1a 64-bit over the C string at s until NUL. Offset basis
 *       14695981039346656037, prime 1099511628211. NULL s -> 0.
 *       Empty string -> offset basis.
 *   uint64_t __gj_str_hash_fnv1a64_u  (alias)
 *   __libcgj_batch2324_marker = "libcgj-batch2324"
 *
 * Post-2320 hash exclusive wave (2321-2330). Distinct from gj_fnv1a64 /
 * fnv1a_64 (length-bounded mem hashes) - unique gj_str_hash_fnv1a64_u
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2324_marker[] = "libcgj-batch2324";

/* FNV-1a 64-bit parameters (public domain constants). */
#define B2324_FNV64_OFFSET 14695981039346656037ULL
#define B2324_FNV64_PRIME  1099511628211ULL

/* ---- freestanding helpers ---------------------------------------------- */

/* FNV-1a 64 walk of a NUL-terminated C string. NULL -> 0. */
static uint64_t
b2324_fnv1a64_u(const char *sz)
{
	const unsigned char *p;
	uint64_t u64H;

	if (sz == NULL) {
		return 0ULL;
	}
	p = (const unsigned char *)sz;
	u64H = B2324_FNV64_OFFSET;
	while (*p != 0u) {
		u64H ^= (uint64_t)(*p);
		u64H *= B2324_FNV64_PRIME;
		p++;
	}
	return u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_hash_fnv1a64_u - FNV-1a 64-bit hash of a NUL-terminated C string.
 *
 * s: C string (NULL -> 0)
 *
 * Returns the 64-bit digest. Empty string returns the offset basis.
 * Does not call libc.
 */
uint64_t
gj_str_hash_fnv1a64_u(const char *sz)
{
	(void)NULL;
	return b2324_fnv1a64_u(sz);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_str_hash_fnv1a64_u(const char *sz)
    __attribute__((alias("gj_str_hash_fnv1a64_u")));
