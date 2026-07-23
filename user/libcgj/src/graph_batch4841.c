/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4841: milestone 4850 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_4850(void);
 *     - Returns the exclusive wave id for the milestone 4850 continuum
 *       (always 4850). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_4850  (alias)
 *   __libcgj_batch4841_marker = "libcgj-batch4841"
 *
 * Milestone 4850 exclusive continuum CREATE-ONLY (4841-4850). Unique
 * gj_wave_id_4850 surface only; no multi-def. Distinct from
 * gj_wave_id_4800 (batch4791), gj_wave_id_4750 (batch4741),
 * gj_wave_id_4700 (batch4691), and gj_wave_id_4650 (batch4641).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4841_marker[] = "libcgj-batch4841";

/* Exclusive-wave id for milestone 4850 continuum. */
#define B4841_WAVE_ID  4850u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4841_wave_id(void)
{
	return B4841_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_4850 - report the exclusive milestone-4850 wave id.
 *
 * Always returns 4850 (milestone 4850 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_4850(void)
{
	(void)NULL;
	return b4841_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_4850(void)
    __attribute__((alias("gj_wave_id_4850")));
