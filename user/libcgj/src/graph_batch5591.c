/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5591: milestone 5600 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_5600(void);
 *     - Returns the exclusive wave id for the milestone 5600 continuum
 *       (always 5600). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_5600  (alias)
 *   __libcgj_batch5591_marker = "libcgj-batch5591"
 *
 * Milestone 5600 exclusive continuum CREATE-ONLY (5591-5600). Unique
 * gj_wave_id_5600 surface only; no multi-def. Distinct from
 * gj_wave_id_5550 (batch5541), gj_wave_id_5500 (batch5491),
 * gj_wave_id_5450 (batch5441), and gj_wave_id_5400 (batch5391).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5591_marker[] = "libcgj-batch5591";

/* Exclusive-wave id for milestone 5600 continuum. */
#define B5591_WAVE_ID  5600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5591_wave_id(void)
{
	return B5591_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_5600 - report the exclusive milestone-5600 wave id.
 *
 * Always returns 5600 (milestone 5600 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_5600(void)
{
	(void)NULL;
	return b5591_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_5600(void)
    __attribute__((alias("gj_wave_id_5600")));
