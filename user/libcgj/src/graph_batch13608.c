/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13608: continuum smoke soft lamp (wave 36).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_smoke_soft_13608(void);
 *     - Returns 1 (continuum smoke soft surface ready for continuum
 *       product deepen wave 36). Soft compile-time product tag; not a
 *       live smoke harness run.
 *   uint32_t __gj_continuum_smoke_soft_13608  (alias)
 *   __libcgj_batch13608_marker = "libcgj-batch13608"
 *
 * Exclusive continuum CREATE-ONLY (13601-13610: continuum product
 * deepen wave 36, post-13600). Unique gj_continuum_smoke_soft_13608
 * surface only; no multi-def. Distinct from gj_continuum_smoke_soft_13508,
 * gj_continuum_smoke_soft_13408, and
 * gj_continuum_product_deepen_lib_13607. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13608_marker[] = "libcgj-batch13608";

/* Continuum smoke soft lamp. */
#define B13608_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13608_soft(void)
{
	return B13608_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_smoke_soft_13608 - continuum smoke soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_smoke_soft_13608(void)
{
	(void)NULL;
	return b13608_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_smoke_soft_13608(void)
    __attribute__((alias("gj_continuum_smoke_soft_13608")));
