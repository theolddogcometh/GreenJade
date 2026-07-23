/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6391: milestone 6400 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_6400(void);
 *     - Returns the exclusive wave id for the milestone 6400 continuum
 *       (always 6400). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_6400  (alias)
 *   __libcgj_batch6391_marker = "libcgj-batch6391"
 *
 * Milestone 6400 exclusive continuum CREATE-ONLY (6391-6400). Unique
 * gj_wave_id_6400 surface only; no multi-def. Distinct from
 * gj_wave_id_6300 (batch6291), gj_wave_id_6200 (batch6191),
 * gj_wave_id_6100 (batch6091), and gj_wave_id_6000 (batch5991).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6391_marker[] = "libcgj-batch6391";

/* Exclusive-wave id for milestone 6400 continuum. */
#define B6391_WAVE_ID  6400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6391_wave_id(void)
{
	return B6391_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_6400 - report the exclusive milestone-6400 wave id.
 *
 * Always returns 6400 (milestone 6400 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_6400(void)
{
	(void)NULL;
	return b6391_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_6400(void)
    __attribute__((alias("gj_wave_id_6400")));
