/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5831: libc SONAME major deep gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_libc_soname_deep_gate_5831(void);
 *     - Return the product libc SONAME major deep gate (always 6, as in
 *       libc.so.6). Soft compile-time deep soname product constant.
 *   uint32_t __gj_libc_soname_deep_gate_5831  (alias)
 *   __libcgj_batch5831_marker = "libcgj-batch5831"
 *
 * Exclusive continuum CREATE-ONLY (5831-5840: libc soname deep gates
 * and GLIBC version node probes —
 * libc_soname_deep_gate_5831, glibc_vernode_deep_2_0_5832,
 * glibc_vernode_deep_2_4_5833, glibc_vernode_deep_2_15_5834,
 * glibc_vernode_deep_2_28_5835, glibc_vernode_deep_2_31_5836,
 * glibc_vernode_deep_2_36_5837, glibc_vernode_deep_2_39_5838,
 * soname_deep_probe_ok_5839, batch_id_5840). Unique
 * gj_libc_soname_deep_gate_5831 surface only; no multi-def. Distinct
 * from gj_libc_soname_gate_5476 and gj_libc_soname_major_u (batch5381).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5831_marker[] = "libcgj-batch5831";

/* libc.so.N SONAME major deep gate (N = 6). */
#define B5831_SONAME_DEEP_GATE  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5831_soname_deep_gate(void)
{
	return B5831_SONAME_DEEP_GATE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libc_soname_deep_gate_5831 - report libc SONAME major deep gate.
 *
 * Always returns 6 (libc.so.6 product identity). Soft pure-data deep
 * gate for soname / packaging tables. Self-contained; no parent wires.
 */
uint32_t
gj_libc_soname_deep_gate_5831(void)
{
	(void)NULL;
	return b5831_soname_deep_gate();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libc_soname_deep_gate_5831(void)
    __attribute__((alias("gj_libc_soname_deep_gate_5831")));
