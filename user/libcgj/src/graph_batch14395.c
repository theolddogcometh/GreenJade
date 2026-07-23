/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14395: milestone 14400 continuum ready tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_ready_14400(void);
 *     - Returns 1 (continuum readiness complete for the milestone 14400
 *       continuum). Soft compile-time product tag for exclusive-wave
 *       continuum readiness.
 *   uint32_t __gj_continuum_ready_14400  (alias)
 *   __libcgj_batch14395_marker = "libcgj-batch14395"
 *
 * Milestone 14400 exclusive continuum CREATE-ONLY (14391-14400). Unique
 * gj_continuum_ready_14400 surface only; no multi-def. Distinct from
 * gj_continuum_ready_14000 (batch13995), gj_continuum_ready_13000
 * (batch12995), gj_continuum_ready_12900 (batch12895), and
 * gj_graph_milestone_14400 (batch14400). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14395_marker[] = "libcgj-batch14395";

/* Continuum readiness lamp for milestone 14400 continuum. */
#define B14395_CONTINUUM_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14395_ready(void)
{
	return B14395_CONTINUUM_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_ready_14400 - report the milestone-14400 continuum ready tag.
 *
 * Always returns 1 (continuum readiness complete). Link-time presence
 * of this symbol tags continuum readiness for the wave. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_continuum_ready_14400(void)
{
	(void)NULL;
	return b14395_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_ready_14400(void)
    __attribute__((alias("gj_continuum_ready_14400")));
