/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6091: milestone 6100 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_6100(void);
 *     - Returns the exclusive wave id for the milestone 6100 continuum
 *       (always 6100). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_6100  (alias)
 *   __libcgj_batch6091_marker = "libcgj-batch6091"
 *
 * Milestone 6100 exclusive continuum CREATE-ONLY (6091-6100). Unique
 * gj_wave_id_6100 surface only; no multi-def. Distinct from
 * gj_wave_id_6000 (batch5991), gj_wave_id_5900 (batch5891),
 * gj_wave_id_5800 (batch5791), and gj_wave_id_5700 (batch5691).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6091_marker[] = "libcgj-batch6091";

/* Exclusive-wave id for milestone 6100 continuum. */
#define B6091_WAVE_ID  6100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6091_wave_id(void)
{
	return B6091_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_6100 - report the exclusive milestone-6100 wave id.
 *
 * Always returns 6100 (milestone 6100 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_6100(void)
{
	(void)NULL;
	return b6091_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_6100(void)
    __attribute__((alias("gj_wave_id_6100")));
