/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch549: simple hazard-pointer slot helpers
 * (plain store / load, no fences).
 *
 * Surface (unique symbols):
 *   void gj_hazard_set(void **slot, void *p);
 *     — Store p into *slot. NULL slot is a no-op.
 *   void *gj_hazard_get(void *const *slot);
 *     — Return *slot. NULL slot returns NULL.
 *   __gj_hazard_set / __gj_hazard_get  (aliases)
 *   __libcgj_batch549_marker = "libcgj-batch549"
 *
 * Plain pointer store/load only (no volatile, no compiler barrier, no
 * hardware fence). For full publish/clear/scan with barriers see
 * graph_batch329 (gj_hp_*). This TU is the minimal freestanding slot
 * accessor surface for harnesses and single-thread ownership transfer.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch549_marker[] = "libcgj-batch549";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hazard_set — write a hazard pointer into the caller's slot.
 *
 * slot: address of the hazard-pointer cell (NULL → no-op)
 * p:    pointer value to store (may be NULL to clear)
 */
void
gj_hazard_set(void **slot, void *p)
{
	if (slot == NULL) {
		return;
	}
	*slot = p;
}

/*
 * gj_hazard_get — read the hazard pointer from the caller's slot.
 *
 * slot: address of the hazard-pointer cell (NULL → NULL)
 * Returns the pointer currently stored in *slot.
 */
void *
gj_hazard_get(void *const *slot)
{
	if (slot == NULL) {
		return NULL;
	}
	return *slot;
}

/* ---- underscored aliases ----------------------------------------------- */

void __gj_hazard_set(void **slot, void *p)
    __attribute__((alias("gj_hazard_set")));
void *__gj_hazard_get(void *const *slot)
    __attribute__((alias("gj_hazard_get")));
