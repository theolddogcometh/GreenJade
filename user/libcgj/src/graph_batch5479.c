/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5479: no-GPL-in-tree assertion helper.
 *
 * Surface (unique symbols):
 *   uint32_t gj_no_gpl_assert_5479(void);
 *     - Return 1 when the product tree asserts no GPL-contaminated
 *       source in this continuum wave. Soft compile-time policy flag.
 *   uint32_t __gj_no_gpl_assert_5479  (alias)
 *   __libcgj_batch5479_marker = "libcgj-batch5479"
 *
 * Exclusive continuum CREATE-ONLY (5471-5480: libc soname gates /
 * GLIBC_2.x version nodes, dual MIT/Apache license gates, clean-room
 * ABI markers, no-GPL assertion helpers —
 * glibc_vernode_2_0_5471, glibc_vernode_2_2_5472,
 * glibc_vernode_2_17_5473, glibc_vernode_2_27_5474,
 * glibc_vernode_2_34_5475, libc_soname_gate_5476,
 * dual_license_gate_5477, cleanroom_abi_marker_5478,
 * no_gpl_assert_5479, batch_id_5480). Unique
 * gj_no_gpl_assert_5479 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5479_marker[] = "libcgj-batch5479";

/* No-GPL-in-tree product assertion green. */
#define B5479_NO_GPL_ASSERT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5479_no_gpl_assert(void)
{
	return B5479_NO_GPL_ASSERT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_no_gpl_assert_5479 - report no-GPL-in-tree product assertion.
 *
 * Always returns 1 (claim holds). Soft pure-data constant for license
 * policy tables. Self-contained; no parent wires.
 */
uint32_t
gj_no_gpl_assert_5479(void)
{
	(void)NULL;
	return b5479_no_gpl_assert();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_no_gpl_assert_5479(void)
    __attribute__((alias("gj_no_gpl_assert_5479")));
