/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13609: continuum dyn soft lamp (wave 36).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_dyn_soft_13609(void);
 *     - Returns 1 (continuum dyn soft surface ready for continuum
 *       product deepen wave 36). Soft compile-time product tag; not a
 *       live dynamic linker probe.
 *   uint32_t __gj_continuum_dyn_soft_13609  (alias)
 *   __libcgj_batch13609_marker = "libcgj-batch13609"
 *
 * Exclusive continuum CREATE-ONLY (13601-13610: continuum product
 * deepen wave 36, post-13600). Unique gj_continuum_dyn_soft_13609
 * surface only; no multi-def. Distinct from gj_continuum_dyn_soft_13509,
 * gj_continuum_dyn_soft_13409, and gj_continuum_smoke_soft_13608.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13609_marker[] = "libcgj-batch13609";

/* Continuum dyn soft lamp. */
#define B13609_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13609_soft(void)
{
	return B13609_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_dyn_soft_13609 - continuum dyn soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_dyn_soft_13609(void)
{
	(void)NULL;
	return b13609_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_dyn_soft_13609(void)
    __attribute__((alias("gj_continuum_dyn_soft_13609")));
