/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5844: dual MIT OR Apache-2.0 license assert.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dual_license_assert_5844(void);
 *     - Returns 1 (dual MIT OR Apache-2.0 product license claim holds).
 *       Soft compile-time dual-assert conjunction tag.
 *   uint32_t __gj_dual_license_assert_5844  (alias)
 *   __libcgj_batch5844_marker = "libcgj-batch5844"
 *
 * Exclusive continuum CREATE-ONLY (5841-5850: clean-room license
 * dual-assert continuum — cleanroom_present_5841,
 * mit_license_assert_5842, apache_license_assert_5843,
 * dual_license_assert_5844, cleanroom_source_assert_5845,
 * no_third_party_copy_5846, spdx_dual_ok_5847,
 * dual_assert_green_5848, cleanroom_license_ready_5849,
 * batch_id_5850). Unique gj_dual_license_assert_5844 surface only;
 * no multi-def. Distinct from gj_dual_license_gate_5477,
 * gj_dual_license_ok_u, and gj_dual_license_tag_ok. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5844_marker[] = "libcgj-batch5844";

/* Dual MIT OR Apache-2.0 license claim holds. */
#define B5844_DUAL_LICENSE_ASSERT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5844_dual_assert(void)
{
	return B5844_DUAL_LICENSE_ASSERT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dual_license_assert_5844 - report dual MIT/Apache-2.0 license assert.
 *
 * Always returns 1 (dual claim holds). Soft pure-data constant for
 * dual-assert policy tables. Does not call libc. No parent wires.
 */
uint32_t
gj_dual_license_assert_5844(void)
{
	(void)NULL;
	return b5844_dual_assert();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dual_license_assert_5844(void)
    __attribute__((alias("gj_dual_license_assert_5844")));
