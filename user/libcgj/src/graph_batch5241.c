/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5241: milestone 5250 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_5250(void);
 *     - Returns the exclusive wave id for the milestone 5250 continuum
 *       (always 5250). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_5250  (alias)
 *   __libcgj_batch5241_marker = "libcgj-batch5241"
 *
 * Milestone 5250 exclusive continuum CREATE-ONLY (5241-5250). Unique
 * gj_wave_id_5250 surface only; no multi-def. Distinct from
 * gj_wave_id_5200 (batch5191), gj_wave_id_5100 (batch5091),
 * gj_wave_id_5000 (batch4991), and gj_wave_id_4900 (batch4891).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5241_marker[] = "libcgj-batch5241";

/* Exclusive-wave id for milestone 5250 continuum. */
#define B5241_WAVE_ID  5250u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5241_wave_id(void)
{
	return B5241_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_5250 - report the exclusive milestone-5250 wave id.
 *
 * Always returns 5250 (milestone 5250 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_5250(void)
{
	(void)NULL;
	return b5241_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_5250(void)
    __attribute__((alias("gj_wave_id_5250")));
