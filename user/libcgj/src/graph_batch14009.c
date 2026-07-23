/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14009: continuum dyn soft lamp (wave 40).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_dyn_soft_14009(void);
 *     - Returns 1 (continuum dyn soft surface ready for continuum
 *       product deepen wave 40). Soft compile-time product tag; not a
 *       live dynamic linker probe.
 *   uint32_t __gj_continuum_dyn_soft_14009  (alias)
 *   __libcgj_batch14009_marker = "libcgj-batch14009"
 *
 * Exclusive continuum CREATE-ONLY (14001-14010: continuum product
 * deepen wave 40, post-14000). Unique gj_continuum_dyn_soft_14009
 * surface only; no multi-def. Distinct from gj_continuum_dyn_soft_13009,
 * gj_continuum_dyn_soft_12909, and gj_continuum_smoke_soft_14008.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14009_marker[] = "libcgj-batch14009";

/* Continuum dyn soft lamp. */
#define B14009_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14009_soft(void)
{
	return B14009_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_dyn_soft_14009 - continuum dyn soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_dyn_soft_14009(void)
{
	(void)NULL;
	return b14009_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_dyn_soft_14009(void)
    __attribute__((alias("gj_continuum_dyn_soft_14009")));
