/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5341: milestone 5350 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_5350(void);
 *     - Returns the exclusive wave id for the milestone 5350 continuum
 *       (always 5350). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_5350  (alias)
 *   __libcgj_batch5341_marker = "libcgj-batch5341"
 *
 * Milestone 5350 exclusive continuum CREATE-ONLY (5341-5350). Unique
 * gj_wave_id_5350 surface only; no multi-def. Distinct from
 * gj_wave_id_5300 (batch5291), gj_wave_id_5200 (batch5191),
 * gj_wave_id_5100 (batch5091), and gj_wave_id_5000 (batch4991).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5341_marker[] = "libcgj-batch5341";

/* Exclusive-wave id for milestone 5350 continuum. */
#define B5341_WAVE_ID  5350u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5341_wave_id(void)
{
	return B5341_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_5350 - report the exclusive milestone-5350 wave id.
 *
 * Always returns 5350 (milestone 5350 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_5350(void)
{
	(void)NULL;
	return b5341_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_5350(void)
    __attribute__((alias("gj_wave_id_5350")));
