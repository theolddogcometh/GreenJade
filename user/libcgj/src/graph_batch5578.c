/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5578: GreenJade A/B install slot B stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ab_slot_b_u(void);
 *     - Returns 1 (A/B install slot B id). Soft compile-time product
 *       tag for dual-slot GreenJade OS image install continuum.
 *   uint32_t __gj_ab_slot_b_u  (alias)
 *   __libcgj_batch5578_marker = "libcgj-batch5578"
 *
 * Exclusive continuum CREATE-ONLY (5571-5580: update channels / OS image
 * version / A/B install slot stubs unique). Unique gj_ab_slot_b_u
 * surface only; no multi-def. Distinct from gj_ab_slot_a_u (batch5577)
 * and gj_ab_slot_active_default_u (batch5579). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5578_marker[] = "libcgj-batch5578";

/* A/B install slot B id. */
#define B5578_SLOT_B  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5578_slot_b(void)
{
	return B5578_SLOT_B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ab_slot_b_u - report A/B install slot B id.
 *
 * Always returns 1 (slot B). Soft pure-data stub only; does not probe
 * bootloader or partition tables. Does not call libc. No parent wires.
 */
uint32_t
gj_ab_slot_b_u(void)
{
	(void)NULL;
	return b5578_slot_b();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ab_slot_b_u(void)
    __attribute__((alias("gj_ab_slot_b_u")));
