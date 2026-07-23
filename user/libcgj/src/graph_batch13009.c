/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13009: continuum dyn soft lamp (wave 30).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_dyn_soft_13009(void);
 *     - Returns 1 (continuum dyn soft surface ready for continuum
 *       product deepen wave 30). Soft compile-time product tag; not a
 *       live dynamic linker probe.
 *   uint32_t __gj_continuum_dyn_soft_13009  (alias)
 *   __libcgj_batch13009_marker = "libcgj-batch13009"
 *
 * Exclusive continuum CREATE-ONLY (13001-13010: continuum product
 * deepen wave 30, post-13000). Unique gj_continuum_dyn_soft_13009
 * surface only; no multi-def. Distinct from gj_continuum_dyn_soft_12909,
 * gj_continuum_dyn_soft_12809, and gj_continuum_smoke_soft_13008.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13009_marker[] = "libcgj-batch13009";

/* Continuum dyn soft lamp. */
#define B13009_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13009_soft(void)
{
	return B13009_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_dyn_soft_13009 - continuum dyn soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_dyn_soft_13009(void)
{
	(void)NULL;
	return b13009_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_dyn_soft_13009(void)
    __attribute__((alias("gj_continuum_dyn_soft_13009")));
