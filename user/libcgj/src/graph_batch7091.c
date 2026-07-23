/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7091: milestone 7100 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_7100(void);
 *     - Returns the exclusive wave id for the milestone 7100 continuum
 *       (always 7100). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_7100  (alias)
 *   __libcgj_batch7091_marker = "libcgj-batch7091"
 *
 * Milestone 7100 exclusive continuum CREATE-ONLY (7091-7100). Unique
 * gj_wave_id_7100 surface only; no multi-def. Distinct from
 * gj_wave_id_7000 (batch6991), gj_wave_id_6000 (batch5991),
 * gj_wave_id_5900 (batch5891), and gj_wave_id_5800 (batch5791).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7091_marker[] = "libcgj-batch7091";

/* Exclusive-wave id for milestone 7100 continuum. */
#define B7091_WAVE_ID  7100u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7091_wave_id(void)
{
	return B7091_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_7100 - report the exclusive milestone-7100 wave id.
 *
 * Always returns 7100 (milestone 7100 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_7100(void)
{
	(void)NULL;
	return b7091_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_7100(void)
    __attribute__((alias("gj_wave_id_7100")));
