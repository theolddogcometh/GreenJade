/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12408: continuum smoke soft lamp (wave 24).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_smoke_soft_12408(void);
 *     - Returns 1 (continuum smoke soft surface ready for continuum
 *       product deepen wave 24). Soft compile-time product tag; not a
 *       live smoke harness run.
 *   uint32_t __gj_continuum_smoke_soft_12408  (alias)
 *   __libcgj_batch12408_marker = "libcgj-batch12408"
 *
 * Exclusive continuum CREATE-ONLY (12401-12410: continuum product
 * deepen wave 24, post-12400). Unique gj_continuum_smoke_soft_12408
 * surface only; no multi-def. Distinct from gj_continuum_smoke_soft_12308,
 * gj_continuum_smoke_soft_12208, and
 * gj_continuum_product_deepen_lib_12407. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12408_marker[] = "libcgj-batch12408";

/* Continuum smoke soft lamp. */
#define B12408_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12408_soft(void)
{
	return B12408_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_smoke_soft_12408 - continuum smoke soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_smoke_soft_12408(void)
{
	(void)NULL;
	return b12408_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_smoke_soft_12408(void)
    __attribute__((alias("gj_continuum_smoke_soft_12408")));
