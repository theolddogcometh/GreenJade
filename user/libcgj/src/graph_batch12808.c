/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12808: continuum smoke soft lamp (wave 28).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_smoke_soft_12808(void);
 *     - Returns 1 (continuum smoke soft surface ready for continuum
 *       product deepen wave 28). Soft compile-time product tag; not a
 *       live smoke harness run.
 *   uint32_t __gj_continuum_smoke_soft_12808  (alias)
 *   __libcgj_batch12808_marker = "libcgj-batch12808"
 *
 * Exclusive continuum CREATE-ONLY (12801-12810: continuum product
 * deepen wave 28, post-12800). Unique gj_continuum_smoke_soft_12808
 * surface only; no multi-def. Distinct from gj_continuum_smoke_soft_12708,
 * gj_continuum_smoke_soft_12608, and
 * gj_continuum_product_deepen_lib_12807. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12808_marker[] = "libcgj-batch12808";

/* Continuum smoke soft lamp. */
#define B12808_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12808_soft(void)
{
	return B12808_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_smoke_soft_12808 - continuum smoke soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_smoke_soft_12808(void)
{
	(void)NULL;
	return b12808_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_smoke_soft_12808(void)
    __attribute__((alias("gj_continuum_smoke_soft_12808")));
