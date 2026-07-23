/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5091: milestone 5100 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_5100(void);
 *     - Returns the exclusive wave id for the milestone 5100 continuum
 *       (always 5100). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_5100  (alias)
 *   __libcgj_batch5091_marker = "libcgj-batch5091"
 *
 * Milestone 5100 exclusive continuum CREATE-ONLY (5091-5100). Unique
 * gj_wave_id_5100 surface only; no multi-def. Distinct from
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

const char __libcgj_batch5091_marker[] = "libcgj-batch5091";

/* Exclusive-wave id for milestone 5100 continuum. */
#define B5091_WAVE_ID  5100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5091_wave_id(void)
{
	return B5091_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_5100 - report the exclusive milestone-5100 wave id.
 *
 * Always returns 5100 (milestone 5100 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_5100(void)
{
	(void)NULL;
	return b5091_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_5100(void)
    __attribute__((alias("gj_wave_id_5100")));
