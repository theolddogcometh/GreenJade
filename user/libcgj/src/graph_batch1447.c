/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1447: Bob Jenkins SmallFast (JSF) 64-bit PRNG.
 *
 * Surface (unique symbols):
 *   uint64_t gj_jsf64_next(uint64_t s[4]);
 *     — Advance a 4-word JSF64 state (a,b,c,d) in place and return the
 *       next 64-bit sample (d after the step). s may not be NULL
 *       (returns 0 if NULL).
 *   uint64_t __gj_jsf64_next  (alias)
 *   __libcgj_batch1447_marker = "libcgj-batch1447"
 *
 * Distinct from xoshiro / sfc / romu batches. Unique surface only; no
 * multi-def. Avoids splitmix/xorshift/pcg names.
 *
 * Algorithm (Bob Jenkins JSF64, public specification):
 *   e = a - rotl(b, 7)
 *   a = b ^ rotl(c, 13)
 *   b = c + rotl(d, 37)
 *   c = d + e
 *   d = e + a
 *   return d
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1447_marker[] = "libcgj-batch1447";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1447_rotl64(uint64_t u64X, unsigned uBits)
{
	return (u64X << uBits) | (u64X >> (64u - uBits));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_jsf64_next — one JSF64 step on s[0..3] = {a, b, c, d}.
 *
 * pS: state array of four uint64_t words (mutated in place).
 *     NULL → returns 0 without writing.
 *
 * Returns the updated d word (standard JSF64 output).
 * Caller should seed so the state is not degenerate (e.g. not all zero).
 */
uint64_t
gj_jsf64_next(uint64_t pS[4])
{
	uint64_t u64E;
	uint64_t u64A;
	uint64_t u64B;
	uint64_t u64C;
	uint64_t u64D;

	if (pS == NULL) {
		return 0ULL;
	}

	u64A = pS[0];
	u64B = pS[1];
	u64C = pS[2];
	u64D = pS[3];

	u64E = u64A - b1447_rotl64(u64B, 7u);
	u64A = u64B ^ b1447_rotl64(u64C, 13u);
	u64B = u64C + b1447_rotl64(u64D, 37u);
	u64C = u64D + u64E;
	u64D = u64E + u64A;

	pS[0] = u64A;
	pS[1] = u64B;
	pS[2] = u64C;
	pS[3] = u64D;

	return u64D;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_jsf64_next(uint64_t pS[4])
    __attribute__((alias("gj_jsf64_next")));
