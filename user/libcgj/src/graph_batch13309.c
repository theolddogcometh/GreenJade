/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13309: continuum dyn soft lamp (wave 33).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_dyn_soft_13309(void);
 *     - Returns 1 (continuum dyn soft surface ready for continuum
 *       product deepen wave 33). Soft compile-time product tag; not a
 *       live dynamic linker probe.
 *   uint32_t __gj_continuum_dyn_soft_13309  (alias)
 *   __libcgj_batch13309_marker = "libcgj-batch13309"
 *
 * Exclusive continuum CREATE-ONLY (13301-13310: continuum product
 * deepen wave 33, post-13300). Unique gj_continuum_dyn_soft_13309
 * surface only; no multi-def. Distinct from gj_continuum_dyn_soft_13209,
 * gj_continuum_dyn_soft_13109, and gj_continuum_smoke_soft_13308.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13309_marker[] = "libcgj-batch13309";

/* Continuum dyn soft lamp. */
#define B13309_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13309_soft(void)
{
	return B13309_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_dyn_soft_13309 - continuum dyn soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_dyn_soft_13309(void)
{
	(void)NULL;
	return b13309_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_dyn_soft_13309(void)
    __attribute__((alias("gj_continuum_dyn_soft_13309")));
