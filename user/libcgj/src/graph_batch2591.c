/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2591: milestone 2600 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_2600(void);
 *     - Returns the exclusive wave id for the milestone 2600 continuum
 *       (always 2600). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_2600  (alias)
 *   __libcgj_batch2591_marker = "libcgj-batch2591"
 *
 * Milestone 2600 exclusive product helpers (2591-2600). Unique
 * gj_wave_id_2600 surface only; no multi-def. Distinct from
 * gj_wave_id_2500 (batch2491), gj_wave_id_2400 (batch2391),
 * gj_wave_id_2300 (batch2291), and gj_product_wave_id (batch2191).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2591_marker[] = "libcgj-batch2591";

/* Exclusive-wave id for milestone 2600 continuum. */
#define B2591_WAVE_ID  2600u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2591_wave_id(void)
{
	return B2591_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_2600 - report the exclusive milestone-2600 wave id.
 *
 * Always returns 2600 (milestone 2600 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 */
uint32_t
gj_wave_id_2600(void)
{
	(void)NULL;
	return b2591_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_2600(void)
    __attribute__((alias("gj_wave_id_2600")));
