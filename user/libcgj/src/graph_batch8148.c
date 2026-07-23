/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8148: strstr params present ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strstr_params_ok_u_8148(uint32_t hay_present,
 *                                       uint32_t needle_present);
 *     - Return 1 if both hay and needle soft-presence tags are non-zero.
 *   uint32_t __gj_strstr_params_ok_u_8148  (alias)
 *   __libcgj_batch8148_marker = "libcgj-batch8148"
 *
 * Exclusive continuum CREATE-ONLY (8141-8150: strstr needle stubs —
 * empty_needle_ok, not_found_id, match_zero_id, needle_is_empty,
 * hay_is_empty, needle_fits, offset_ok, params_ok, needle_errorish,
 * batch_id_8150). Unique gj_strstr_params_ok_u_8148 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8148_marker[] = "libcgj-batch8148";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8148_params_ok(uint32_t u32HayPresent, uint32_t u32NeedlePresent)
{
	/* Soft presence: non-zero tag means pointer catalogued as present. */
	if (u32HayPresent == 0u || u32NeedlePresent == 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strstr_params_ok_u_8148 - 1 if hay and needle soft-present.
 *
 * hay_present:    non-zero if haystack pointer is catalogued present
 * needle_present: non-zero if needle pointer is catalogued present
 *
 * Returns 1 when both tags are non-zero, else 0. Soft catalog test;
 * does not dereference pointers or call strstr(3). No parent wires.
 */
uint32_t
gj_strstr_params_ok_u_8148(uint32_t u32HayPresent, uint32_t u32NeedlePresent)
{
	(void)NULL;
	return b8148_params_ok(u32HayPresent, u32NeedlePresent);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strstr_params_ok_u_8148(uint32_t u32HayPresent,
    uint32_t u32NeedlePresent)
    __attribute__((alias("gj_strstr_params_ok_u_8148")));
