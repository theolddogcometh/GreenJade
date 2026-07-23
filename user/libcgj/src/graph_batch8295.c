/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8295: milestone 8300 continuum ready tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_ready_8300(void);
 *     - Returns 1 (continuum readiness complete for the milestone 8300
 *       continuum). Soft compile-time product tag for exclusive-wave
 *       continuum readiness.
 *   uint32_t __gj_continuum_ready_8300  (alias)
 *   __libcgj_batch8295_marker = "libcgj-batch8295"
 *
 * Milestone 8300 exclusive continuum CREATE-ONLY (8291-8300). Unique
 * gj_continuum_ready_8300 surface only; no multi-def. Distinct from
 * gj_continuum_ready_8200 (batch8199), gj_continuum_ready_8100
 * (batch8099), gj_continuum_ready_8000 (batch7999), and
 * gj_graph_milestone_8300 (batch8300). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8295_marker[] = "libcgj-batch8295";

/* Continuum readiness lamp for milestone 8300 continuum. */
#define B8295_CONTINUUM_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8295_ready(void)
{
	return B8295_CONTINUUM_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_ready_8300 - report the milestone-8300 continuum ready tag.
 *
 * Always returns 1 (continuum readiness complete). Link-time presence
 * of this symbol tags continuum readiness for the wave. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_continuum_ready_8300(void)
{
	(void)NULL;
	return b8295_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_ready_8300(void)
    __attribute__((alias("gj_continuum_ready_8300")));
