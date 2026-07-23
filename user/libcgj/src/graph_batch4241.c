/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4241: milestone 4250 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_4250(void);
 *     - Returns the exclusive wave id for the milestone 4250 continuum
 *       (always 4250). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_4250  (alias)
 *   __libcgj_batch4241_marker = "libcgj-batch4241"
 *
 * Milestone 4250 exclusive continuum CREATE-ONLY (4241-4250). Unique
 * gj_wave_id_4250 surface only; no multi-def. Distinct from
 * gj_wave_id_4100 (batch4091), gj_wave_id_4000 (batch3991),
 * gj_wave_id_3950 (batch3941), and gj_wave_id_3900 (batch3891).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4241_marker[] = "libcgj-batch4241";

/* Exclusive-wave id for milestone 4250 continuum. */
#define B4241_WAVE_ID  4250u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4241_wave_id(void)
{
	return B4241_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_4250 - report the exclusive milestone-4250 wave id.
 *
 * Always returns 4250 (milestone 4250 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_4250(void)
{
	(void)NULL;
	return b4241_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_4250(void)
    __attribute__((alias("gj_wave_id_4250")));
