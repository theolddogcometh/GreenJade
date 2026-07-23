/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5029: soft CPU affinity mask bit test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sched_cpu_mask_test_u(uint32_t mask, uint32_t cpu);
 *     - Return 1 when bit cpu is set in mask (cpu in 0..31), else 0.
 *       cpu >= 32 always yields 0.
 *   uint32_t __gj_sched_cpu_mask_test_u  (alias)
 *   __libcgj_batch5029_marker = "libcgj-batch5029"
 *
 * Exclusive continuum CREATE-ONLY (5021-5030). Unique
 * gj_sched_cpu_mask_test_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5029_marker[] = "libcgj-batch5029";

/* Soft mask width in bits (uint32_t affinity word). */
#define B5029_CPU_BITS  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5029_cpu_test(uint32_t u32Mask, uint32_t u32Cpu)
{
	uint32_t u32Bit;

	if (u32Cpu >= B5029_CPU_BITS) {
		return 0u;
	}
	u32Bit = 1u << u32Cpu;
	return ((u32Mask & u32Bit) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sched_cpu_mask_test_u - test one bit of a soft CPU affinity mask.
 *
 * mask: soft 32-bit affinity word
 * cpu:  logical CPU index
 *
 * Returns 1 if bit cpu is set and cpu < 32, else 0. Self-contained;
 * does not call gj_sched_cpu_mask_bit_u (no parent wires).
 */
uint32_t
gj_sched_cpu_mask_test_u(uint32_t u32Mask, uint32_t u32Cpu)
{
	(void)NULL;
	return b5029_cpu_test(u32Mask, u32Cpu);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sched_cpu_mask_test_u(uint32_t u32Mask, uint32_t u32Cpu)
    __attribute__((alias("gj_sched_cpu_mask_test_u")));
