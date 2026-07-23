/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7666: times tms field is-utime stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tms_is_utime_u_7666(uint32_t field_id);
 *     - Return 1 if field_id == tms_utime (0), else 0.
 *   uint32_t __gj_tms_is_utime_u_7666  (alias)
 *   __libcgj_batch7666_marker = "libcgj-batch7666"
 *
 * Exclusive continuum CREATE-ONLY (7661-7670: times clock_t stubs —
 * clocks_per_sec, tms_utime_id, tms_stime_id, tms_cutime_id,
 * tms_cstime_id, is_utime, field_ok, field_pack, clock_t_errorish,
 * continuum + batch_id_7670). Unique gj_tms_is_utime_u_7666 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7666_marker[] = "libcgj-batch7666";

/* Soft tms_utime catalog id (matches batch7662). */
#define B7666_TMS_UTIME_ID  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7666_is_utime(uint32_t u32FieldId)
{
	if (u32FieldId == B7666_TMS_UTIME_ID) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tms_is_utime_u_7666 - 1 if field_id names soft tms_utime.
 *
 * field_id: soft struct tms field catalog id
 *
 * Returns 1 when field_id is 0; else 0. Soft catalog check; does not
 * call times(). No parent wires.
 */
uint32_t
gj_tms_is_utime_u_7666(uint32_t field_id)
{
	(void)NULL;
	return b7666_is_utime(field_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tms_is_utime_u_7666(uint32_t field_id)
    __attribute__((alias("gj_tms_is_utime_u_7666")));
