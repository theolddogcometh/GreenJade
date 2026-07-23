/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5141: milestone 5150 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_5150(void);
 *     - Returns the exclusive wave id for the milestone 5150 continuum
 *       (always 5150). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_5150  (alias)
 *   __libcgj_batch5141_marker = "libcgj-batch5141"
 *
 * Milestone 5150 exclusive continuum CREATE-ONLY (5141-5150). Unique
 * gj_wave_id_5150 surface only; no multi-def. Distinct from
 * gj_wave_id_5100 (batch5091), gj_wave_id_5000 (batch4991),
 * gj_wave_id_4900 (batch4891), and gj_wave_id_4850 (batch4841).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5141_marker[] = "libcgj-batch5141";

/* Exclusive-wave id for milestone 5150 continuum. */
#define B5141_WAVE_ID  5150u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5141_wave_id(void)
{
	return B5141_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_5150 - report the exclusive milestone-5150 wave id.
 *
 * Always returns 5150 (milestone 5150 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_5150(void)
{
	(void)NULL;
	return b5141_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_5150(void)
    __attribute__((alias("gj_wave_id_5150")));
