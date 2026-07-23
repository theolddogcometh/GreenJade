/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10295: milestone 10300 continuum ready tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_ready_10300(void);
 *     - Returns 1 (continuum readiness complete for the milestone 10300
 *       continuum). Soft compile-time product tag for exclusive-wave
 *       continuum readiness.
 *   uint32_t __gj_continuum_ready_10300  (alias)
 *   __libcgj_batch10295_marker = "libcgj-batch10295"
 *
 * Milestone 10300 exclusive continuum CREATE-ONLY (10291-10300). Unique
 * gj_continuum_ready_10300 surface only; no multi-def. Distinct from
 * gj_continuum_ready_10200 (batch10195), gj_continuum_ready_10100
 * (batch10095), gj_continuum_ready_10000 (batch9995), and
 * gj_graph_milestone_10300 (batch10300). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10295_marker[] = "libcgj-batch10295";

/* Continuum readiness lamp for milestone 10300 continuum. */
#define B10295_CONTINUUM_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10295_ready(void)
{
	return B10295_CONTINUUM_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_ready_10300 - report the milestone-10300 continuum ready tag.
 *
 * Always returns 1 (continuum readiness complete). Link-time presence
 * of this symbol tags continuum readiness for the wave. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_continuum_ready_10300(void)
{
	(void)NULL;
	return b10295_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_ready_10300(void)
    __attribute__((alias("gj_continuum_ready_10300")));
