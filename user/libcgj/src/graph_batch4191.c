/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4191: milestone 4200 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_4200(void);
 *     - Returns the exclusive wave id for the milestone 4200 continuum
 *       (always 4200). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_4200  (alias)
 *   __libcgj_batch4191_marker = "libcgj-batch4191"
 *
 * Milestone 4200 exclusive continuum CREATE-ONLY (4191-4200). Unique
 * gj_wave_id_4200 surface only; no multi-def. Distinct from
 * gj_wave_id_4100 (batch4091), gj_wave_id_3950 (batch3941),
 * gj_wave_id_3900 (batch3891), and gj_wave_id_3700 (batch3691).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4191_marker[] = "libcgj-batch4191";

/* Exclusive-wave id for milestone 4200 continuum. */
#define B4191_WAVE_ID  4200u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4191_wave_id(void)
{
	return B4191_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_4200 - report the exclusive milestone-4200 wave id.
 *
 * Always returns 4200 (milestone 4200 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_4200(void)
{
	(void)NULL;
	return b4191_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_4200(void)
    __attribute__((alias("gj_wave_id_4200")));
