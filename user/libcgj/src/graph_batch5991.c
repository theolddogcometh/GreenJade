/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5991: milestone 6000 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_6000(void);
 *     - Returns the exclusive wave id for the milestone 6000 continuum
 *       (always 6000). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_6000  (alias)
 *   __libcgj_batch5991_marker = "libcgj-batch5991"
 *
 * Milestone 6000 exclusive continuum CREATE-ONLY (5991-6000). Unique
 * gj_wave_id_6000 surface only; no multi-def. Distinct from
 * gj_wave_id_5900 (batch5891), gj_wave_id_5800 (batch5791),
 * gj_wave_id_5700 (batch5691), and gj_wave_id_5650 (batch5641).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5991_marker[] = "libcgj-batch5991";

/* Exclusive-wave id for milestone 6000 continuum. */
#define B5991_WAVE_ID  6000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5991_wave_id(void)
{
	return B5991_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_6000 - report the exclusive milestone-6000 wave id.
 *
 * Always returns 6000 (milestone 6000 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_6000(void)
{
	(void)NULL;
	return b5991_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_6000(void)
    __attribute__((alias("gj_wave_id_6000")));
