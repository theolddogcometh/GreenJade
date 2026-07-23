/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9409: continuum dyn soft lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_dyn_soft_9409(void);
 *     - Returns 1 (continuum dyn soft surface ready for the post-9400
 *       continuum product deepen wave). Soft compile-time product tag;
 *       not a live dynamic linker probe.
 *   uint32_t __gj_continuum_dyn_soft_9409  (alias)
 *   __libcgj_batch9409_marker = "libcgj-batch9409"
 *
 * Exclusive continuum CREATE-ONLY (9401-9410: post-9400 continuum
 * product deepen). Unique gj_continuum_dyn_soft_9409 surface only;
 * no multi-def. Distinct from gj_dyn_soft_9400 (batch9397),
 * gj_continuum_dyn_soft_9309 (batch9309),
 * gj_continuum_dyn_soft_9209 (batch9209), and
 * gj_continuum_smoke_soft_9408. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9409_marker[] = "libcgj-batch9409";

/* Continuum dyn soft lamp. */
#define B9409_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9409_soft(void)
{
	return B9409_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_dyn_soft_9409 - continuum dyn soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_dyn_soft_9409(void)
{
	(void)NULL;
	return b9409_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_dyn_soft_9409(void)
    __attribute__((alias("gj_continuum_dyn_soft_9409")));
