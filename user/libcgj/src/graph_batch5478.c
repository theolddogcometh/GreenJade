/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5478: clean-room ABI marker helper.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cleanroom_abi_marker_5478(void);
 *     - Return 1 when the clean-room freestanding ABI claim is green
 *       for this continuum wave. Soft compile-time ABI readiness flag.
 *   uint32_t __gj_cleanroom_abi_marker_5478  (alias)
 *   __libcgj_batch5478_marker = "libcgj-batch5478"
 *
 * Exclusive continuum CREATE-ONLY (5471-5480: libc soname gates /
 * GLIBC_2.x version nodes, dual MIT/Apache license gates, clean-room
 * ABI markers, no-GPL assertion helpers —
 * glibc_vernode_2_0_5471, glibc_vernode_2_2_5472,
 * glibc_vernode_2_17_5473, glibc_vernode_2_27_5474,
 * glibc_vernode_2_34_5475, libc_soname_gate_5476,
 * dual_license_gate_5477, cleanroom_abi_marker_5478,
 * no_gpl_assert_5479, batch_id_5480). Unique
 * gj_cleanroom_abi_marker_5478 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5478_marker[] = "libcgj-batch5478";

/* Clean-room freestanding ABI claim green. */
#define B5478_CLEANROOM_ABI_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5478_cleanroom_abi(void)
{
	return B5478_CLEANROOM_ABI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cleanroom_abi_marker_5478 - report clean-room ABI readiness marker.
 *
 * Always returns 1 (green). Soft pure-data constant for product policy
 * tables. Self-contained; no parent wires.
 */
uint32_t
gj_cleanroom_abi_marker_5478(void)
{
	(void)NULL;
	return b5478_cleanroom_abi();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cleanroom_abi_marker_5478(void)
    __attribute__((alias("gj_cleanroom_abi_marker_5478")));
