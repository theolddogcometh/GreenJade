/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5841: clean-room claim present lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cleanroom_present_5841(void);
 *     - Returns 1 (clean-room freestanding product claim is present
 *       for the license dual-assert continuum). Soft compile-time tag.
 *   uint32_t __gj_cleanroom_present_5841  (alias)
 *   __libcgj_batch5841_marker = "libcgj-batch5841"
 *
 * Exclusive continuum CREATE-ONLY (5841-5850: clean-room license
 * dual-assert continuum — cleanroom_present_5841,
 * mit_license_assert_5842, apache_license_assert_5843,
 * dual_license_assert_5844, cleanroom_source_assert_5845,
 * no_third_party_copy_5846, spdx_dual_ok_5847,
 * dual_assert_green_5848, cleanroom_license_ready_5849,
 * batch_id_5850). Unique gj_cleanroom_present_5841 surface only;
 * no multi-def. Distinct from gj_cleanroom_ok_u (batch5387) and
 * gj_cleanroom_abi_marker_5478. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5841_marker[] = "libcgj-batch5841";

/* Clean-room freestanding product claim present for this wave. */
#define B5841_CLEANROOM_PRESENT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5841_cleanroom_present(void)
{
	return B5841_CLEANROOM_PRESENT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cleanroom_present_5841 - report clean-room claim present lamp.
 *
 * Always returns 1 (claim present). Soft pure-data constant for the
 * clean-room license dual-assert continuum. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_cleanroom_present_5841(void)
{
	(void)NULL;
	return b5841_cleanroom_present();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cleanroom_present_5841(void)
    __attribute__((alias("gj_cleanroom_present_5841")));
