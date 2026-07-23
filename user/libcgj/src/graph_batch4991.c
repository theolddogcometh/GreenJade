/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4991: milestone 5000 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_5000(void);
 *     - Returns the exclusive wave id for the milestone 5000 continuum
 *       (always 5000). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_5000  (alias)
 *   __libcgj_batch4991_marker = "libcgj-batch4991"
 *
 * Milestone 5000 exclusive continuum CREATE-ONLY (4991-5000). Unique
 * gj_wave_id_5000 surface only; no multi-def. Distinct from
 * gj_wave_id_4900 (batch4891), gj_wave_id_4850 (batch4841),
 * gj_wave_id_4800 (batch4791), and gj_wave_id_4750 (batch4741).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4991_marker[] = "libcgj-batch4991";

/* Exclusive-wave id for milestone 5000 continuum. */
#define B4991_WAVE_ID  5000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4991_wave_id(void)
{
	return B4991_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_5000 - report the exclusive milestone-5000 wave id.
 *
 * Always returns 5000 (milestone 5000 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_5000(void)
{
	(void)NULL;
	return b4991_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_5000(void)
    __attribute__((alias("gj_wave_id_5000")));
