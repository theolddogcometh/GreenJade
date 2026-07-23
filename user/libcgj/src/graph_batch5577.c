/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5577: GreenJade A/B install slot A stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ab_slot_a_u(void);
 *     - Returns 0 (A/B install slot A id). Soft compile-time product
 *       tag for dual-slot GreenJade OS image install continuum.
 *   uint32_t __gj_ab_slot_a_u  (alias)
 *   __libcgj_batch5577_marker = "libcgj-batch5577"
 *
 * Exclusive continuum CREATE-ONLY (5571-5580: update channels / OS image
 * version / A/B install slot stubs unique). Unique gj_ab_slot_a_u
 * surface only; no multi-def. Distinct from gj_ab_slot_b_u (batch5578)
 * and gj_ab_slot_active_default_u (batch5579). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5577_marker[] = "libcgj-batch5577";

/* A/B install slot A id. */
#define B5577_SLOT_A  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5577_slot_a(void)
{
	return B5577_SLOT_A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ab_slot_a_u - report A/B install slot A id.
 *
 * Always returns 0 (slot A). Soft pure-data stub only; does not probe
 * bootloader or partition tables. Does not call libc. No parent wires.
 */
uint32_t
gj_ab_slot_a_u(void)
{
	(void)NULL;
	return b5577_slot_a();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ab_slot_a_u(void)
    __attribute__((alias("gj_ab_slot_a_u")));
