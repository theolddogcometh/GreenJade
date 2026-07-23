/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12809: continuum dyn soft lamp (wave 28).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_dyn_soft_12809(void);
 *     - Returns 1 (continuum dyn soft surface ready for continuum
 *       product deepen wave 28). Soft compile-time product tag; not a
 *       live dynamic linker probe.
 *   uint32_t __gj_continuum_dyn_soft_12809  (alias)
 *   __libcgj_batch12809_marker = "libcgj-batch12809"
 *
 * Exclusive continuum CREATE-ONLY (12801-12810: continuum product
 * deepen wave 28, post-12800). Unique gj_continuum_dyn_soft_12809
 * surface only; no multi-def. Distinct from gj_continuum_dyn_soft_12709,
 * gj_continuum_dyn_soft_12609, and gj_continuum_smoke_soft_12808.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12809_marker[] = "libcgj-batch12809";

/* Continuum dyn soft lamp. */
#define B12809_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12809_soft(void)
{
	return B12809_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_dyn_soft_12809 - continuum dyn soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_dyn_soft_12809(void)
{
	(void)NULL;
	return b12809_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_dyn_soft_12809(void)
    __attribute__((alias("gj_continuum_dyn_soft_12809")));
