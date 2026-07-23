/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3491: milestone 3500 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_3500(void);
 *     - Returns the exclusive wave id for the milestone 3500 continuum
 *       (always 3500). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_3500  (alias)
 *   __libcgj_batch3491_marker = "libcgj-batch3491"
 *
 * Milestone 3500 exclusive continuum CREATE-ONLY (3491-3500). Unique
 * gj_wave_id_3500 surface only; no multi-def. Distinct from
 * gj_wave_id_3400 (batch3391), gj_wave_id_3300 (batch3291),
 * gj_wave_id_3200 (batch3192), and gj_wave_id_3100 (batch3093).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3491_marker[] = "libcgj-batch3491";

/* Exclusive-wave id for milestone 3500 continuum. */
#define B3491_WAVE_ID  3500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3491_wave_id(void)
{
	return B3491_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_3500 - report the exclusive milestone-3500 wave id.
 *
 * Always returns 3500 (milestone 3500 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_3500(void)
{
	(void)NULL;
	return b3491_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_3500(void)
    __attribute__((alias("gj_wave_id_3500")));
