/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5102: soft IRQ vector id for the spurious line.
 *
 * Surface (unique symbols):
 *   uint32_t gj_irq_vector_spurious_u(void);
 *     - Returns the soft architecture spurious IRQ vector number (255).
 *       Top of the 0..255 vector space (classic APIC-style tag).
 *   uint32_t __gj_irq_vector_spurious_u  (alias)
 *   __libcgj_batch5102_marker = "libcgj-batch5102"
 *
 * Exclusive continuum CREATE-ONLY (5101-5110). Unique
 * gj_irq_vector_spurious_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5102_marker[] = "libcgj-batch5102";

/* Soft spurious IRQ vector (top of 8-bit vector space). */
#define B5102_SPURIOUS_VEC  255u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5102_spurious_vec(void)
{
	return B5102_SPURIOUS_VEC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_irq_vector_spurious_u - soft spurious IRQ vector number.
 *
 * Always returns 255. Soft tag for the classic top-of-range spurious
 * vector; does not program hardware. No parent wires.
 */
uint32_t
gj_irq_vector_spurious_u(void)
{
	(void)NULL;
	return b5102_spurious_vec();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_irq_vector_spurious_u(void)
    __attribute__((alias("gj_irq_vector_spurious_u")));
