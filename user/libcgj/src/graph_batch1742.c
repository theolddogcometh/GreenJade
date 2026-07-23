/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1742: cpu.shares clamp (cgroup v1).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cpu_shares_clamp(uint32_t shares);
 *     — Clamp cgroup v1 cpu.shares into [2, 262144]. Below 2 → 2;
 *       above 262144 → 262144. Default weight in the kernel is 1024.
 *   uint32_t __gj_cpu_shares_clamp  (alias)
 *   __libcgj_batch1742_marker = "libcgj-batch1742"
 *
 * Distinct from gj_cgroup_weight_clamp (batch1741, 1..10000). Does NOT
 * redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1742_marker[] = "libcgj-batch1742";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1742_clamp(uint32_t u32S)
{
	if (u32S < 2u) {
		return 2u;
	}
	if (u32S > 262144u) {
		return 262144u;
	}
	return u32S;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cpu_shares_clamp — clamp shares into the 2..262144 cpu.shares range.
 *
 * Pure sanitizer for v1 cpu controller share values. No scheduler
 * objects, no cgroupfs.
 */
uint32_t
gj_cpu_shares_clamp(uint32_t u32Shares)
{
	return b1742_clamp(u32Shares);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cpu_shares_clamp(uint32_t u32Shares)
    __attribute__((alias("gj_cpu_shares_clamp")));
