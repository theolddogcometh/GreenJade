/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2291: milestone 2300 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_2300(void);
 *     - Returns the exclusive wave id for the milestone 2300 continuum
 *       (always 2300). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_2300  (alias)
 *   __libcgj_batch2291_marker = "libcgj-batch2291"
 *
 * Milestone 2300 exclusive product helpers (2291-2300). Unique
 * gj_wave_id_2300 surface only; no multi-def. Distinct from
 * gj_product_wave_id (batch2191, wave 2200).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2291_marker[] = "libcgj-batch2291";

/* Exclusive-wave id for milestone 2300 continuum. */
#define B2291_WAVE_ID  2300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2291_wave_id(void)
{
	return B2291_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_2300 - report the exclusive milestone-2300 wave id.
 *
 * Always returns 2300 (milestone 2300 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 */
uint32_t
gj_wave_id_2300(void)
{
	(void)NULL;
	return b2291_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_2300(void)
    __attribute__((alias("gj_wave_id_2300")));
