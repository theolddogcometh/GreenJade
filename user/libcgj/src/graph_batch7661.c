/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7661: times/clock_t CLOCKS_PER_SEC soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clocks_per_sec_u_7661(void);
 *     - Return soft CLOCKS_PER_SEC (1000000): clock_t ticks per second.
 *   uint32_t __gj_clocks_per_sec_u_7661  (alias)
 *   __libcgj_batch7661_marker = "libcgj-batch7661"
 *
 * Exclusive continuum CREATE-ONLY (7661-7670: times clock_t stubs —
 * clocks_per_sec, tms_utime_id, tms_stime_id, tms_cutime_id,
 * tms_cstime_id, is_utime, field_ok, field_pack, clock_t_errorish,
 * continuum + batch_id_7670). Unique gj_clocks_per_sec_u_7661 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7661_marker[] = "libcgj-batch7661";

/* Soft CLOCKS_PER_SEC as used by XSI / common glibc desktop clock_t. */
#define B7661_CLOCKS_PER_SEC  ((uint32_t)1000000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7661_clocks_per_sec(void)
{
	return B7661_CLOCKS_PER_SEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clocks_per_sec_u_7661 - soft CLOCKS_PER_SEC constant.
 *
 * Always returns 1000000. Catalog value only; does not call clock/times.
 * No parent wires.
 */
uint32_t
gj_clocks_per_sec_u_7661(void)
{
	(void)NULL;
	return b7661_clocks_per_sec();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clocks_per_sec_u_7661(void)
    __attribute__((alias("gj_clocks_per_sec_u_7661")));
