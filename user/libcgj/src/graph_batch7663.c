/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7663: times tms_stime field id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tms_stime_id_7663(void);
 *     - Return soft tms_stime field catalog id (1): system CPU time.
 *   uint32_t __gj_tms_stime_id_7663  (alias)
 *   __libcgj_batch7663_marker = "libcgj-batch7663"
 *
 * Exclusive continuum CREATE-ONLY (7661-7670: times clock_t stubs —
 * clocks_per_sec, tms_utime_id, tms_stime_id, tms_cutime_id,
 * tms_cstime_id, is_utime, field_ok, field_pack, clock_t_errorish,
 * continuum + batch_id_7670). Unique gj_tms_stime_id_7663 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7663_marker[] = "libcgj-batch7663";

/* Soft struct tms field catalog: tms_stime index 1. */
#define B7663_TMS_STIME_ID  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7663_stime_id(void)
{
	return B7663_TMS_STIME_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tms_stime_id_7663 - soft tms_stime field catalog id.
 *
 * Always returns 1. Catalog id only; does not call times().
 * No parent wires.
 */
uint32_t
gj_tms_stime_id_7663(void)
{
	(void)NULL;
	return b7663_stime_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tms_stime_id_7663(void)
    __attribute__((alias("gj_tms_stime_id_7663")));
