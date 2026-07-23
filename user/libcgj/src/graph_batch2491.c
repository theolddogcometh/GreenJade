/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2491: milestone 2500 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_2500(void);
 *     - Returns the exclusive wave id for the milestone 2500 continuum
 *       (always 2500). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_2500  (alias)
 *   __libcgj_batch2491_marker = "libcgj-batch2491"
 *
 * Milestone 2500 exclusive product helpers (2491-2500). Unique
 * gj_wave_id_2500 surface only; no multi-def. Distinct from
 * gj_wave_id_2400 (batch2391), gj_wave_id_2300 (batch2291), and
 * gj_product_wave_id (batch2191).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2491_marker[] = "libcgj-batch2491";

/* Exclusive-wave id for milestone 2500 continuum. */
#define B2491_WAVE_ID  2500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2491_wave_id(void)
{
	return B2491_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_2500 - report the exclusive milestone-2500 wave id.
 *
 * Always returns 2500 (milestone 2500 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 */
uint32_t
gj_wave_id_2500(void)
{
	(void)NULL;
	return b2491_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_2500(void)
    __attribute__((alias("gj_wave_id_2500")));
