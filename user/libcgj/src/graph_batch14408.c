/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14408: continuum smoke soft lamp (wave 44).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_smoke_soft_14408(void);
 *     - Returns 1 (continuum smoke soft surface ready for continuum
 *       product deepen wave 44). Soft compile-time product tag; not a
 *       live smoke harness run.
 *   uint32_t __gj_continuum_smoke_soft_14408  (alias)
 *   __libcgj_batch14408_marker = "libcgj-batch14408"
 *
 * Exclusive continuum CREATE-ONLY (14401-14410: continuum product
 * deepen wave 44, post-14400). Unique gj_continuum_smoke_soft_14408
 * surface only; no multi-def. Distinct from prior continuum_product_deepen / continuum_smoke_soft /
 * continuum_dyn_soft symbols (1430x/1420x/1410x/…). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14408_marker[] = "libcgj-batch14408";

/* Continuum smoke soft ready lamp. */
#define B14408_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14408_soft(void)
{
	return B14408_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_smoke_soft_14408 - continuum smoke soft lamp
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_continuum_smoke_soft_14408(void)
{
	(void)NULL;
	return b14408_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_smoke_soft_14408(void)
    __attribute__((alias("gj_continuum_smoke_soft_14408")));
