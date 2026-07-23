/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2279: Steam Input controller slot soft-ok
 * predicate (unsigned).
 *
 * Surface (unique symbols):
 *   int gj_controller_slot_ok_u(uint32_t slot);
 *     - Return 1 if slot is a soft-valid Steam Input / controller
 *       player slot in [0, 15] (sixteen local slots with headroom past
 *       the typical 0-3 Deck/local pad set), else 0.
 *   int __gj_controller_slot_ok_u  (alias)
 *   __libcgj_batch2279_marker = "libcgj-batch2279"
 *
 * Post-2270 product exclusive wave (2271-2280). Distinct from
 * gj_controller_type_ok_hint (batch1867) - unique gj_controller_slot_ok_u
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2279_marker[] = "libcgj-batch2279";

/* Inclusive soft max controller / player slot index. */
#define B2279_SLOT_MAX  15u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if slot is in [0, B2279_SLOT_MAX]. */
static int
b2279_slot_ok(uint32_t uSlot)
{
	if (uSlot > B2279_SLOT_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_controller_slot_ok_u - soft check that slot is a valid pad slot.
 *
 * slot: controller / Steam Input player slot index
 *
 * Returns 1 when slot <= 15, else 0. Does not call libc.
 */
int
gj_controller_slot_ok_u(uint32_t uSlot)
{
	(void)NULL;
	return b2279_slot_ok(uSlot);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_controller_slot_ok_u(uint32_t uSlot)
    __attribute__((alias("gj_controller_slot_ok_u")));
