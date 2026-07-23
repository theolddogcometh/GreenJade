/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14295: milestone 14300 continuum ready tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_ready_14300(void);
 *     - Returns 1 (continuum readiness complete for the milestone 14300
 *       continuum). Soft compile-time product tag for exclusive-wave
 *       continuum readiness.
 *   uint32_t __gj_continuum_ready_14300  (alias)
 *   __libcgj_batch14295_marker = "libcgj-batch14295"
 *
 * Milestone 14300 exclusive continuum CREATE-ONLY (14291-14300). Unique
 * gj_continuum_ready_14300 surface only; no multi-def. Distinct from
 * gj_continuum_ready_14200 (batch14195), gj_continuum_ready_14100
 * (batch14095), gj_continuum_ready_14000 (batch13995), and
 * gj_graph_milestone_14300 (batch14300). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14295_marker[] = "libcgj-batch14295";

/* Continuum readiness lamp for milestone 14300 continuum. */
#define B14295_CONTINUUM_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14295_ready(void)
{
	return B14295_CONTINUUM_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_ready_14300 - report the milestone-14300 continuum ready tag.
 *
 * Always returns 1 (continuum readiness complete). Link-time presence
 * of this symbol tags continuum readiness for the wave. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_continuum_ready_14300(void)
{
	(void)NULL;
	return b14295_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_ready_14300(void)
    __attribute__((alias("gj_continuum_ready_14300")));
