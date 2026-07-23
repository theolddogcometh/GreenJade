/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5791: milestone 5800 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_5800(void);
 *     - Returns the exclusive wave id for the milestone 5800 continuum
 *       (always 5800). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_5800  (alias)
 *   __libcgj_batch5791_marker = "libcgj-batch5791"
 *
 * Milestone 5800 exclusive continuum CREATE-ONLY (5791-5800). Unique
 * gj_wave_id_5800 surface only; no multi-def. Distinct from
 * gj_wave_id_5700 (batch5691), gj_wave_id_5650 (batch5641),
 * gj_wave_id_5600 (batch5591), and gj_wave_id_5550 (batch5541).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5791_marker[] = "libcgj-batch5791";

/* Exclusive-wave id for milestone 5800 continuum. */
#define B5791_WAVE_ID  5800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5791_wave_id(void)
{
	return B5791_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_5800 - report the exclusive milestone-5800 wave id.
 *
 * Always returns 5800 (milestone 5800 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_5800(void)
{
	(void)NULL;
	return b5791_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_5800(void)
    __attribute__((alias("gj_wave_id_5800")));
