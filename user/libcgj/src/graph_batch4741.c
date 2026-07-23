/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4741: milestone 4750 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_4750(void);
 *     - Returns the exclusive wave id for the milestone 4750 continuum
 *       (always 4750). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_4750  (alias)
 *   __libcgj_batch4741_marker = "libcgj-batch4741"
 *
 * Milestone 4750 exclusive continuum CREATE-ONLY (4741-4750). Unique
 * gj_wave_id_4750 surface only; no multi-def. Distinct from
 * gj_wave_id_4700 (batch4691), gj_wave_id_4650 (batch4641),
 * gj_wave_id_4600 (batch4591), and gj_wave_id_4550 (batch4541).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4741_marker[] = "libcgj-batch4741";

/* Exclusive-wave id for milestone 4750 continuum. */
#define B4741_WAVE_ID  4750u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4741_wave_id(void)
{
	return B4741_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_4750 - report the exclusive milestone-4750 wave id.
 *
 * Always returns 4750 (milestone 4750 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_4750(void)
{
	(void)NULL;
	return b4741_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_4750(void)
    __attribute__((alias("gj_wave_id_4750")));
