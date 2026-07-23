/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8343: timespec nanosecond normalize stub (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_timespec_norm_u_8343(uint64_t *sec, uint64_t nsec);
 *     - Carry whole seconds out of nsec into *sec (saturating at
 *       UINT64_MAX). Return residual nsec in [0, 999999999].
 *       If sec is NULL, only the residual is returned (carry dropped).
 *   uint64_t __gj_timespec_norm_u_8343  (alias)
 *   __libcgj_batch8343_marker = "libcgj-batch8343"
 *
 * Exclusive continuum CREATE-ONLY (8341-8350: time/clock unit stubs).
 * Distinct from gj_timespec_cmp (batch697). Unique
 * gj_timespec_norm_u_8343 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8343_marker[] = "libcgj-batch8343";

/* Nanoseconds per second (SI: 1 s = 1e9 ns). */
#define B8343_NSEC_PER_SEC  1000000000ull

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b8343_norm — split nsec into (carry_sec, residual_nsec).
 * residual always in [0, 999999999]. Saturating add of carry into *sec
 * when sec is non-NULL.
 */
static uint64_t
b8343_norm(uint64_t *pSec, uint64_t u64Nsec)
{
	uint64_t u64Carry;
	uint64_t u64Rem;
	uint64_t u64Sum;

	u64Carry = u64Nsec / B8343_NSEC_PER_SEC;
	u64Rem = u64Nsec % B8343_NSEC_PER_SEC;

	if (pSec != NULL && u64Carry != 0ull) {
		u64Sum = *pSec + u64Carry;
		/* Saturate on wrap (unsigned overflow). */
		if (u64Sum < *pSec) {
			*pSec = UINT64_MAX;
		} else {
			*pSec = u64Sum;
		}
	}

	return u64Rem;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_timespec_norm_u_8343 - normalize timespec nsec with optional sec carry.
 *
 * sec:  optional in/out whole-seconds (NULL → drop carry)
 * nsec: raw nanoseconds (any magnitude)
 *
 * Returns residual nsec in [0, 999999999]. When sec is non-NULL, adds
 * floor(nsec / 1e9) into *sec, saturating at UINT64_MAX. Soft pure
 * integer; no parent wires.
 */
uint64_t
gj_timespec_norm_u_8343(uint64_t *sec, uint64_t nsec)
{
	(void)NULL;
	return b8343_norm(sec, nsec);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_timespec_norm_u_8343(uint64_t *sec, uint64_t nsec)
    __attribute__((alias("gj_timespec_norm_u_8343")));
