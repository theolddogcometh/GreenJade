/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5472: GLIBC_2.2 version-node probe helper.
 *
 * Surface (unique symbols):
 *   uint32_t gj_glibc_vernode_2_2_5472(void);
 *     - Return packed GLIBC_2.2 version-node tag (major=2, minor=2 as
 *       0x00020002). Soft compile-time soname / version-script probe.
 *   uint32_t __gj_glibc_vernode_2_2_5472  (alias)
 *   __libcgj_batch5472_marker = "libcgj-batch5472"
 *
 * Exclusive continuum CREATE-ONLY (5471-5480: libc soname gates /
 * GLIBC_2.x version nodes, dual MIT/Apache license gates, clean-room
 * ABI markers, no-GPL assertion helpers —
 * glibc_vernode_2_0_5471, glibc_vernode_2_2_5472,
 * glibc_vernode_2_17_5473, glibc_vernode_2_27_5474,
 * glibc_vernode_2_34_5475, libc_soname_gate_5476,
 * dual_license_gate_5477, cleanroom_abi_marker_5478,
 * no_gpl_assert_5479, batch_id_5480). Unique
 * gj_glibc_vernode_2_2_5472 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5472_marker[] = "libcgj-batch5472";

/* Packed GLIBC_2.2 version node: (major << 16) | minor. */
#define B5472_VERNODE_2_2  ((uint32_t)0x00020002u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5472_vernode_2_2(void)
{
	return B5472_VERNODE_2_2;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glibc_vernode_2_2_5472 - report GLIBC_2.2 version-node probe tag.
 *
 * Always returns 0x00020002. Soft pure-data constant for soname /
 * version-script tables. Self-contained; no parent wires.
 */
uint32_t
gj_glibc_vernode_2_2_5472(void)
{
	(void)NULL;
	return b5472_vernode_2_2();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glibc_vernode_2_2_5472(void)
    __attribute__((alias("gj_glibc_vernode_2_2_5472")));
