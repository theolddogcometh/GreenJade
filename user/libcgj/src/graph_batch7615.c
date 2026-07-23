/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7615: getrlimit soft<=hard ordering stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rlim_soft_le_hard_u64_7615(uint64_t soft, uint64_t hard);
 *     - Return 1 if soft limit is allowed vs hard (soft<=hard, or hard
 *       is infinity), else 0. Soft infinity requires hard infinity.
 *   uint32_t __gj_rlim_soft_le_hard_u64_7615  (alias)
 *   __libcgj_batch7615_marker = "libcgj-batch7615"
 *
 * Exclusive continuum CREATE-ONLY (7611-7620: getrlimit resource stubs —
 * infinity, nlimits, resource_ok, is_inf, soft_le_hard, cpu_id,
 * nofile_id, as_id, pair_ok, continuum + batch_id_7620). Unique
 * gj_rlim_soft_le_hard_u64_7615 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7615_marker[] = "libcgj-batch7615";

/* Soft RLIM_INFINITY: (rlim_t)-1 on 64-bit Linux. */
#define B7615_RLIM_INFINITY  ((uint64_t)0xffffffffffffffffull)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7615_rlim_soft_le_hard(uint64_t u64Soft, uint64_t u64Hard)
{
	/* Hard infinity accepts any soft, including soft infinity. */
	if (u64Hard == B7615_RLIM_INFINITY)
		return 1u;
	/* Soft infinity with finite hard is invalid. */
	if (u64Soft == B7615_RLIM_INFINITY)
		return 0u;
	return (u64Soft <= u64Hard) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rlim_soft_le_hard_u64_7615 - 1 if soft/hard pair satisfies order.
 *
 * soft: soft rlim_cur
 * hard: soft rlim_max
 *
 * Returns 1 when soft is allowed under hard (POSIX setrlimit order:
 * soft may not exceed hard; infinity hard is unbounded). Self-contained;
 * no parent wires. Does not call setrlimit.
 */
uint32_t
gj_rlim_soft_le_hard_u64_7615(uint64_t u64Soft, uint64_t u64Hard)
{
	(void)NULL;
	return b7615_rlim_soft_le_hard(u64Soft, u64Hard);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rlim_soft_le_hard_u64_7615(uint64_t u64Soft, uint64_t u64Hard)
    __attribute__((alias("gj_rlim_soft_le_hard_u64_7615")));
