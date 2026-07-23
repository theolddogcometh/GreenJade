/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5579: default active A/B install slot stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ab_slot_active_default_u(void);
 *     - Returns 0 (default active A/B slot = slot A). Soft compile-time
 *       product tag for dual-slot GreenJade OS image install continuum.
 *   uint32_t __gj_ab_slot_active_default_u  (alias)
 *   __libcgj_batch5579_marker = "libcgj-batch5579"
 *
 * Exclusive continuum CREATE-ONLY (5571-5580: update channels / OS image
 * version / A/B install slot stubs unique). Unique
 * gj_ab_slot_active_default_u surface only; no multi-def. Distinct from
 * gj_ab_slot_a_u (batch5577) and gj_ab_slot_b_u (batch5578). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5579_marker[] = "libcgj-batch5579";

/* Default active A/B slot (slot A). */
#define B5579_ACTIVE_DEFAULT  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5579_active_default(void)
{
	return B5579_ACTIVE_DEFAULT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ab_slot_active_default_u - report default active A/B install slot.
 *
 * Always returns 0 (slot A is the default active slot). Soft pure-data
 * stub only; does not read bootloader state. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_ab_slot_active_default_u(void)
{
	(void)NULL;
	return b5579_active_default();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ab_slot_active_default_u(void)
    __attribute__((alias("gj_ab_slot_active_default_u")));
