/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5028: soft CPU affinity mask single-bit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sched_cpu_mask_bit_u(uint32_t cpu);
 *     - Return (1u << cpu) for cpu in 0..31; return 0 when cpu >= 32
 *       (out of the soft 32-bit mask width).
 *   uint32_t __gj_sched_cpu_mask_bit_u  (alias)
 *   __libcgj_batch5028_marker = "libcgj-batch5028"
 *
 * Exclusive continuum CREATE-ONLY (5021-5030). Unique
 * gj_sched_cpu_mask_bit_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5028_marker[] = "libcgj-batch5028";

/* Soft mask width in bits (uint32_t affinity word). */
#define B5028_CPU_BITS  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5028_cpu_bit(uint32_t u32Cpu)
{
	if (u32Cpu >= B5028_CPU_BITS) {
		return 0u;
	}
	return 1u << u32Cpu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sched_cpu_mask_bit_u - single-bit soft CPU affinity mask.
 *
 * cpu: logical CPU index
 *
 * Returns 1u << cpu for cpu 0..31, else 0. Useful for constructing or
 * probing a 32-bit soft affinity word without UB on shift width.
 * No parent wires.
 */
uint32_t
gj_sched_cpu_mask_bit_u(uint32_t u32Cpu)
{
	(void)NULL;
	return b5028_cpu_bit(u32Cpu);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sched_cpu_mask_bit_u(uint32_t u32Cpu)
    __attribute__((alias("gj_sched_cpu_mask_bit_u")));
