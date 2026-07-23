/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7667: times tms field validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tms_field_ok_u_7667(uint32_t field_id);
 *     - Return 1 if field_id is a known soft tms field
 *       (utime|stime|cutime|cstime), else 0.
 *   uint32_t __gj_tms_field_ok_u_7667  (alias)
 *   __libcgj_batch7667_marker = "libcgj-batch7667"
 *
 * Exclusive continuum CREATE-ONLY (7661-7670: times clock_t stubs —
 * clocks_per_sec, tms_utime_id, tms_stime_id, tms_cutime_id,
 * tms_cstime_id, is_utime, field_ok, field_pack, clock_t_errorish,
 * continuum + batch_id_7670). Unique gj_tms_field_ok_u_7667 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7667_marker[] = "libcgj-batch7667";

/* Soft known tms field ids: utime=0, stime=1, cutime=2, cstime=3. */
#define B7667_TMS_UTIME   ((uint32_t)0u)
#define B7667_TMS_STIME   ((uint32_t)1u)
#define B7667_TMS_CUTIME  ((uint32_t)2u)
#define B7667_TMS_CSTIME  ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7667_field_ok(uint32_t u32FieldId)
{
	if (u32FieldId == B7667_TMS_UTIME) {
		return 1u;
	}
	if (u32FieldId == B7667_TMS_STIME) {
		return 1u;
	}
	if (u32FieldId == B7667_TMS_CUTIME) {
		return 1u;
	}
	if (u32FieldId == B7667_TMS_CSTIME) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tms_field_ok_u_7667 - 1 if field_id is a known soft tms field.
 *
 * field_id: soft struct tms field catalog id
 *
 * Returns 1 when field_id is 0..3; else 0. Soft catalog check; does not
 * call times(). No parent wires.
 */
uint32_t
gj_tms_field_ok_u_7667(uint32_t field_id)
{
	(void)NULL;
	return b7667_field_ok(field_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tms_field_ok_u_7667(uint32_t field_id)
    __attribute__((alias("gj_tms_field_ok_u_7667")));
