/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11408: continuum smoke soft lamp (wave 14).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_smoke_soft_11408(void);
 *     - Returns 1 (continuum smoke soft surface ready for continuum
 *       product deepen wave 14). Soft compile-time product tag; not a
 *       live smoke harness run.
 *   uint32_t __gj_continuum_smoke_soft_11408  (alias)
 *   __libcgj_batch11408_marker = "libcgj-batch11408"
 *
 * Exclusive continuum CREATE-ONLY (11401-11410: continuum product
 * deepen wave 14). Unique gj_continuum_smoke_soft_11408 surface only;
 * no multi-def. Distinct from gj_continuum_smoke_soft_11308,
 * gj_continuum_smoke_soft_11208, and
 * gj_continuum_product_deepen_lib_11407. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11408_marker[] = "libcgj-batch11408";

/* Continuum smoke soft lamp. */
#define B11408_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11408_soft(void)
{
	return B11408_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_smoke_soft_11408 - continuum smoke soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_smoke_soft_11408(void)
{
	(void)NULL;
	return b11408_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_smoke_soft_11408(void)
    __attribute__((alias("gj_continuum_smoke_soft_11408")));
