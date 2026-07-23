/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13409: continuum dyn soft lamp (wave 34).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_dyn_soft_13409(void);
 *     - Returns 1 (continuum dyn soft surface ready for continuum
 *       product deepen wave 34). Soft compile-time product tag; not a
 *       live dynamic linker probe.
 *   uint32_t __gj_continuum_dyn_soft_13409  (alias)
 *   __libcgj_batch13409_marker = "libcgj-batch13409"
 *
 * Exclusive continuum CREATE-ONLY (13401-13410: continuum product
 * deepen wave 34, post-13400). Unique gj_continuum_dyn_soft_13409
 * surface only; no multi-def. Distinct from gj_continuum_dyn_soft_13309,
 * gj_continuum_dyn_soft_13209, and gj_continuum_smoke_soft_13408.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13409_marker[] = "libcgj-batch13409";

/* Continuum dyn soft lamp. */
#define B13409_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13409_soft(void)
{
	return B13409_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_dyn_soft_13409 - continuum dyn soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_dyn_soft_13409(void)
{
	(void)NULL;
	return b13409_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_dyn_soft_13409(void)
    __attribute__((alias("gj_continuum_dyn_soft_13409")));
