/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5491: milestone 5500 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_5500(void);
 *     - Returns the exclusive wave id for the milestone 5500 continuum
 *       (always 5500). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_5500  (alias)
 *   __libcgj_batch5491_marker = "libcgj-batch5491"
 *
 * Milestone 5500 exclusive continuum CREATE-ONLY (5491-5500). Unique
 * gj_wave_id_5500 surface only; no multi-def. Distinct from
 * gj_wave_id_5450 (batch5441), gj_wave_id_5400 (batch5391),
 * gj_wave_id_5350 (batch5341), and gj_wave_id_5300 (batch5291).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5491_marker[] = "libcgj-batch5491";

/* Exclusive-wave id for milestone 5500 continuum. */
#define B5491_WAVE_ID  5500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5491_wave_id(void)
{
	return B5491_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_5500 - report the exclusive milestone-5500 wave id.
 *
 * Always returns 5500 (milestone 5500 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_5500(void)
{
	(void)NULL;
	return b5491_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_5500(void)
    __attribute__((alias("gj_wave_id_5500")));
