/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4491: milestone 4500 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_4500(void);
 *     - Returns the exclusive wave id for the milestone 4500 continuum
 *       (always 4500). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_4500  (alias)
 *   __libcgj_batch4491_marker = "libcgj-batch4491"
 *
 * Milestone 4500 exclusive continuum CREATE-ONLY (4491-4500). Unique
 * gj_wave_id_4500 surface only; no multi-def. Distinct from
 * gj_wave_id_4400 (batch4391), gj_wave_id_4300 (batch4291),
 * gj_wave_id_4100 (batch4091), and gj_wave_id_3950 (batch3941).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4491_marker[] = "libcgj-batch4491";

/* Exclusive-wave id for milestone 4500 continuum. */
#define B4491_WAVE_ID  4500u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4491_wave_id(void)
{
	return B4491_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_4500 - report the exclusive milestone-4500 wave id.
 *
 * Always returns 4500 (milestone 4500 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_4500(void)
{
	(void)NULL;
	return b4491_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_4500(void)
    __attribute__((alias("gj_wave_id_4500")));
