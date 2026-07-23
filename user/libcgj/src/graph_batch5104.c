/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5104: classify a vector as a hardware IRQ.
 *
 * Surface (unique symbols):
 *   uint32_t gj_irq_is_irq_u(uint32_t v);
 *     - Return 1 when v is in the soft IRQ window [32, 256), else 0.
 *   uint32_t __gj_irq_is_irq_u  (alias)
 *   __libcgj_batch5104_marker = "libcgj-batch5104"
 *
 * Exclusive continuum CREATE-ONLY (5101-5110). Unique
 * gj_irq_is_irq_u surface only; no multi-def. Complements
 * gj_irq_is_exception_u (batch5103). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5104_marker[] = "libcgj-batch5104";

/* Soft hardware IRQ window: first IRQ after exceptions. */
#define B5104_IRQ_BASE   32u
/* Soft vector space upper exclusive bound (0..255). */
#define B5104_VEC_LIMIT  256u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5104_is_irq(uint32_t u32V)
{
	if (u32V >= B5104_IRQ_BASE && u32V < B5104_VEC_LIMIT) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_irq_is_irq_u - 1 if vector v is a soft hardware IRQ.
 *
 * v: interrupt vector number
 *
 * Returns 1 when 32 <= v < 256, else 0 (exceptions and out-of-range
 * vectors fail). Self-contained; does not call sibling helpers.
 * No parent wires.
 */
uint32_t
gj_irq_is_irq_u(uint32_t u32V)
{
	(void)NULL;
	return b5104_is_irq(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_irq_is_irq_u(uint32_t u32V)
    __attribute__((alias("gj_irq_is_irq_u")));
