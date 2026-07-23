/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8091: milestone 8100 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_8100(void);
 *     - Returns the exclusive wave id for the milestone 8100 continuum
 *       (always 8100). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_8100  (alias)
 *   __libcgj_batch8091_marker = "libcgj-batch8091"
 *
 * Milestone 8100 exclusive continuum CREATE-ONLY (8091-8100). Unique
 * gj_wave_id_8100 surface only; no multi-def. Distinct from
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

const char __libcgj_batch8091_marker[] = "libcgj-batch8091";

/* Exclusive-wave id for milestone 8100 continuum. */
#define B8091_WAVE_ID  8100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8091_wave_id(void)
{
	return B8091_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_8100 - report the exclusive milestone-8100 wave id.
 *
 * Always returns 8100 (milestone 8100 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_8100(void)
{
	(void)NULL;
	return b8091_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_8100(void)
    __attribute__((alias("gj_wave_id_8100")));
