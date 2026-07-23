/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5391: milestone 5400 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_5400(void);
 *     - Returns the exclusive wave id for the milestone 5400 continuum
 *       (always 5400). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_5400  (alias)
 *   __libcgj_batch5391_marker = "libcgj-batch5391"
 *
 * Milestone 5400 exclusive continuum CREATE-ONLY (5391-5400). Unique
 * gj_wave_id_5400 surface only; no multi-def. Distinct from
 * gj_wave_id_5350 (batch5341), gj_wave_id_5300 (batch5291),
 * gj_wave_id_5200 (batch5191), and gj_wave_id_5100 (batch5091).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5391_marker[] = "libcgj-batch5391";

/* Exclusive-wave id for milestone 5400 continuum. */
#define B5391_WAVE_ID  5400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5391_wave_id(void)
{
	return B5391_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_5400 - report the exclusive milestone-5400 wave id.
 *
 * Always returns 5400 (milestone 5400 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_5400(void)
{
	(void)NULL;
	return b5391_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_5400(void)
    __attribute__((alias("gj_wave_id_5400")));
