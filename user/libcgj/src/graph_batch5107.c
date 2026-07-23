/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5107: soft IRQ mask bit test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_irq_mask_test_u(uint32_t mask, uint32_t irq);
 *     - Return 1 when bit irq is set in mask (irq in 0..31), else 0.
 *       irq >= 32 always yields 0.
 *   uint32_t __gj_irq_mask_test_u  (alias)
 *   __libcgj_batch5107_marker = "libcgj-batch5107"
 *
 * Exclusive continuum CREATE-ONLY (5101-5110). Distinct from
 * gj_sched_cpu_mask_test_u (batch5029) — unique gj_irq_mask_test_u
 * surface only; no multi-def. Does not call gj_irq_mask_bit_u.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5107_marker[] = "libcgj-batch5107";

/* Soft IRQ mask width in bits (uint32_t word). */
#define B5107_IRQ_BITS  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5107_mask_test(uint32_t u32Mask, uint32_t u32Irq)
{
	uint32_t u32Bit;

	if (u32Irq >= B5107_IRQ_BITS) {
		return 0u;
	}
	u32Bit = 1u << u32Irq;
	return ((u32Mask & u32Bit) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_irq_mask_test_u - test one bit of a soft IRQ enable mask.
 *
 * mask: soft 32-bit IRQ mask word
 * irq:  soft IRQ line index
 *
 * Returns 1 if bit irq is set and irq < 32, else 0. Self-contained;
 * does not call gj_irq_mask_bit_u (no parent wires).
 */
uint32_t
gj_irq_mask_test_u(uint32_t u32Mask, uint32_t u32Irq)
{
	(void)NULL;
	return b5107_mask_test(u32Mask, u32Irq);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_irq_mask_test_u(uint32_t u32Mask, uint32_t u32Irq)
    __attribute__((alias("gj_irq_mask_test_u")));
