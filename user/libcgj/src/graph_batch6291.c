/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6291: milestone 6300 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_6300(void);
 *     - Returns the exclusive wave id for the milestone 6300 continuum
 *       (always 6300). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_6300  (alias)
 *   __libcgj_batch6291_marker = "libcgj-batch6291"
 *
 * Milestone 6300 exclusive continuum CREATE-ONLY (6291-6300). Unique
 * gj_wave_id_6300 surface only; no multi-def. Distinct from
 * gj_wave_id_6200 (batch6191), gj_wave_id_6100 (batch6091),
 * gj_wave_id_6000 (batch5991), and gj_wave_id_5900 (batch5891).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6291_marker[] = "libcgj-batch6291";

/* Exclusive-wave id for milestone 6300 continuum. */
#define B6291_WAVE_ID  6300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6291_wave_id(void)
{
	return B6291_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_6300 - report the exclusive milestone-6300 wave id.
 *
 * Always returns 6300 (milestone 6300 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_6300(void)
{
	(void)NULL;
	return b6291_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_6300(void)
    __attribute__((alias("gj_wave_id_6300")));
