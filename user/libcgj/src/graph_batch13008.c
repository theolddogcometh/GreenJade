/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13008: continuum smoke soft lamp (wave 30).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_smoke_soft_13008(void);
 *     - Returns 1 (continuum smoke soft surface ready for continuum
 *       product deepen wave 30). Soft compile-time product tag; not a
 *       live smoke harness run.
 *   uint32_t __gj_continuum_smoke_soft_13008  (alias)
 *   __libcgj_batch13008_marker = "libcgj-batch13008"
 *
 * Exclusive continuum CREATE-ONLY (13001-13010: continuum product
 * deepen wave 30, post-13000). Unique gj_continuum_smoke_soft_13008
 * surface only; no multi-def. Distinct from gj_continuum_smoke_soft_12908,
 * gj_continuum_smoke_soft_12808, and
 * gj_continuum_product_deepen_lib_13007. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13008_marker[] = "libcgj-batch13008";

/* Continuum smoke soft lamp. */
#define B13008_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13008_soft(void)
{
	return B13008_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_smoke_soft_13008 - continuum smoke soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_smoke_soft_13008(void)
{
	(void)NULL;
	return b13008_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_smoke_soft_13008(void)
    __attribute__((alias("gj_continuum_smoke_soft_13008")));
