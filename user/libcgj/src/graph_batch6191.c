/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6191: milestone 6200 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_6200(void);
 *     - Returns the exclusive wave id for the milestone 6200 continuum
 *       (always 6200). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_6200  (alias)
 *   __libcgj_batch6191_marker = "libcgj-batch6191"
 *
 * Milestone 6200 exclusive continuum CREATE-ONLY (6191-6200). Unique
 * gj_wave_id_6200 surface only; no multi-def. Distinct from
 * gj_wave_id_6000 (batch5991), gj_wave_id_5900 (batch5891),
 * gj_wave_id_5800 (batch5791), and gj_wave_id_5700 (batch5691).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6191_marker[] = "libcgj-batch6191";

/* Exclusive-wave id for milestone 6200 continuum. */
#define B6191_WAVE_ID  6200u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6191_wave_id(void)
{
	return B6191_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_6200 - report the exclusive milestone-6200 wave id.
 *
 * Always returns 6200 (milestone 6200 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_6200(void)
{
	(void)NULL;
	return b6191_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_6200(void)
    __attribute__((alias("gj_wave_id_6200")));
