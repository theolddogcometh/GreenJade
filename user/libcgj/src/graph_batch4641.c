/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4641: milestone 4650 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_4650(void);
 *     - Returns the exclusive wave id for the milestone 4650 continuum
 *       (always 4650). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_4650  (alias)
 *   __libcgj_batch4641_marker = "libcgj-batch4641"
 *
 * Milestone 4650 exclusive continuum CREATE-ONLY (4641-4650). Unique
 * gj_wave_id_4650 surface only; no multi-def. Distinct from
 * gj_wave_id_4600 (batch4591), gj_wave_id_4550 (batch4541),
 * gj_wave_id_4500 (batch4491), and gj_wave_id_4400 (batch4391).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4641_marker[] = "libcgj-batch4641";

/* Exclusive-wave id for milestone 4650 continuum. */
#define B4641_WAVE_ID  4650u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4641_wave_id(void)
{
	return B4641_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_4650 - report the exclusive milestone-4650 wave id.
 *
 * Always returns 4650 (milestone 4650 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_4650(void)
{
	(void)NULL;
	return b4641_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_4650(void)
    __attribute__((alias("gj_wave_id_4650")));
