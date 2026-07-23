/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4716: pointer address hash cascade (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_ptr_u(const void *p);
 *     - Derive a 64-bit hash from pointer p by mixing its address bits.
 *       NULL p -> 0. Does not dereference p. Pure.
 *   uint64_t __gj_hash_ptr_u  (alias)
 *   __libcgj_batch4716_marker = "libcgj-batch4716"
 *
 * Exclusive continuum CREATE-ONLY (4711-4720). Unique gj_hash_ptr_u
 * surface only; no multi-def. Distinct from gj_hash_seed_from_ptr
 * (batch2329 Murmur fmix64 constants) — different multipliers and
 * shift schedule. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4716_marker[] = "libcgj-batch4716";

/* Cascade-unique pointer mix constants. */
#define B4716_M0  0xd6e8feb86659fd93ULL
#define B4716_M1  0xa5a755bfd6b6a9b5ULL

/* ---- freestanding helpers ---------------------------------------------- */

/* Mix pointer address bits into a 64-bit hash. NULL -> 0. */
static uint64_t
b4716_hash_ptr(const void *p)
{
	uint64_t u64H;

	if (p == NULL) {
		return 0ULL;
	}
	u64H = (uint64_t)(uintptr_t)p;
	/* Scramble low alignment bits into higher positions first. */
	u64H ^= u64H << 13;
	u64H ^= u64H >> 7;
	u64H ^= u64H << 17;
	u64H *= B4716_M0;
	u64H ^= u64H >> 32;
	u64H *= B4716_M1;
	u64H ^= u64H >> 29;
	return u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_ptr_u - hash a pointer's address bits into a 64-bit word.
 *
 * p: arbitrary pointer (NULL -> 0). Not dereferenced.
 *
 * Returns a mixed 64-bit hash derived from the address. Does not call
 * libc. No parent wires.
 */
uint64_t
gj_hash_ptr_u(const void *p)
{
	(void)NULL;
	return b4716_hash_ptr(p);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_ptr_u(const void *p)
    __attribute__((alias("gj_hash_ptr_u")));
