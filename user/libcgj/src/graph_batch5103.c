/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5103: classify a vector as a CPU exception.
 *
 * Surface (unique symbols):
 *   uint32_t gj_irq_is_exception_u(uint32_t v);
 *     - Return 1 when v is in the soft exception window [0, 32), else 0.
 *   uint32_t __gj_irq_is_exception_u  (alias)
 *   __libcgj_batch5103_marker = "libcgj-batch5103"
 *
 * Exclusive continuum CREATE-ONLY (5101-5110). Unique
 * gj_irq_is_exception_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5103_marker[] = "libcgj-batch5103";

/* Soft exception window width (vectors 0 .. 31). */
#define B5103_EXC_LIMIT  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5103_is_exception(uint32_t u32V)
{
	if (u32V < B5103_EXC_LIMIT) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_irq_is_exception_u - 1 if vector v is a soft CPU exception.
 *
 * v: interrupt / trap vector number
 *
 * Returns 1 when v < 32, else 0. Complements gj_irq_is_irq_u
 * (batch5104). Self-contained; no parent wires.
 */
uint32_t
gj_irq_is_exception_u(uint32_t u32V)
{
	(void)NULL;
	return b5103_is_exception(u32V);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_irq_is_exception_u(uint32_t u32V)
    __attribute__((alias("gj_irq_is_exception_u")));
