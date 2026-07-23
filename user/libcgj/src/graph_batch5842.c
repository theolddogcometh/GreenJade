/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5842: MIT license dual-assert half.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mit_license_assert_5842(void);
 *     - Returns 1 (MIT license half of the dual-assert pair is green).
 *       Soft compile-time dual-license policy tag; not a file scan.
 *   uint32_t __gj_mit_license_assert_5842  (alias)
 *   __libcgj_batch5842_marker = "libcgj-batch5842"
 *
 * Exclusive continuum CREATE-ONLY (5841-5850: clean-room license
 * dual-assert continuum — cleanroom_present_5841,
 * mit_license_assert_5842, apache_license_assert_5843,
 * dual_license_assert_5844, cleanroom_source_assert_5845,
 * no_third_party_copy_5846, spdx_dual_ok_5847,
 * dual_assert_green_5848, cleanroom_license_ready_5849,
 * batch_id_5850). Unique gj_mit_license_assert_5842 surface only;
 * no multi-def. Distinct from gj_dual_license_gate_5477 and
 * gj_dual_license_ok_u. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5842_marker[] = "libcgj-batch5842";

/* MIT license half of dual-assert pair is green. */
#define B5842_MIT_LICENSE_ASSERT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5842_mit_assert(void)
{
	return B5842_MIT_LICENSE_ASSERT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mit_license_assert_5842 - report MIT license dual-assert half.
 *
 * Always returns 1 (MIT half green). Soft pure-data constant; does not
 * parse LICENSE files. Does not call libc. No parent wires.
 */
uint32_t
gj_mit_license_assert_5842(void)
{
	(void)NULL;
	return b5842_mit_assert();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mit_license_assert_5842(void)
    __attribute__((alias("gj_mit_license_assert_5842")));
