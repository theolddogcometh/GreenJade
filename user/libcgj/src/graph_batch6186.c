/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6186: cpuset bitmask popcount stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cg_cpuset_mask_pop_6186(uint64_t mask);
 *     - Return the number of set bits in a 64-bit cpuset/cpumask word
 *       (soft container runtime affinity word). mask == 0 → 0.
 *   uint32_t __gj_cg_cpuset_mask_pop_6186  (alias)
 *   __libcgj_batch6186_marker = "libcgj-batch6186"
 *
 * Exclusive continuum CREATE-ONLY (6181-6190: container runtime cgroup
 * stubs). Unique gj_cg_cpuset_mask_pop_6186 surface only; no multi-def.
 * Pure data — not a cpuset.cpus writer. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6186_marker[] = "libcgj-batch6186";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6186_popcount(uint64_t uMask)
{
	uint32_t uN;

	uN = 0u;
	while (uMask != 0ull) {
		uMask &= uMask - 1ull;
		uN++;
	}
	return uN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cg_cpuset_mask_pop_6186 - popcount of one 64-bit cpuset word.
 *
 * mask: soft cpuset/cpumask affinity word for container placement.
 * Returns set-bit count in [0, 64]. Pure integer; no kernel cpuset
 * access. Does not call libc. No parent wires.
 */
uint32_t
gj_cg_cpuset_mask_pop_6186(uint64_t uMask)
{
	(void)NULL;
	return b6186_popcount(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cg_cpuset_mask_pop_6186(uint64_t uMask)
    __attribute__((alias("gj_cg_cpuset_mask_pop_6186")));
