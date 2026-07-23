/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14088: continuum smoke soft lamp
 * (post-14080 deepen + soft smoke/dyn).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_smoke_soft_14088(void);
 *     - Returns 1 (continuum smoke soft surface ready for continuum
 *       product deepen + soft smoke/dyn). Soft compile-time product
 *       tag; not a live smoke harness run.
 *   uint32_t __gj_continuum_smoke_soft_14088  (alias)
 *   __libcgj_batch14088_marker = "libcgj-batch14088"
 *
 * Exclusive continuum CREATE-ONLY (14081-14090: continuum product
 * deepen + soft smoke/dyn, post-14080). Unique
 * gj_continuum_smoke_soft_14088 surface only; no multi-def. Distinct
 * from gj_continuum_smoke_soft_14008 (wave 40),
 * gj_continuum_smoke_soft_13908, and
 * gj_continuum_product_deepen_lib_14087. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14088_marker[] = "libcgj-batch14088";

/* Continuum smoke soft lamp. */
#define B14088_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14088_soft(void)
{
	return B14088_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_smoke_soft_14088 - continuum smoke soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_smoke_soft_14088(void)
{
	(void)NULL;
	return b14088_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_smoke_soft_14088(void)
    __attribute__((alias("gj_continuum_smoke_soft_14088")));
