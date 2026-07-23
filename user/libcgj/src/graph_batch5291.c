/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5291: milestone 5300 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_5300(void);
 *     - Returns the exclusive wave id for the milestone 5300 continuum
 *       (always 5300). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_5300  (alias)
 *   __libcgj_batch5291_marker = "libcgj-batch5291"
 *
 * Milestone 5300 exclusive continuum CREATE-ONLY (5291-5300). Unique
 * gj_wave_id_5300 surface only; no multi-def. Distinct from
 * gj_wave_id_5200 (batch5191), gj_wave_id_5100 (batch5091),
 * gj_wave_id_5000 (batch4991), and gj_wave_id_4900 (batch4891).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5291_marker[] = "libcgj-batch5291";

/* Exclusive-wave id for milestone 5300 continuum. */
#define B5291_WAVE_ID  5300u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5291_wave_id(void)
{
	return B5291_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_5300 - report the exclusive milestone-5300 wave id.
 *
 * Always returns 5300 (milestone 5300 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_5300(void)
{
	(void)NULL;
	return b5291_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_5300(void)
    __attribute__((alias("gj_wave_id_5300")));
