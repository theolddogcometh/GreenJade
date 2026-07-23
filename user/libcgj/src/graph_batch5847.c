/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5847: SPDX dual MIT OR Apache-2.0 tag ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_spdx_dual_ok_5847(void);
 *     - Returns 1 (SPDX-License-Identifier: MIT OR Apache-2.0 product
 *       tag is accepted for this continuum wave). Soft compile-time
 *       SPDX dual-license readiness lamp.
 *   uint32_t __gj_spdx_dual_ok_5847  (alias)
 *   __libcgj_batch5847_marker = "libcgj-batch5847"
 *
 * Exclusive continuum CREATE-ONLY (5841-5850: clean-room license
 * dual-assert continuum — cleanroom_present_5841,
 * mit_license_assert_5842, apache_license_assert_5843,
 * dual_license_assert_5844, cleanroom_source_assert_5845,
 * no_third_party_copy_5846, spdx_dual_ok_5847,
 * dual_assert_green_5848, cleanroom_license_ready_5849,
 * batch_id_5850). Unique gj_spdx_dual_ok_5847 surface only;
 * no multi-def. Distinct from gj_dual_license_tag_ok and
 * gj_dual_license_assert_5844. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5847_marker[] = "libcgj-batch5847";

/* SPDX dual MIT OR Apache-2.0 product tag accepted. */
#define B5847_SPDX_DUAL_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5847_spdx_dual_ok(void)
{
	return B5847_SPDX_DUAL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_spdx_dual_ok_5847 - report SPDX dual-license tag readiness.
 *
 * Always returns 1 (SPDX dual tag ok). Soft pure-data constant; does
 * not parse SPDX headers. Does not call libc. No parent wires.
 */
uint32_t
gj_spdx_dual_ok_5847(void)
{
	(void)NULL;
	return b5847_spdx_dual_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_spdx_dual_ok_5847(void)
    __attribute__((alias("gj_spdx_dual_ok_5847")));
