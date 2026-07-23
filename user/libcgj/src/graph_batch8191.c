/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8191: milestone 8200 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_8200(void);
 *     - Returns the exclusive wave id for the milestone 8200 continuum
 *       (always 8200). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_8200  (alias)
 *   __libcgj_batch8191_marker = "libcgj-batch8191"
 *
 * Milestone 8200 exclusive continuum CREATE-ONLY (8191-8200). Unique
 * gj_wave_id_8200 surface only; no multi-def. Distinct from
 * gj_wave_id_8000 (batch7991), gj_wave_id_7000 (batch6991),
 * gj_wave_id_6000 (batch5991), and gj_wave_id_5900 (batch5891).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8191_marker[] = "libcgj-batch8191";

/* Exclusive-wave id for milestone 8200 continuum. */
#define B8191_WAVE_ID  8200u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8191_wave_id(void)
{
	return B8191_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_8200 - report the exclusive milestone-8200 wave id.
 *
 * Always returns 8200 (milestone 8200 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_8200(void)
{
	(void)NULL;
	return b8191_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_8200(void)
    __attribute__((alias("gj_wave_id_8200")));
