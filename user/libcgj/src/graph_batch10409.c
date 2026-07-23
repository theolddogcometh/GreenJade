/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10409: continuum dyn soft lamp (wave 4).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_dyn_soft_10409(void);
 *     - Returns 1 (continuum dyn soft surface ready for continuum
 *       product deepen wave 4). Soft compile-time product tag; not a
 *       live dynamic linker probe.
 *   uint32_t __gj_continuum_dyn_soft_10409  (alias)
 *   __libcgj_batch10409_marker = "libcgj-batch10409"
 *
 * Exclusive continuum CREATE-ONLY (10401-10410: continuum product
 * deepen wave 4). Unique gj_continuum_dyn_soft_10409 surface only;
 * no multi-def. Distinct from gj_continuum_dyn_soft_10309,
 * gj_dyn_soft_10400 (batch10397), and gj_continuum_smoke_soft_10408.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10409_marker[] = "libcgj-batch10409";

/* Continuum dyn soft lamp. */
#define B10409_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10409_soft(void)
{
	return B10409_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_dyn_soft_10409 - continuum dyn soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_dyn_soft_10409(void)
{
	(void)NULL;
	return b10409_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_dyn_soft_10409(void)
    __attribute__((alias("gj_continuum_dyn_soft_10409")));
