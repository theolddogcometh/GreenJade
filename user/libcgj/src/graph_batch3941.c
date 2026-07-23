/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3941: milestone 3950 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_3950(void);
 *     - Returns the exclusive wave id for the milestone 3950 continuum
 *       (always 3950). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_3950  (alias)
 *   __libcgj_batch3941_marker = "libcgj-batch3941"
 *
 * Milestone 3950 exclusive continuum CREATE-ONLY (3941-3950). Unique
 * gj_wave_id_3950 surface only; no multi-def. Distinct from
 * gj_wave_id_3900 (batch3891), gj_wave_id_3700 (batch3691),
 * gj_wave_id_3600 (batch3591), and gj_wave_id_3550 (batch3541).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3941_marker[] = "libcgj-batch3941";

/* Exclusive-wave id for milestone 3950 continuum. */
#define B3941_WAVE_ID  3950u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3941_wave_id(void)
{
	return B3941_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_3950 - report the exclusive milestone-3950 wave id.
 *
 * Always returns 3950 (milestone 3950 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_3950(void)
{
	(void)NULL;
	return b3941_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_3950(void)
    __attribute__((alias("gj_wave_id_3950")));
