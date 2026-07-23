/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8342: time nanoseconds validity stub (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_time_nsec_ok_u_8342(uint64_t nsec);
 *     - Soft predicate: 1 if nsec is a normalized nanosecond field
 *       in [0, 999999999], else 0.
 *   uint32_t __gj_time_nsec_ok_u_8342  (alias)
 *   __libcgj_batch8342_marker = "libcgj-batch8342"
 *
 * Exclusive continuum CREATE-ONLY (8341-8350: time/clock unit stubs).
 * Unique gj_time_nsec_ok_u_8342 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8342_marker[] = "libcgj-batch8342";

/* Nanoseconds per second (SI); valid nsec fields are strictly below this. */
#define B8342_NSEC_PER_SEC  1000000000ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8342_nsec_ok(uint64_t u64Nsec)
{
	return (u64Nsec < B8342_NSEC_PER_SEC) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_time_nsec_ok_u_8342 - soft nanoseconds-field validity predicate.
 *
 * nsec: candidate nanosecond component of a timespec-shaped value
 *
 * Returns 1 when nsec is in [0, 999999999], else 0. Soft pure-integer
 * unit stub; no parent wires.
 */
uint32_t
gj_time_nsec_ok_u_8342(uint64_t nsec)
{
	(void)NULL;
	return b8342_nsec_ok(nsec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_time_nsec_ok_u_8342(uint64_t nsec)
    __attribute__((alias("gj_time_nsec_ok_u_8342")));
