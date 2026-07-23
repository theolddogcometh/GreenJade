/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14308: continuum smoke soft lamp (wave 43).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_smoke_soft_14308(void);
 *     - Returns 1 (continuum smoke soft surface ready for continuum
 *       product deepen wave 43). Soft compile-time product tag; not a
 *       live smoke harness run.
 *   uint32_t __gj_continuum_smoke_soft_14308  (alias)
 *   __libcgj_batch14308_marker = "libcgj-batch14308"
 *
 * Exclusive continuum CREATE-ONLY (14301-14310: continuum product
 * deepen wave 43, post-14300). Unique gj_continuum_smoke_soft_14308
 * surface only; no multi-def. Distinct from gj_continuum_smoke_soft_14008,
 * gj_continuum_smoke_soft_13008, and
 * gj_continuum_product_deepen_lib_14307. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14308_marker[] = "libcgj-batch14308";

/* Continuum smoke soft lamp. */
#define B14308_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14308_soft(void)
{
	return B14308_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_smoke_soft_14308 - continuum smoke soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_smoke_soft_14308(void)
{
	(void)NULL;
	return b14308_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_smoke_soft_14308(void)
    __attribute__((alias("gj_continuum_smoke_soft_14308")));
