/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4541: milestone 4550 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_4550(void);
 *     - Returns the exclusive wave id for the milestone 4550 continuum
 *       (always 4550). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_4550  (alias)
 *   __libcgj_batch4541_marker = "libcgj-batch4541"
 *
 * Milestone 4550 exclusive continuum CREATE-ONLY (4541-4550). Unique
 * gj_wave_id_4550 surface only; no multi-def. Distinct from
 * gj_wave_id_4500 (batch4491), gj_wave_id_4400 (batch4391),
 * gj_wave_id_4300 (batch4291), and gj_wave_id_4100 (batch4091).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4541_marker[] = "libcgj-batch4541";

/* Exclusive-wave id for milestone 4550 continuum. */
#define B4541_WAVE_ID  4550u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4541_wave_id(void)
{
	return B4541_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_4550 - report the exclusive milestone-4550 wave id.
 *
 * Always returns 4550 (milestone 4550 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_4550(void)
{
	(void)NULL;
	return b4541_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_4550(void)
    __attribute__((alias("gj_wave_id_4550")));
