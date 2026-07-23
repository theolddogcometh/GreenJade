/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6299: milestone 6300 continuum ready tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_ready_6300(void);
 *     - Returns 1 (continuum readiness complete for the milestone 6300
 *       continuum). Soft compile-time product tag for exclusive-wave
 *       continuum readiness.
 *   uint32_t __gj_continuum_ready_6300  (alias)
 *   __libcgj_batch6299_marker = "libcgj-batch6299"
 *
 * Milestone 6300 exclusive continuum CREATE-ONLY (6291-6300). Unique
 * gj_continuum_ready_6300 surface only; no multi-def. Distinct from
 * gj_continuum_ready_6200 (batch6199), gj_continuum_ready_6100
 * (batch6099), gj_continuum_ready_6000 (batch5999), and
 * gj_graph_milestone_6300 (batch6300). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6299_marker[] = "libcgj-batch6299";

/* Continuum readiness lamp for milestone 6300 continuum. */
#define B6299_CONTINUUM_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6299_ready(void)
{
	return B6299_CONTINUUM_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_ready_6300 - report the milestone-6300 continuum ready tag.
 *
 * Always returns 1 (continuum readiness complete). Link-time presence
 * of this symbol tags continuum readiness for the wave. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_continuum_ready_6300(void)
{
	(void)NULL;
	return b6299_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_ready_6300(void)
    __attribute__((alias("gj_continuum_ready_6300")));
