/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7991: milestone 8000 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_8000(void);
 *     - Returns the exclusive wave id for the milestone 8000 continuum
 *       (always 8000). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_8000  (alias)
 *   __libcgj_batch7991_marker = "libcgj-batch7991"
 *
 * Milestone 8000 exclusive continuum CREATE-ONLY (7991-8000). Unique
 * gj_wave_id_8000 surface only; no multi-def. Distinct from
 * gj_wave_id_7000 (batch6991), gj_wave_id_6000 (batch5991),
 * gj_wave_id_5900 (batch5891), and gj_wave_id_5800 (batch5791).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7991_marker[] = "libcgj-batch7991";

/* Exclusive-wave id for milestone 8000 continuum. */
#define B7991_WAVE_ID  8000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7991_wave_id(void)
{
	return B7991_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_8000 - report the exclusive milestone-8000 wave id.
 *
 * Always returns 8000 (milestone 8000 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_8000(void)
{
	(void)NULL;
	return b7991_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_8000(void)
    __attribute__((alias("gj_wave_id_8000")));
