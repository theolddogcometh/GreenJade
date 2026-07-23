/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2059: 64-bit xorshift then high/low fold.
 *
 * Surface (unique symbols):
 *   uint64_t gj_hash_xorshift_fold64(uint64_t x);
 *     - Apply the classic triple xorshift (<<13, >>7, <<17), then fold
 *       high and low 32-bit halves with xor so both halves influence
 *       the low half (high half cleared by the fold). Pure; no buffer.
 *   uint64_t __gj_hash_xorshift_fold64  (alias)
 *   __libcgj_batch2059_marker = "libcgj-batch2059"
 *
 * Post-2050 hash-mix exclusive wave (2051-2060). Distinct from
 * gj_hash_mix64 / avalanche / finalizer / splitmix mixers - this is
 * xorshift+fold only. Unique gj_hash_xorshift_fold64 surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2059_marker[] = "libcgj-batch2059";

/* ---- freestanding helpers ---------------------------------------------- */

/* Classic 64-bit xorshift triple (Marsaglia-family shift set). */
static uint64_t
b2059_xorshift(uint64_t u64X)
{
	u64X ^= u64X << 13;
	u64X ^= u64X >> 7;
	u64X ^= u64X << 17;
	return u64X;
}

/* Fold high half into low half; result lives in low 32 bits of u64. */
static uint64_t
b2059_fold(uint64_t u64X)
{
	return (u64X ^ (u64X >> 32)) & 0xffffffffULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hash_xorshift_fold64 - xorshift then fold high/low of x.
 *
 * x: arbitrary 64-bit input (all values accepted, including 0)
 *
 * Returns a 64-bit value with only the low 32 bits potentially set
 * (high half zero after fold). Stateless pure function.
 */
uint64_t
gj_hash_xorshift_fold64(uint64_t u64X)
{
	(void)NULL;
	return b2059_fold(b2059_xorshift(u64X));
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hash_xorshift_fold64(uint64_t u64X)
    __attribute__((alias("gj_hash_xorshift_fold64")));
