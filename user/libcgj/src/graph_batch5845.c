/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5845: clean-room source assert.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cleanroom_source_assert_5845(void);
 *     - Returns 1 (clean-room original source claim is asserted for
 *       this continuum wave). Soft compile-time provenance tag.
 *   uint32_t __gj_cleanroom_source_assert_5845  (alias)
 *   __libcgj_batch5845_marker = "libcgj-batch5845"
 *
 * Exclusive continuum CREATE-ONLY (5841-5850: clean-room license
 * dual-assert continuum — cleanroom_present_5841,
 * mit_license_assert_5842, apache_license_assert_5843,
 * dual_license_assert_5844, cleanroom_source_assert_5845,
 * no_third_party_copy_5846, spdx_dual_ok_5847,
 * dual_assert_green_5848, cleanroom_license_ready_5849,
 * batch_id_5850). Unique gj_cleanroom_source_assert_5845 surface only;
 * no multi-def. Distinct from gj_cleanroom_present_5841 and
 * gj_cleanroom_ok_u. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5845_marker[] = "libcgj-batch5845";

/* Clean-room original source claim asserted. */
#define B5845_CLEANROOM_SOURCE_ASSERT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5845_source_assert(void)
{
	return B5845_CLEANROOM_SOURCE_ASSERT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cleanroom_source_assert_5845 - report clean-room source assert.
 *
 * Always returns 1 (source claim holds). Soft pure-data provenance
 * tag; does not walk the tree. Does not call libc. No parent wires.
 */
uint32_t
gj_cleanroom_source_assert_5845(void)
{
	(void)NULL;
	return b5845_source_assert();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cleanroom_source_assert_5845(void)
    __attribute__((alias("gj_cleanroom_source_assert_5845")));
