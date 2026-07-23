/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5106: soft IRQ enable/disable single-bit mask.
 *
 * Surface (unique symbols):
 *   uint32_t gj_irq_mask_bit_u(uint32_t irq);
 *     - Return (1u << irq) for irq in 0..31; return 0 when irq >= 32
 *       (out of the soft 32-bit mask width).
 *   uint32_t __gj_irq_mask_bit_u  (alias)
 *   __libcgj_batch5106_marker = "libcgj-batch5106"
 *
 * Exclusive continuum CREATE-ONLY (5101-5110). Distinct from
 * gj_sched_cpu_mask_bit_u (batch5028) — unique gj_irq_mask_bit_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5106_marker[] = "libcgj-batch5106";

/* Soft IRQ mask width in bits (uint32_t word). */
#define B5106_IRQ_BITS  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5106_mask_bit(uint32_t u32Irq)
{
	if (u32Irq >= B5106_IRQ_BITS) {
		return 0u;
	}
	return 1u << u32Irq;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_irq_mask_bit_u - single-bit soft IRQ enable/disable mask.
 *
 * irq: soft IRQ line index (0..31)
 *
 * Returns 1u << irq for irq 0..31, else 0. Safe against shift-width UB.
 * No parent wires.
 */
uint32_t
gj_irq_mask_bit_u(uint32_t u32Irq)
{
	(void)NULL;
	return b5106_mask_bit(u32Irq);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_irq_mask_bit_u(uint32_t u32Irq)
    __attribute__((alias("gj_irq_mask_bit_u")));
