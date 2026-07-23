/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7291: milestone 7300 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_7300(void);
 *     - Returns the exclusive wave id for the milestone 7300 continuum
 *       (always 7300). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_7300  (alias)
 *   __libcgj_batch7291_marker = "libcgj-batch7291"
 *
 * Milestone 7300 exclusive continuum CREATE-ONLY (7291-7300). Unique
 * gj_wave_id_7300 surface only; no multi-def. Distinct from
 * gj_wave_id_7200 (batch7191), gj_wave_id_7100 (batch7091),
 * gj_wave_id_7000 (batch6991), and gj_wave_id_6000 (batch5991).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7291_marker[] = "libcgj-batch7291";

/* Exclusive-wave id for milestone 7300 continuum. */
#define B7291_WAVE_ID  7300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7291_wave_id(void)
{
	return B7291_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_7300 - report the exclusive milestone-7300 wave id.
 *
 * Always returns 7300 (milestone 7300 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_7300(void)
{
	(void)NULL;
	return b7291_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_7300(void)
    __attribute__((alias("gj_wave_id_7300")));
