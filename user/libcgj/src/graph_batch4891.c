/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4891: milestone 4900 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_4900(void);
 *     - Returns the exclusive wave id for the milestone 4900 continuum
 *       (always 4900). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_4900  (alias)
 *   __libcgj_batch4891_marker = "libcgj-batch4891"
 *
 * Milestone 4900 exclusive continuum CREATE-ONLY (4891-4900). Unique
 * gj_wave_id_4900 surface only; no multi-def. Distinct from
 * gj_wave_id_4850 (batch4841), gj_wave_id_4800 (batch4791),
 * gj_wave_id_4750 (batch4741), and gj_wave_id_4700 (batch4691).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4891_marker[] = "libcgj-batch4891";

/* Exclusive-wave id for milestone 4900 continuum. */
#define B4891_WAVE_ID  4900u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4891_wave_id(void)
{
	return B4891_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_4900 - report the exclusive milestone-4900 wave id.
 *
 * Always returns 4900 (milestone 4900 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_4900(void)
{
	(void)NULL;
	return b4891_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_4900(void)
    __attribute__((alias("gj_wave_id_4900")));
