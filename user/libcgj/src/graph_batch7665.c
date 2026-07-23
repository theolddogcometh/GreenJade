/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7665: times tms_cstime field id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tms_cstime_id_7665(void);
 *     - Return soft tms_cstime field catalog id (3): children system CPU.
 *   uint32_t __gj_tms_cstime_id_7665  (alias)
 *   __libcgj_batch7665_marker = "libcgj-batch7665"
 *
 * Exclusive continuum CREATE-ONLY (7661-7670: times clock_t stubs —
 * clocks_per_sec, tms_utime_id, tms_stime_id, tms_cutime_id,
 * tms_cstime_id, is_utime, field_ok, field_pack, clock_t_errorish,
 * continuum + batch_id_7670). Unique gj_tms_cstime_id_7665 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7665_marker[] = "libcgj-batch7665";

/* Soft struct tms field catalog: tms_cstime index 3. */
#define B7665_TMS_CSTIME_ID  ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7665_cstime_id(void)
{
	return B7665_TMS_CSTIME_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tms_cstime_id_7665 - soft tms_cstime field catalog id.
 *
 * Always returns 3. Catalog id only; does not call times().
 * No parent wires.
 */
uint32_t
gj_tms_cstime_id_7665(void)
{
	(void)NULL;
	return b7665_cstime_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tms_cstime_id_7665(void)
    __attribute__((alias("gj_tms_cstime_id_7665")));
