/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5846: no third-party source copy assert.
 *
 * Surface (unique symbols):
 *   uint32_t gj_no_third_party_copy_5846(void);
 *     - Returns 1 (no third-party source copied into this continuum
 *       wave). Soft compile-time clean-room purity tag.
 *   uint32_t __gj_no_third_party_copy_5846  (alias)
 *   __libcgj_batch5846_marker = "libcgj-batch5846"
 *
 * Exclusive continuum CREATE-ONLY (5841-5850: clean-room license
 * dual-assert continuum — cleanroom_present_5841,
 * mit_license_assert_5842, apache_license_assert_5843,
 * dual_license_assert_5844, cleanroom_source_assert_5845,
 * no_third_party_copy_5846, spdx_dual_ok_5847,
 * dual_assert_green_5848, cleanroom_license_ready_5849,
 * batch_id_5850). Unique gj_no_third_party_copy_5846 surface only;
 * no multi-def. Distinct from gj_no_gpl_assert_5479 and
 * gj_no_gpl_in_tree_u. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5846_marker[] = "libcgj-batch5846";

/* No third-party source copied into this continuum wave. */
#define B5846_NO_THIRD_PARTY_COPY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5846_no_copy(void)
{
	return B5846_NO_THIRD_PARTY_COPY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_no_third_party_copy_5846 - report no third-party source copy assert.
 *
 * Always returns 1 (no third-party source copied). Soft pure-data
 * clean-room purity tag. Does not call libc. No parent wires.
 */
uint32_t
gj_no_third_party_copy_5846(void)
{
	(void)NULL;
	return b5846_no_copy();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_no_third_party_copy_5846(void)
    __attribute__((alias("gj_no_third_party_copy_5846")));
