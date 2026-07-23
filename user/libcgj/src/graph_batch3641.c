/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3641: milestone 3650 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_3650(void);
 *     - Returns the exclusive wave id for the milestone 3650 continuum
 *       (always 3650). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_3650  (alias)
 *   __libcgj_batch3641_marker = "libcgj-batch3641"
 *
 * Milestone 3650 exclusive continuum CREATE-ONLY (3641-3650). Unique
 * gj_wave_id_3650 surface only; no multi-def. Distinct from
 * gj_wave_id_3600 (batch3591), gj_wave_id_3550 (batch3541),
 * gj_wave_id_3500 (batch3491), and gj_wave_id_3400 (batch3391).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3641_marker[] = "libcgj-batch3641";

/* Exclusive-wave id for milestone 3650 continuum. */
#define B3641_WAVE_ID  3650u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3641_wave_id(void)
{
	return B3641_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_3650 - report the exclusive milestone-3650 wave id.
 *
 * Always returns 3650 (milestone 3650 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_3650(void)
{
	(void)NULL;
	return b3641_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_3650(void)
    __attribute__((alias("gj_wave_id_3650")));
