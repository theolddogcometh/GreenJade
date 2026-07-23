/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3391: milestone 3400 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_3400(void);
 *     - Returns the exclusive wave id for the milestone 3400 continuum
 *       (always 3400). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_3400  (alias)
 *   __libcgj_batch3391_marker = "libcgj-batch3391"
 *
 * Milestone 3400 exclusive continuum CREATE-ONLY (3391-3400). Unique
 * gj_wave_id_3400 surface only; no multi-def. Distinct from
 * gj_wave_id_3300 (batch3291), gj_wave_id_3200 (batch3192),
 * gj_wave_id_3100 (batch3093), and gj_continuum_wave_id_3000
 * (batch2991). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3391_marker[] = "libcgj-batch3391";

/* Exclusive-wave id for milestone 3400 continuum. */
#define B3391_WAVE_ID  3400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3391_wave_id(void)
{
	return B3391_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_3400 - report the exclusive milestone-3400 wave id.
 *
 * Always returns 3400 (milestone 3400 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_3400(void)
{
	(void)NULL;
	return b3391_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_3400(void)
    __attribute__((alias("gj_wave_id_3400")));
