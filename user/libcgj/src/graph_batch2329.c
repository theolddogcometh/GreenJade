/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2329: freestanding pointer-to-seed hash mix
 * (post-2320 hash exclusive surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_seed_from_ptr(const void *p);
 *     - Derive a 64-bit hash seed from pointer p by mixing its address
 *       bits (Murmur fmix64-style). NULL p -> 0. Pure; no deref of p.
 *   uint64_t __gj_hash_seed_from_ptr  (alias)
 *   __libcgj_batch2329_marker = "libcgj-batch2329"
 *
 * Post-2320 hash exclusive wave (2321-2330). Distinct from
 * gj_hash_seed_mix_u64 (batch1709 combine) and pure mixers that take
 * integer inputs only - unique gj_hash_seed_from_ptr surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2329_marker[] = "libcgj-batch2329";

/* MurmurHash3 fmix64 constants (public algorithm). */
#define B2329_M1 0xff51afd7ed558ccdULL
#define B2329_M2 0xc4ceb9fe1a85ec53ULL

/* ---- freestanding helpers ---------------------------------------------- */

/* Mix pointer address bits into a 64-bit seed. NULL -> 0. */
static uint64_t
b2329_seed_from_ptr(const void *p)
{
	uint64_t u64H;

	if (p == NULL) {
		return 0ULL;
	}
	u64H = (uint64_t)(uintptr_t)p;
	u64H ^= u64H >> 33;
	u64H *= B2329_M1;
	u64H ^= u64H >> 33;
	u64H *= B2329_M2;
	u64H ^= u64H >> 33;
	return u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_seed_from_ptr - derive a 64-bit hash seed from pointer p.
 *
 * p: arbitrary pointer (NULL -> 0). Not dereferenced.
 *
 * Returns a mixed 64-bit seed derived from the address bits. Does not
 * call libc.
 */
uint64_t
gj_hash_seed_from_ptr(const void *p)
{
	(void)NULL;
	return b2329_seed_from_ptr(p);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_seed_from_ptr(const void *p)
    __attribute__((alias("gj_hash_seed_from_ptr")));
