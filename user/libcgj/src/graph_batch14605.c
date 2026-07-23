/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14605: continuum product deepen path tag
 * (wave 46).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_path_14605(void);
 *     - Returns 0 (continuum product-deepen path soft baseline for wave 46).
 *       Soft compile-time path tag; not a filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_path_14605  (alias)
 *   __libcgj_batch14605_marker = "libcgj-batch14605"
 *
 * Exclusive continuum CREATE-ONLY (14601-14610: continuum product deepen wave 46, post-14600). Unique surface only; no multi-def. Distinct from prior continuum_product_deepen / continuum_smoke_soft / continuum_dyn_soft symbols (1450x/1440x/…). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14605_marker[] = "libcgj-batch14605";

/* Continuum product deepen path soft baseline. */
#define B14605_CPD_PATH  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14605_path(void)
{
	return B14605_CPD_PATH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_path_14605 - soft continuum surface
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_path_14605(void)
{
	(void)NULL;
	return b14605_path();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_path_14605(void)
    __attribute__((alias("gj_continuum_product_deepen_path_14605")));
