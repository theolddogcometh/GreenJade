/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3591: milestone 3600 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_3600(void);
 *     - Returns the exclusive wave id for the milestone 3600 continuum
 *       (always 3600). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_3600  (alias)
 *   __libcgj_batch3591_marker = "libcgj-batch3591"
 *
 * Milestone 3600 exclusive continuum CREATE-ONLY (3591-3600). Unique
 * gj_wave_id_3600 surface only; no multi-def. Distinct from
 * gj_wave_id_3550 (batch3541), gj_wave_id_3500 (batch3491),
 * gj_wave_id_3400 (batch3391), and gj_wave_id_3300 (batch3291).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3591_marker[] = "libcgj-batch3591";

/* Exclusive-wave id for milestone 3600 continuum. */
#define B3591_WAVE_ID  3600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3591_wave_id(void)
{
	return B3591_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_3600 - report the exclusive milestone-3600 wave id.
 *
 * Always returns 3600 (milestone 3600 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_3600(void)
{
	(void)NULL;
	return b3591_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_3600(void)
    __attribute__((alias("gj_wave_id_3600")));
