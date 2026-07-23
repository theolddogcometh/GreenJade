/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3691: milestone 3700 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_3700(void);
 *     - Returns the exclusive wave id for the milestone 3700 continuum
 *       (always 3700). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_3700  (alias)
 *   __libcgj_batch3691_marker = "libcgj-batch3691"
 *
 * Milestone 3700 exclusive continuum CREATE-ONLY (3691-3700). Unique
 * gj_wave_id_3700 surface only; no multi-def. Distinct from
 * gj_wave_id_3600 (batch3591), gj_wave_id_3550 (batch3541),
 * gj_wave_id_3500 (batch3491), and gj_wave_id_3400 (batch3391).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3691_marker[] = "libcgj-batch3691";

/* Exclusive-wave id for milestone 3700 continuum. */
#define B3691_WAVE_ID  3700u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3691_wave_id(void)
{
	return B3691_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_3700 - report the exclusive milestone-3700 wave id.
 *
 * Always returns 3700 (milestone 3700 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_3700(void)
{
	(void)NULL;
	return b3691_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_3700(void)
    __attribute__((alias("gj_wave_id_3700")));
