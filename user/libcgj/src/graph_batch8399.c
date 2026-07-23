/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8399: milestone 8400 continuum wave id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_wave_8400(void);
 *     - Returns the exclusive continuum wave id for the milestone 8400
 *       continuum (always 8400). Soft compile-time product tag.
 *   uint32_t __gj_continuum_wave_8400  (alias)
 *   __libcgj_batch8399_marker = "libcgj-batch8399"
 *
 * Milestone 8400 exclusive continuum CREATE-ONLY (8391-8400). Unique
 * gj_continuum_wave_8400 surface only; no multi-def. Distinct from
 * gj_continuum_ready_8400 (batch8395), gj_wave_id_8200 (batch8191),
 * gj_continuum_wave_id_3000 (batch2991), and gj_graph_milestone_8400
 * (batch8400). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8399_marker[] = "libcgj-batch8399";

/* Exclusive continuum wave id for milestone 8400. */
#define B8399_CONTINUUM_WAVE  8400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8399_wave(void)
{
	return B8399_CONTINUUM_WAVE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_wave_8400 - report the exclusive milestone-8400 wave id.
 *
 * Always returns 8400 (milestone 8400 product continuum). Link-time
 * presence of this symbol tags the continuum wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_wave_8400(void)
{
	(void)NULL;
	return b8399_wave();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_wave_8400(void)
    __attribute__((alias("gj_continuum_wave_8400")));
