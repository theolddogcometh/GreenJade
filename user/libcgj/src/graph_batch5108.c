/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5108: soft check whether EOI is appropriate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_irq_eoi_ok_u(uint32_t v);
 *     - Return 1 when v is a real hardware IRQ suitable for soft EOI
 *       (32 <= v < 255). Exceptions (v < 32), the spurious vector
 *       (255), and out-of-range vectors return 0.
 *   uint32_t __gj_irq_eoi_ok_u  (alias)
 *   __libcgj_batch5108_marker = "libcgj-batch5108"
 *
 * Exclusive continuum CREATE-ONLY (5101-5110). Unique gj_irq_eoi_ok_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5108_marker[] = "libcgj-batch5108";

/* First soft hardware IRQ vector (after exception window). */
#define B5108_IRQ_BASE      32u
/* Spurious vector: EOI not indicated in this soft model. */
#define B5108_SPURIOUS_VEC  255u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5108_eoi_ok(uint32_t u32V)
{
	if (u32V >= B5108_IRQ_BASE && u32V < B5108_SPURIOUS_VEC) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_irq_eoi_ok_u - 1 if soft EOI is appropriate for vector v.
 *
 * v: interrupt vector number
 *
 * Returns 1 for real IRQs in [32, 255), else 0 (exceptions, spurious
 * 255, and out-of-range). Soft policy only; no hardware side effects.
 * No parent wires.
 */
uint32_t
gj_irq_eoi_ok_u(uint32_t u32V)
{
	(void)NULL;
	return b5108_eoi_ok(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_irq_eoi_ok_u(uint32_t u32V)
    __attribute__((alias("gj_irq_eoi_ok_u")));
