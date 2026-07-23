/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6491: milestone 6500 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_6500(void);
 *     - Returns the exclusive wave id for the milestone 6500 continuum
 *       (always 6500). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_6500  (alias)
 *   __libcgj_batch6491_marker = "libcgj-batch6491"
 *
 * Milestone 6500 exclusive continuum CREATE-ONLY (6491-6500). Unique
 * gj_wave_id_6500 surface only; no multi-def. Distinct from
 * gj_wave_id_6400 (batch6391), gj_wave_id_6300 (batch6291),
 * gj_wave_id_6200 (batch6191), and gj_wave_id_6100 (batch6091).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6491_marker[] = "libcgj-batch6491";

/* Exclusive-wave id for milestone 6500 continuum. */
#define B6491_WAVE_ID  6500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6491_wave_id(void)
{
	return B6491_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_6500 - report the exclusive milestone-6500 wave id.
 *
 * Always returns 6500 (milestone 6500 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_6500(void)
{
	(void)NULL;
	return b6491_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_6500(void)
    __attribute__((alias("gj_wave_id_6500")));
