/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4691: milestone 4700 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_4700(void);
 *     - Returns the exclusive wave id for the milestone 4700 continuum
 *       (always 4700). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_4700  (alias)
 *   __libcgj_batch4691_marker = "libcgj-batch4691"
 *
 * Milestone 4700 exclusive continuum CREATE-ONLY (4691-4700). Unique
 * gj_wave_id_4700 surface only; no multi-def. Distinct from
 * gj_wave_id_4650 (batch4641), gj_wave_id_4600 (batch4591),
 * gj_wave_id_4550 (batch4541), and gj_wave_id_4500 (batch4491).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4691_marker[] = "libcgj-batch4691";

/* Exclusive-wave id for milestone 4700 continuum. */
#define B4691_WAVE_ID  4700u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4691_wave_id(void)
{
	return B4691_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_4700 - report the exclusive milestone-4700 wave id.
 *
 * Always returns 4700 (milestone 4700 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_4700(void)
{
	(void)NULL;
	return b4691_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_4700(void)
    __attribute__((alias("gj_wave_id_4700")));
