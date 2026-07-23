/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7662: times tms_utime field id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tms_utime_id_7662(void);
 *     - Return soft tms_utime field catalog id (0): user CPU time.
 *   uint32_t __gj_tms_utime_id_7662  (alias)
 *   __libcgj_batch7662_marker = "libcgj-batch7662"
 *
 * Exclusive continuum CREATE-ONLY (7661-7670: times clock_t stubs —
 * clocks_per_sec, tms_utime_id, tms_stime_id, tms_cutime_id,
 * tms_cstime_id, is_utime, field_ok, field_pack, clock_t_errorish,
 * continuum + batch_id_7670). Unique gj_tms_utime_id_7662 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7662_marker[] = "libcgj-batch7662";

/* Soft struct tms field catalog: tms_utime index 0. */
#define B7662_TMS_UTIME_ID  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7662_utime_id(void)
{
	return B7662_TMS_UTIME_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tms_utime_id_7662 - soft tms_utime field catalog id.
 *
 * Always returns 0. Catalog id only; does not call times().
 * No parent wires.
 */
uint32_t
gj_tms_utime_id_7662(void)
{
	(void)NULL;
	return b7662_utime_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tms_utime_id_7662(void)
    __attribute__((alias("gj_tms_utime_id_7662")));
