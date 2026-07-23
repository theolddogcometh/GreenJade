/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4717: size_t value hash cascade (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_size_u(size_t n);
 *     - Mix a size_t length / count into a 64-bit hash word with a
 *       cascade finalizer. Pure; no buffer walk.
 *   uint64_t __gj_hash_size_u  (alias)
 *   __libcgj_batch4717_marker = "libcgj-batch4717"
 *
 * Exclusive continuum CREATE-ONLY (4711-4720). Unique gj_hash_size_u
 * surface only; no multi-def. Distinct from gj_hash_ptr_u (batch4716)
 * and buffer length folds inside string hashes. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4717_marker[] = "libcgj-batch4717";

/* Cascade-unique size mix constants. */
#define B4717_BIAS  0x9e3779b97f4a7c15ULL
#define B4717_M0    0xfc73c997b2c2a0e5ULL
#define B4717_M1    0xbea1b1c3f6f4e9b9ULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4717_hash_size(size_t n)
{
	uint64_t u64H;

	u64H = (uint64_t)n + B4717_BIAS;
	u64H ^= u64H >> 33;
	u64H *= B4717_M0;
	u64H ^= u64H >> 29;
	u64H *= B4717_M1;
	u64H ^= u64H >> 32;
	return u64H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_size_u - hash a size_t value into a 64-bit word.
 *
 * n: length, count, or other size_t quantity (all values accepted)
 *
 * Returns a well-mixed 64-bit hash of n. Stateless pure function.
 * No parent wires.
 */
uint64_t
gj_hash_size_u(size_t n)
{
	(void)NULL;
	return b4717_hash_size(n);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_size_u(size_t n)
    __attribute__((alias("gj_hash_size_u")));
