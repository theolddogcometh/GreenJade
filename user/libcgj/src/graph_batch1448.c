/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1448: SFC64 (Small Fast Counter) PRNG.
 *
 * Surface (unique symbols):
 *   uint64_t gj_sfc64_next(uint64_t s[4]);
 *     — Advance a 4-word SFC64 state (a, b, c, counter) in place and
 *       return the next 64-bit sample. s may not be NULL (returns 0
 *       if NULL).
 *   uint64_t __gj_sfc64_next  (alias)
 *   __libcgj_batch1448_marker = "libcgj-batch1448"
 *
 * Distinct from jsf64 / xoshiro / romu batches. Unique surface only;
 * no multi-def. Avoids splitmix/xorshift/pcg names.
 *
 * Algorithm (Chris Doty-Humphrey SFC64, public specification):
 *   tmp = a + b + counter++
 *   a = b ^ (b >> 11)
 *   b = c + (c << 3)
 *   c = rotl(c, 24) + tmp
 *   return tmp
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1448_marker[] = "libcgj-batch1448";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1448_rotl64(uint64_t u64X, unsigned uBits)
{
	return (u64X << uBits) | (u64X >> (64u - uBits));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sfc64_next — one SFC64 step on s[0..3] = {a, b, c, counter}.
 *
 * pS: state array of four uint64_t words (mutated in place).
 *     NULL → returns 0 without writing.
 *
 * Returns tmp = a + b + counter (pre-increment value path as specified).
 * Caller should seed a/b/c non-trivially; counter may start at 1.
 */
uint64_t
gj_sfc64_next(uint64_t pS[4])
{
	uint64_t u64Tmp;
	uint64_t u64A;
	uint64_t u64B;
	uint64_t u64C;
	uint64_t u64Ctr;

	if (pS == NULL) {
		return 0ULL;
	}

	u64A = pS[0];
	u64B = pS[1];
	u64C = pS[2];
	u64Ctr = pS[3];

	u64Tmp = u64A + u64B + u64Ctr;
	pS[3] = u64Ctr + 1ULL;
	pS[0] = u64B ^ (u64B >> 11);
	pS[1] = u64C + (u64C << 3);
	pS[2] = b1448_rotl64(u64C, 24u) + u64Tmp;

	return u64Tmp;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_sfc64_next(uint64_t pS[4])
    __attribute__((alias("gj_sfc64_next")));
