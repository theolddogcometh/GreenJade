/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13908: continuum smoke soft lamp (wave 39).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_smoke_soft_13908(void);
 *     - Returns 1 (continuum smoke soft surface ready for continuum
 *       product deepen wave 39). Soft compile-time product tag; not a
 *       live smoke harness run.
 *   uint32_t __gj_continuum_smoke_soft_13908  (alias)
 *   __libcgj_batch13908_marker = "libcgj-batch13908"
 *
 * Exclusive continuum CREATE-ONLY (13901-13910: continuum product
 * deepen wave 39, post-13900). Unique gj_continuum_smoke_soft_13908
 * surface only; no multi-def. Distinct from gj_continuum_smoke_soft_13708,
 * gj_continuum_smoke_soft_13608, and
 * gj_continuum_product_deepen_lib_13907. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13908_marker[] = "libcgj-batch13908";

/* Continuum smoke soft lamp. */
#define B13908_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13908_soft(void)
{
	return B13908_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_smoke_soft_13908 - continuum smoke soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_smoke_soft_13908(void)
{
	(void)NULL;
	return b13908_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_smoke_soft_13908(void)
    __attribute__((alias("gj_continuum_smoke_soft_13908")));
