/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5891: milestone 5900 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_5900(void);
 *     - Returns the exclusive wave id for the milestone 5900 continuum
 *       (always 5900). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_5900  (alias)
 *   __libcgj_batch5891_marker = "libcgj-batch5891"
 *
 * Milestone 5900 exclusive continuum CREATE-ONLY (5891-5900). Unique
 * gj_wave_id_5900 surface only; no multi-def. Distinct from
 * gj_wave_id_5800 (batch5791), gj_wave_id_5700 (batch5691),
 * gj_wave_id_5650 (batch5641), and gj_wave_id_5600 (batch5591).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5891_marker[] = "libcgj-batch5891";

/* Exclusive-wave id for milestone 5900 continuum. */
#define B5891_WAVE_ID  5900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5891_wave_id(void)
{
	return B5891_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_5900 - report the exclusive milestone-5900 wave id.
 *
 * Always returns 5900 (milestone 5900 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_5900(void)
{
	(void)NULL;
	return b5891_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_5900(void)
    __attribute__((alias("gj_wave_id_5900")));
