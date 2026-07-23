/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7687: timeval usec-to-sec carry stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tv_usec_carry_u_7687(uint32_t usec);
 *     - Return whole seconds carried from usec (usec / 1000000).
 *   uint32_t __gj_tv_usec_carry_u_7687  (alias)
 *   __libcgj_batch7687_marker = "libcgj-batch7687"
 *
 * Exclusive continuum CREATE-ONLY (7681-7690: settimeofday stubs —
 * usec_ok, sec_ok, valid, is_zero, is_pos, usec_per_sec, usec_carry,
 * stime_usec_zero, req_ok, continuum + batch_id_7690). Unique
 * gj_tv_usec_carry_u_7687 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7687_marker[] = "libcgj-batch7687";

/* Soft microseconds per second for carry division (1e6). */
#define B7687_USEC_PER_SEC  ((uint32_t)1000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7687_tv_usec_carry(uint32_t u32Usec)
{
	return u32Usec / B7687_USEC_PER_SEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tv_usec_carry_u_7687 - whole seconds carried out of usec.
 *
 * usec: soft microsecond total that may exceed 1e6-1
 *
 * Returns floor(usec / 1e6). Used when normalizing an unnormalized
 * timeval before settimeofday. Pure integer divide; no parent wires.
 * Does not call settimeofday.
 */
uint32_t
gj_tv_usec_carry_u_7687(uint32_t u32Usec)
{
	(void)NULL;
	return b7687_tv_usec_carry(u32Usec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tv_usec_carry_u_7687(uint32_t u32Usec)
    __attribute__((alias("gj_tv_usec_carry_u_7687")));
