/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch30295: continuum readiness gate (wave 30300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_ready_30300(void);
 *     - Returns 1 (continuum readiness lamp for the milestone 30300
 *       continuum). Soft compile-time product status tag for exclusive-
 *       wave continuum readiness (CREATE-ONLY marker band complete).
 *   uint32_t __gj_continuum_ready_30300  (alias)
 *   __libcgj_batch30295_marker = "libcgj-batch30295"
 *
 * Milestone 30300 exclusive continuum CREATE-ONLY (30291-30300). Unique
 * gj_continuum_ready_30300 surface only; no multi-def. Distinct from
 * gj_continuum_ready_30200 / gj_continuum_ready_30100 /
 * gj_continuum_ready_30000 and sibling 30300 milestone symbols
 * (gj_graph_milestone_30300, smoke_soft, dyn_soft, …).
 * No parent wires. No __int128.
 *
 * CGJ soft marker band (30291–30300): host soft probes
 * (cgj_soft_milestone_30300.*) may dlsym / direct-link this lamp.
 * Soft rule: missing symbol → skip; present but wrong value → hard fail.
 * Soft continuum ready ≠ bar3 ready (bar3 stays 0 at batch30293).
 * Soft gates only: does not wire Makefile / libc.map / smoke harnesses.
 * makefile_max honesty is parent-side (scan → makefile_max=30300 when wired).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

/* CGJ TU marker: greppable continuum identity string for batch 30295. */
const char __libcgj_batch30295_marker[] = "libcgj-batch30295";

/* Continuum readiness lamp for wave 30300 (soft continuum complete). */
#define B30295_CONTINUUM_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b30295_ready(void)
{
	return B30295_CONTINUUM_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_ready_30300 - report milestone-30300 continuum ready tag.
 *
 * Always returns 1u (continuum readiness complete for soft graph growth).
 * Link-time presence tags continuum readiness for the wave. Soft pure-
 * data only; does not call libc. No parent wires.
 * CGJ soft KAT expectation: 1.
 */
uint32_t
gj_continuum_ready_30300(void)
{
	(void)NULL;
	return b30295_ready();
}

/* ---- underscored alias ------------------------------------------------- */

/* CGJ alias: underscored form for map / weak-link compatibility. */
uint32_t __gj_continuum_ready_30300(void)
    __attribute__((alias("gj_continuum_ready_30300")));
