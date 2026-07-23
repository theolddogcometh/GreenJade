/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7391: milestone 7400 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_7400(void);
 *     - Returns the exclusive wave id for the milestone 7400 continuum
 *       (always 7400). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_7400  (alias)
 *   __libcgj_batch7391_marker = "libcgj-batch7391"
 *
 * Milestone 7400 exclusive continuum CREATE-ONLY (7391-7400). Unique
 * gj_wave_id_7400 surface only; no multi-def. Distinct from
 * gj_wave_id_7300 (batch7291), gj_wave_id_7200 (batch7191),
 * gj_wave_id_7100 (batch7091), and gj_wave_id_7000 (batch6991).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7391_marker[] = "libcgj-batch7391";

/* Exclusive-wave id for milestone 7400 continuum. */
#define B7391_WAVE_ID  7400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7391_wave_id(void)
{
	return B7391_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_7400 - report the exclusive milestone-7400 wave id.
 *
 * Always returns 7400 (milestone 7400 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_7400(void)
{
	(void)NULL;
	return b7391_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_7400(void)
    __attribute__((alias("gj_wave_id_7400")));
