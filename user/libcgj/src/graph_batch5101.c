/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5101: soft IRQ vector id for the timer line.
 *
 * Surface (unique symbols):
 *   uint32_t gj_irq_vector_timer_u(void);
 *     - Returns the soft architecture timer IRQ vector number (32).
 *       First hardware IRQ after the 0..31 exception window.
 *   uint32_t __gj_irq_vector_timer_u  (alias)
 *   __libcgj_batch5101_marker = "libcgj-batch5101"
 *
 * Exclusive continuum CREATE-ONLY (5101-5110: irq_vector_timer_u,
 * irq_vector_spurious_u, irq_is_exception_u, irq_is_irq_u,
 * irq_priority_u, irq_mask_bit_u, irq_mask_test_u, irq_eoi_ok_u,
 * irq_nest_inc_u, batch_id_5110). Unique gj_irq_vector_timer_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5101_marker[] = "libcgj-batch5101";

/* Soft timer IRQ vector (first post-exception hardware line). */
#define B5101_TIMER_VEC  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5101_timer_vec(void)
{
	return B5101_TIMER_VEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_irq_vector_timer_u - soft timer IRQ vector number.
 *
 * Always returns 32 (first hardware IRQ after exceptions 0..31).
 * Compile-time soft architecture tag; no parent wires.
 */
uint32_t
gj_irq_vector_timer_u(void)
{
	(void)NULL;
	return b5101_timer_vec();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_irq_vector_timer_u(void)
    __attribute__((alias("gj_irq_vector_timer_u")));
