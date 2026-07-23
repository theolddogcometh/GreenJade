/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5110: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5110(void);
 *     - Returns the compile-time graph batch number for this TU (5110).
 *   uint32_t __gj_batch_id_5110  (alias)
 *   __libcgj_batch5110_marker = "libcgj-batch5110"
 *
 * Exclusive continuum CREATE-ONLY (5101-5110: irq_vector_timer_u,
 * irq_vector_spurious_u, irq_is_exception_u, irq_is_irq_u,
 * irq_priority_u, irq_mask_bit_u, irq_mask_test_u, irq_eoi_ok_u,
 * irq_nest_inc_u, batch_id_5110). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5110_marker[] = "libcgj-batch5110";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5110_id(void)
{
	return 5110u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5110 - report this TU's graph batch number.
 *
 * Always returns 5110. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5110(void)
{
	(void)NULL;
	return b5110_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5110(void)
    __attribute__((alias("gj_batch_id_5110")));
