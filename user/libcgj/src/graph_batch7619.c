/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7619: getrlimit resource+pair validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rlim_pair_ok_u64_7619(uint32_t resource, uint64_t soft,
 *                                     uint64_t hard);
 *     - Return 1 if resource is in range and soft/hard order is valid.
 *   uint32_t __gj_rlim_pair_ok_u64_7619  (alias)
 *   __libcgj_batch7619_marker = "libcgj-batch7619"
 *
 * Exclusive continuum CREATE-ONLY (7611-7620: getrlimit resource stubs —
 * infinity, nlimits, resource_ok, is_inf, soft_le_hard, cpu_id,
 * nofile_id, as_id, pair_ok, continuum + batch_id_7620). Unique
 * gj_rlim_pair_ok_u64_7619 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7619_marker[] = "libcgj-batch7619";

/* Soft RLIMIT_NLIMITS on Linux. */
#define B7619_RLIMIT_NLIMITS  ((uint32_t)16u)

/* Soft RLIM_INFINITY: (rlim_t)-1 on 64-bit Linux. */
#define B7619_RLIM_INFINITY   ((uint64_t)0xffffffffffffffffull)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7619_rlim_pair_ok(uint32_t u32Resource, uint64_t u64Soft, uint64_t u64Hard)
{
	if (u32Resource >= B7619_RLIMIT_NLIMITS)
		return 0u;
	if (u64Hard == B7619_RLIM_INFINITY)
		return 1u;
	if (u64Soft == B7619_RLIM_INFINITY)
		return 0u;
	return (u64Soft <= u64Hard) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rlim_pair_ok_u64_7619 - 1 if resource and soft/hard pair are valid.
 *
 * resource: soft getrlimit/setrlimit resource id
 * soft:     soft rlim_cur
 * hard:     soft rlim_max
 *
 * Returns 1 when resource is in [0, 16) and soft does not exceed hard
 * (hard infinity allows any soft). Self-contained; does not call sibling
 * helpers. No parent wires. Does not call setrlimit.
 */
uint32_t
gj_rlim_pair_ok_u64_7619(uint32_t u32Resource, uint64_t u64Soft,
    uint64_t u64Hard)
{
	(void)NULL;
	return b7619_rlim_pair_ok(u32Resource, u64Soft, u64Hard);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rlim_pair_ok_u64_7619(uint32_t u32Resource, uint64_t u64Soft,
    uint64_t u64Hard)
    __attribute__((alias("gj_rlim_pair_ok_u64_7619")));
