/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2391: milestone 2400 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_2400(void);
 *     - Returns the exclusive wave id for the milestone 2400 continuum
 *       (always 2400). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_2400  (alias)
 *   __libcgj_batch2391_marker = "libcgj-batch2391"
 *
 * Milestone 2400 exclusive product helpers (2391-2400). Unique
 * gj_wave_id_2400 surface only; no multi-def. Distinct from
 * gj_wave_id_2300 (batch2291) and gj_product_wave_id (batch2191).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2391_marker[] = "libcgj-batch2391";

/* Exclusive-wave id for milestone 2400 continuum. */
#define B2391_WAVE_ID  2400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2391_wave_id(void)
{
	return B2391_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_2400 - report the exclusive milestone-2400 wave id.
 *
 * Always returns 2400 (milestone 2400 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 */
uint32_t
gj_wave_id_2400(void)
{
	(void)NULL;
	return b2391_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_2400(void)
    __attribute__((alias("gj_wave_id_2400")));
