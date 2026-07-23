/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5041: milestone 5050 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_5050(void);
 *     - Returns the exclusive wave id for the milestone 5050 continuum
 *       (always 5050). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_5050  (alias)
 *   __libcgj_batch5041_marker = "libcgj-batch5041"
 *
 * Milestone 5050 exclusive continuum CREATE-ONLY (5041-5050). Unique
 * gj_wave_id_5050 surface only; no multi-def. Distinct from
 * gj_wave_id_5000 (batch4991), gj_wave_id_4900 (batch4891),
 * gj_wave_id_4850 (batch4841), and gj_wave_id_4800 (batch4791).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5041_marker[] = "libcgj-batch5041";

/* Exclusive-wave id for milestone 5050 continuum. */
#define B5041_WAVE_ID  5050u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5041_wave_id(void)
{
	return B5041_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_5050 - report the exclusive milestone-5050 wave id.
 *
 * Always returns 5050 (milestone 5050 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_5050(void)
{
	(void)NULL;
	return b5041_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_5050(void)
    __attribute__((alias("gj_wave_id_5050")));
