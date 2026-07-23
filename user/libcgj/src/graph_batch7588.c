/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7588: timespec nsec-to-sec carry stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ts_nsec_carry_u_7588(uint32_t nsec);
 *     - Return whole seconds carried from nsec (nsec / 1000000000).
 *   uint32_t __gj_ts_nsec_carry_u_7588  (alias)
 *   __libcgj_batch7588_marker = "libcgj-batch7588"
 *
 * Exclusive continuum CREATE-ONLY (7581-7590: nanosleep timespec stubs —
 * nsec_ok, sec_ok, valid, is_zero, is_pos, abstime, nsec_per_sec,
 * nsec_carry, req_ok, continuum + batch_id_7590). Unique
 * gj_ts_nsec_carry_u_7588 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7588_marker[] = "libcgj-batch7588";

/* Soft nanoseconds per second for carry division (1e9). */
#define B7588_NSEC_PER_SEC  ((uint32_t)1000000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7588_ts_nsec_carry(uint32_t u32Nsec)
{
	return u32Nsec / B7588_NSEC_PER_SEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ts_nsec_carry_u_7588 - whole seconds carried out of nsec.
 *
 * nsec: soft nanosecond total that may exceed 1e9-1
 *
 * Returns floor(nsec / 1e9). Used when normalizing an unnormalized
 * timespec before nanosleep. Pure integer divide; no parent wires.
 * Does not call nanosleep.
 */
uint32_t
gj_ts_nsec_carry_u_7588(uint32_t u32Nsec)
{
	(void)NULL;
	return b7588_ts_nsec_carry(u32Nsec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ts_nsec_carry_u_7588(uint32_t u32Nsec)
    __attribute__((alias("gj_ts_nsec_carry_u_7588")));
