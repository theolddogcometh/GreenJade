/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7664: times tms_cutime field id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tms_cutime_id_7664(void);
 *     - Return soft tms_cutime field catalog id (2): children user CPU.
 *   uint32_t __gj_tms_cutime_id_7664  (alias)
 *   __libcgj_batch7664_marker = "libcgj-batch7664"
 *
 * Exclusive continuum CREATE-ONLY (7661-7670: times clock_t stubs —
 * clocks_per_sec, tms_utime_id, tms_stime_id, tms_cutime_id,
 * tms_cstime_id, is_utime, field_ok, field_pack, clock_t_errorish,
 * continuum + batch_id_7670). Unique gj_tms_cutime_id_7664 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7664_marker[] = "libcgj-batch7664";

/* Soft struct tms field catalog: tms_cutime index 2. */
#define B7664_TMS_CUTIME_ID  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7664_cutime_id(void)
{
	return B7664_TMS_CUTIME_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tms_cutime_id_7664 - soft tms_cutime field catalog id.
 *
 * Always returns 2. Catalog id only; does not call times().
 * No parent wires.
 */
uint32_t
gj_tms_cutime_id_7664(void)
{
	(void)NULL;
	return b7664_cutime_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tms_cutime_id_7664(void)
    __attribute__((alias("gj_tms_cutime_id_7664")));
