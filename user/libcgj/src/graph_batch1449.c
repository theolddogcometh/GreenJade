/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1449: RomuTrio PRNG (3-word state).
 *
 * Surface (unique symbols):
 *   uint64_t gj_romu_trio_next(uint64_t s[3]);
 *     — Advance a 3-word RomuTrio state (x, y, z) in place and return
 *       the previous x as the sample. s may not be NULL (returns 0 if
 *       NULL).
 *   uint64_t __gj_romu_trio_next  (alias)
 *   __libcgj_batch1449_marker = "libcgj-batch1449"
 *
 * Distinct from jsf64 / sfc64 / xoshiro batches. Unique surface only;
 * no multi-def. Avoids splitmix/xorshift/pcg names.
 *
 * Algorithm (Mark A. Overton RomuTrio, public specification):
 *   xp = x; yp = y; zp = z
 *   x = 15241094284759029579 * zp
 *   y = rotl(yp - xp, 12)
 *   z = rotl(zp - yp, 44)
 *   return xp
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1449_marker[] = "libcgj-batch1449";

/* RomuTrio public odd multiply constant. */
#define B1449_MULT 15241094284759029579ULL

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1449_rotl64(uint64_t u64X, unsigned uBits)
{
	return (u64X << uBits) | (u64X >> (64u - uBits));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_romu_trio_next — one RomuTrio step on s[0..2] = {x, y, z}.
 *
 * pS: state array of three uint64_t words (mutated in place).
 *     NULL → returns 0 without writing.
 *
 * Returns the pre-update x. Caller must seed so x|y|z != 0.
 */
uint64_t
gj_romu_trio_next(uint64_t pS[3])
{
	uint64_t u64Xp;
	uint64_t u64Yp;
	uint64_t u64Zp;

	if (pS == NULL) {
		return 0ULL;
	}

	u64Xp = pS[0];
	u64Yp = pS[1];
	u64Zp = pS[2];

	pS[0] = B1449_MULT * u64Zp;
	pS[1] = b1449_rotl64(u64Yp - u64Xp, 12u);
	pS[2] = b1449_rotl64(u64Zp - u64Yp, 44u);

	return u64Xp;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_romu_trio_next(uint64_t pS[3])
    __attribute__((alias("gj_romu_trio_next")));
