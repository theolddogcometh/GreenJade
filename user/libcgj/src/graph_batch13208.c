/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13208: continuum smoke soft lamp (wave 32).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_smoke_soft_13208(void);
 *     - Returns 1 (continuum smoke soft surface ready for continuum
 *       product deepen wave 32). Soft compile-time product tag; not a
 *       live smoke harness run.
 *   uint32_t __gj_continuum_smoke_soft_13208  (alias)
 *   __libcgj_batch13208_marker = "libcgj-batch13208"
 *
 * Exclusive continuum CREATE-ONLY (13201-13210: continuum product
 * deepen wave 32, post-13200). Unique gj_continuum_smoke_soft_13208
 * surface only; no multi-def. Distinct from gj_continuum_smoke_soft_13108,
 * gj_continuum_smoke_soft_13008, and
 * gj_continuum_product_deepen_lib_13207. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13208_marker[] = "libcgj-batch13208";

/* Continuum smoke soft lamp. */
#define B13208_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13208_soft(void)
{
	return B13208_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_smoke_soft_13208 - continuum smoke soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_smoke_soft_13208(void)
{
	(void)NULL;
	return b13208_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_smoke_soft_13208(void)
    __attribute__((alias("gj_continuum_smoke_soft_13208")));
