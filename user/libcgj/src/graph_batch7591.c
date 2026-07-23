/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7591: milestone 7600 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_7600(void);
 *     - Returns the exclusive wave id for the milestone 7600 continuum
 *       (always 7600). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_7600  (alias)
 *   __libcgj_batch7591_marker = "libcgj-batch7591"
 *
 * Milestone 7600 exclusive continuum CREATE-ONLY (7591-7600). Unique
 * gj_wave_id_7600 surface only; no multi-def. Distinct from
 * gj_wave_id_7500 (batch7491), gj_wave_id_7400 (batch7391),
 * gj_wave_id_7300 (batch7291), and gj_wave_id_7200 (batch7191).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7591_marker[] = "libcgj-batch7591";

/* Exclusive-wave id for milestone 7600 continuum. */
#define B7591_WAVE_ID  7600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7591_wave_id(void)
{
	return B7591_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_7600 - report the exclusive milestone-7600 wave id.
 *
 * Always returns 7600 (milestone 7600 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_7600(void)
{
	(void)NULL;
	return b7591_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_7600(void)
    __attribute__((alias("gj_wave_id_7600")));
