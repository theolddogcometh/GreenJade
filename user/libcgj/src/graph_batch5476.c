/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5476: libc soname major gate helper.
 *
 * Surface (unique symbols):
 *   uint32_t gj_libc_soname_gate_5476(void);
 *     - Return the product libc SONAME major gate (always 6, as in
 *       libc.so.6). Soft compile-time soname product constant.
 *   uint32_t __gj_libc_soname_gate_5476  (alias)
 *   __libcgj_batch5476_marker = "libcgj-batch5476"
 *
 * Exclusive continuum CREATE-ONLY (5471-5480: libc soname gates /
 * GLIBC_2.x version nodes, dual MIT/Apache license gates, clean-room
 * ABI markers, no-GPL assertion helpers —
 * glibc_vernode_2_0_5471, glibc_vernode_2_2_5472,
 * glibc_vernode_2_17_5473, glibc_vernode_2_27_5474,
 * glibc_vernode_2_34_5475, libc_soname_gate_5476,
 * dual_license_gate_5477, cleanroom_abi_marker_5478,
 * no_gpl_assert_5479, batch_id_5480). Unique
 * gj_libc_soname_gate_5476 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5476_marker[] = "libcgj-batch5476";

/* libc.so.N SONAME major product gate (N = 6). */
#define B5476_SONAME_MAJOR  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5476_soname_gate(void)
{
	return B5476_SONAME_MAJOR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libc_soname_gate_5476 - report libc SONAME major product gate.
 *
 * Always returns 6 (libc.so.6 product identity). Soft pure-data
 * constant for soname / packaging tables. Self-contained; no parent
 * wires.
 */
uint32_t
gj_libc_soname_gate_5476(void)
{
	(void)NULL;
	return b5476_soname_gate();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libc_soname_gate_5476(void)
    __attribute__((alias("gj_libc_soname_gate_5476")));
