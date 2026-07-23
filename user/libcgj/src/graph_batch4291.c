/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4291: milestone 4300 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_4300(void);
 *     - Returns the exclusive wave id for the milestone 4300 continuum
 *       (always 4300). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_4300  (alias)
 *   __libcgj_batch4291_marker = "libcgj-batch4291"
 *
 * Milestone 4300 exclusive continuum CREATE-ONLY (4291-4300). Unique
 * gj_wave_id_4300 surface only; no multi-def. Distinct from
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

const char __libcgj_batch4291_marker[] = "libcgj-batch4291";

/* Exclusive-wave id for milestone 4300 continuum. */
#define B4291_WAVE_ID  4300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4291_wave_id(void)
{
	return B4291_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_4300 - report the exclusive milestone-4300 wave id.
 *
 * Always returns 4300 (milestone 4300 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_4300(void)
{
	(void)NULL;
	return b4291_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_4300(void)
    __attribute__((alias("gj_wave_id_4300")));
