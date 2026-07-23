/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4941: milestone 4950 exclusive wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wave_id_4950(void);
 *     - Returns the exclusive wave id for the milestone 4950 continuum
 *       (always 4950). Soft compile-time product tag.
 *   uint32_t __gj_wave_id_4950  (alias)
 *   __libcgj_batch4941_marker = "libcgj-batch4941"
 *
 * Milestone 4950 exclusive continuum CREATE-ONLY (4941-4950). Unique
 * gj_wave_id_4950 surface only; no multi-def. Distinct from
 * gj_wave_id_4900 (batch4891), gj_wave_id_4850 (batch4841),
 * gj_wave_id_4800 (batch4791), and gj_wave_id_4750 (batch4741).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4941_marker[] = "libcgj-batch4941";

/* Exclusive-wave id for milestone 4950 continuum. */
#define B4941_WAVE_ID  4950u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4941_wave_id(void)
{
	return B4941_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wave_id_4950 - report the exclusive milestone-4950 wave id.
 *
 * Always returns 4950 (milestone 4950 product continuum). Link-time
 * presence of this symbol tags the wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_wave_id_4950(void)
{
	(void)NULL;
	return b4941_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wave_id_4950(void)
    __attribute__((alias("gj_wave_id_4950")));
