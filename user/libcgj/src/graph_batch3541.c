/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3541: milestone 3550 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_3550(void);
 *     - Returns the exclusive wave id for the milestone 3550 continuum
 *       (always 3550). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_3550  (alias)
 *   __libcgj_batch3541_marker = "libcgj-batch3541"
 *
 * Milestone 3550 exclusive continuum CREATE-ONLY (3541-3550). Unique
 * gj_wave_id_3550 surface only; no multi-def. Distinct from
 * gj_wave_id_3400 (batch3391), gj_wave_id_3300 (batch3291),
 * gj_wave_id_3200 (batch3192), and gj_wave_id_3100 (batch3093).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3541_marker[] = "libcgj-batch3541";

/* Exclusive-wave id for milestone 3550 continuum. */
#define B3541_WAVE_ID  3550u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3541_wave_id(void)
{
	return B3541_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_3550 - report the exclusive milestone-3550 wave id.
 *
 * Always returns 3550 (milestone 3550 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_3550(void)
{
	(void)NULL;
	return b3541_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_3550(void)
    __attribute__((alias("gj_wave_id_3550")));
