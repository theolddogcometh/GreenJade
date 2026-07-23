/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3999: milestone 4000 continuum ready tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_ready_4000(void);
 *     - Returns the continuum readiness tag for the milestone 4000
 *       continuum (always 4000). Soft compile-time product tag for
 *       exclusive-wave continuum readiness complete.
 *   uint32_t __gj_continuum_ready_4000  (alias)
 *   __libcgj_batch3999_marker = "libcgj-batch3999"
 *
 * Milestone 4000 exclusive continuum CREATE-ONLY (3991-4000). Unique
 * gj_continuum_ready_4000 surface only; no multi-def. Distinct from
 * gj_continuum_ready_3950 (batch3949), gj_continuum_ready_3900
 * (batch3899), gj_continuum_ready_3700 (batch3699), and
 * gj_graph_milestone_4000 (batch4000). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3999_marker[] = "libcgj-batch3999";

/* Continuum readiness tag for milestone 4000 continuum. */
#define B3999_CONTINUUM_READY  4000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3999_ready(void)
{
	return B3999_CONTINUUM_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_ready_4000 - report the milestone-4000 continuum ready tag.
 *
 * Always returns 4000 (MILESTONE 4000 continuum readiness). Link-time
 * presence of this symbol tags continuum readiness for the wave.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_continuum_ready_4000(void)
{
	(void)NULL;
	return b3999_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_ready_4000(void)
    __attribute__((alias("gj_continuum_ready_4000")));
