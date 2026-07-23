/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5834: GLIBC_2.15 deep version-node probe.
 *
 * Surface (unique symbols):
 *   uint32_t gj_glibc_vernode_deep_2_15_5834(void);
 *     - Return packed GLIBC_2.15 deep version-node tag (major=2,
 *       minor=15 as 0x0002000f). Soft compile-time deep version-script
 *       probe.
 *   uint32_t __gj_glibc_vernode_deep_2_15_5834  (alias)
 *   __libcgj_batch5834_marker = "libcgj-batch5834"
 *
 * Exclusive continuum CREATE-ONLY (5831-5840: libc soname deep gates
 * and GLIBC version node probes —
 * libc_soname_deep_gate_5831, glibc_vernode_deep_2_0_5832,
 * glibc_vernode_deep_2_4_5833, glibc_vernode_deep_2_15_5834,
 * glibc_vernode_deep_2_28_5835, glibc_vernode_deep_2_31_5836,
 * glibc_vernode_deep_2_36_5837, glibc_vernode_deep_2_39_5838,
 * soname_deep_probe_ok_5839, batch_id_5840). Unique
 * gj_glibc_vernode_deep_2_15_5834 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5834_marker[] = "libcgj-batch5834";

/* Packed GLIBC_2.15 deep version node: (major << 16) | minor. */
#define B5834_VERNODE_DEEP_2_15  ((uint32_t)0x0002000fu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5834_vernode_deep_2_15(void)
{
	return B5834_VERNODE_DEEP_2_15;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_glibc_vernode_deep_2_15_5834 - report GLIBC_2.15 deep vernode probe.
 *
 * Always returns 0x0002000f. Soft pure-data constant for deep
 * version-script / soname tables. Self-contained; no parent wires.
 */
uint32_t
gj_glibc_vernode_deep_2_15_5834(void)
{
	(void)NULL;
	return b5834_vernode_deep_2_15();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_glibc_vernode_deep_2_15_5834(void)
    __attribute__((alias("gj_glibc_vernode_deep_2_15_5834")));
