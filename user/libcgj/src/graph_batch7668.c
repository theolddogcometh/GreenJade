/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7668: times tms field pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tms_field_pack_u_7668(uint32_t want_utime,
 *                                     uint32_t want_stime,
 *                                     uint32_t want_cutime,
 *                                     uint32_t want_cstime);
 *     - Soft pack: non-zero want_utime → bit0 (1), want_stime → bit1
 *       (2), want_cutime → bit2 (4), want_cstime → bit3 (8); OR
 *       selected presence bits.
 *   uint32_t __gj_tms_field_pack_u_7668  (alias)
 *   __libcgj_batch7668_marker = "libcgj-batch7668"
 *
 * Exclusive continuum CREATE-ONLY (7661-7670: times clock_t stubs —
 * clocks_per_sec, tms_utime_id, tms_stime_id, tms_cutime_id,
 * tms_cstime_id, is_utime, field_ok, field_pack, clock_t_errorish,
 * continuum + batch_id_7670). Unique gj_tms_field_pack_u_7668 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7668_marker[] = "libcgj-batch7668";

/* Soft presence bits for packed tms-field catalog (not field ids). */
#define B7668_PACK_UTIME   ((uint32_t)0x00000001u)
#define B7668_PACK_STIME   ((uint32_t)0x00000002u)
#define B7668_PACK_CUTIME  ((uint32_t)0x00000004u)
#define B7668_PACK_CSTIME  ((uint32_t)0x00000008u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7668_field_pack(uint32_t u32WantUtime, uint32_t u32WantStime,
    uint32_t u32WantCutime, uint32_t u32WantCstime)
{
	uint32_t u32Out = 0u;

	if (u32WantUtime != 0u) {
		u32Out |= B7668_PACK_UTIME;
	}
	if (u32WantStime != 0u) {
		u32Out |= B7668_PACK_STIME;
	}
	if (u32WantCutime != 0u) {
		u32Out |= B7668_PACK_CUTIME;
	}
	if (u32WantCstime != 0u) {
		u32Out |= B7668_PACK_CSTIME;
	}
	return u32Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tms_field_pack_u_7668 - soft pack of tms-field request bits.
 *
 * want_utime:  non-zero to include tms_utime presence bit
 * want_stime:  non-zero to include tms_stime presence bit
 * want_cutime: non-zero to include tms_cutime presence bit
 * want_cstime: non-zero to include tms_cstime presence bit
 *
 * Returns packed presence mask (bits 0..3). Pure integer; does not call
 * times(). No parent wires.
 */
uint32_t
gj_tms_field_pack_u_7668(uint32_t want_utime, uint32_t want_stime,
    uint32_t want_cutime, uint32_t want_cstime)
{
	(void)NULL;
	return b7668_field_pack(want_utime, want_stime, want_cutime,
	    want_cstime);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tms_field_pack_u_7668(uint32_t want_utime, uint32_t want_stime,
    uint32_t want_cutime, uint32_t want_cstime)
    __attribute__((alias("gj_tms_field_pack_u_7668")));
