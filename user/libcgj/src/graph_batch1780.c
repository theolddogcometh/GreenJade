/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1780: milestone 1780 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_1780(void);
 *     — Returns the compile-time graph batch number for this TU (1780).
 *   uint32_t __gj_batch_id_1780  (alias)
 *   __libcgj_batch1780_marker = "libcgj-batch1780"
 *
 * Milestone for the page/table exclusive wave (batches 1771–1779).
 * Does NOT redefine gj_batch_id / prior gj_batch_id_N symbols —
 * avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1780_marker[] = "libcgj-batch1780";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Identity helper. pUnused is accepted so NULL can be passed from the
 * public entry (freestanding NULL discipline for this wave).
 */
static uint32_t
b1780_id(const void *pUnused)
{
	if (pUnused != NULL) {
		/* Unused non-NULL path still yields the batch id. */
		return 1780u;
	}
	return 1780u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_1780 — report this TU's graph batch number.
 *
 * Always returns 1780.
 */
uint32_t
gj_batch_id_1780(void)
{
	return b1780_id(NULL);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_1780(void)
    __attribute__((alias("gj_batch_id_1780")));
