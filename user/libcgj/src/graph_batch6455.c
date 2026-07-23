/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6455: achievement unlock mask merge stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ach_mask_merge_6455(uint32_t a, uint32_t b);
 *     - Soft union of two unlock masks (a | b). Used to combine
 *       session progress with persisted unlock state.
 *   uint32_t __gj_ach_mask_merge_6455  (alias)
 *   __libcgj_batch6455_marker = "libcgj-batch6455"
 *
 * Exclusive continuum CREATE-ONLY (6451-6460: achievement unlock mask
 * stubs). Unique gj_ach_mask_merge_6455 surface only; no multi-def.
 * Distinct from bit_set/clear/test. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6455_marker[] = "libcgj-batch6455";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6455_merge(uint32_t uA, uint32_t uB)
{
	return uA | uB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ach_mask_merge_6455 - soft OR-merge of two achievement unlock masks.
 *
 * a, b: unlock bitfields
 * Returns a | b. Pure integer. No parent wires.
 */
uint32_t
gj_ach_mask_merge_6455(uint32_t a, uint32_t b)
{
	(void)NULL;
	return b6455_merge(a, b);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ach_mask_merge_6455(uint32_t a, uint32_t b)
    __attribute__((alias("gj_ach_mask_merge_6455")));
