/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7491: milestone 7500 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_7500(void);
 *     - Returns the exclusive wave id for the milestone 7500 continuum
 *       (always 7500). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_7500  (alias)
 *   __libcgj_batch7491_marker = "libcgj-batch7491"
 *
 * Milestone 7500 exclusive continuum CREATE-ONLY (7491-7500). Unique
 * gj_wave_id_7500 surface only; no multi-def. Distinct from
 * gj_wave_id_7400 (batch7391), gj_wave_id_7300 (batch7291),
 * gj_wave_id_7200 (batch7191), and gj_wave_id_7100 (batch7091).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7491_marker[] = "libcgj-batch7491";

/* Exclusive-wave id for milestone 7500 continuum. */
#define B7491_WAVE_ID  7500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7491_wave_id(void)
{
	return B7491_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_7500 - report the exclusive milestone-7500 wave id.
 *
 * Always returns 7500 (milestone 7500 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_7500(void)
{
	(void)NULL;
	return b7491_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_7500(void)
    __attribute__((alias("gj_wave_id_7500")));
