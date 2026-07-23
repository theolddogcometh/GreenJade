/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4791: milestone 4800 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_4800(void);
 *     - Returns the exclusive wave id for the milestone 4800 continuum
 *       (always 4800). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_4800  (alias)
 *   __libcgj_batch4791_marker = "libcgj-batch4791"
 *
 * Milestone 4800 exclusive continuum CREATE-ONLY (4791-4800). Unique
 * gj_wave_id_4800 surface only; no multi-def. Distinct from
 * gj_wave_id_4750 (batch4741), gj_wave_id_4700 (batch4691),
 * gj_wave_id_4650 (batch4641), and gj_wave_id_4600 (batch4591).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4791_marker[] = "libcgj-batch4791";

/* Exclusive-wave id for milestone 4800 continuum. */
#define B4791_WAVE_ID  4800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4791_wave_id(void)
{
	return B4791_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_4800 - report the exclusive milestone-4800 wave id.
 *
 * Always returns 4800 (milestone 4800 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_4800(void)
{
	(void)NULL;
	return b4791_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_4800(void)
    __attribute__((alias("gj_wave_id_4800")));
