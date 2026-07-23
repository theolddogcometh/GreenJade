/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5477: dual MIT/Apache license gate helper.
 *
 * Surface (unique symbols):
 *   uint32_t gj_dual_license_gate_5477(void);
 *     - Return 1 when the dual MIT OR Apache-2.0 product claim is green
 *       for this continuum wave. Soft compile-time license policy gate.
 *   uint32_t __gj_dual_license_gate_5477  (alias)
 *   __libcgj_batch5477_marker = "libcgj-batch5477"
 *
 * Exclusive continuum CREATE-ONLY (5471-5480: libc soname gates /
 * GLIBC_2.x version nodes, dual MIT/Apache license gates, clean-room
 * ABI markers, no-GPL assertion helpers —
 * glibc_vernode_2_0_5471, glibc_vernode_2_2_5472,
 * glibc_vernode_2_17_5473, glibc_vernode_2_27_5474,
 * glibc_vernode_2_34_5475, libc_soname_gate_5476,
 * dual_license_gate_5477, cleanroom_abi_marker_5478,
 * no_gpl_assert_5479, batch_id_5480). Unique
 * gj_dual_license_gate_5477 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5477_marker[] = "libcgj-batch5477";

/* Dual MIT OR Apache-2.0 product claim green. */
#define B5477_DUAL_LICENSE_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5477_dual_license_gate(void)
{
	return B5477_DUAL_LICENSE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_dual_license_gate_5477 - report dual MIT/Apache-2.0 license gate.
 *
 * Always returns 1 (claim holds). Soft pure-data constant for license
 * policy tables. Self-contained; no parent wires.
 */
uint32_t
gj_dual_license_gate_5477(void)
{
	(void)NULL;
	return b5477_dual_license_gate();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_dual_license_gate_5477(void)
    __attribute__((alias("gj_dual_license_gate_5477")));
