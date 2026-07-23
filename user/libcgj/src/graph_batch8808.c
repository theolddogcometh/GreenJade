/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8808: continuum smoke soft gate (post-8800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_smoke_soft_8808(void);
 *     - Returns 1 (continuum smoke soft PASS lamp for the post-8800
 *       continuum product deepen wave). Soft compile-time product
 *       status tag.
 *   uint32_t __gj_continuum_smoke_soft_8808  (alias)
 *   __libcgj_batch8808_marker = "libcgj-batch8808"
 *
 * Exclusive continuum CREATE-ONLY (8801-8810: post-8800 continuum
 * product deepen). Unique gj_continuum_smoke_soft_8808 surface only;
 * no multi-def. Distinct from gj_continuum_smoke_soft_8708,
 * gj_smoke_soft_8800 (batch8796), and
 * gj_continuum_dyn_soft_8809. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8808_marker[] = "libcgj-batch8808";

/* Continuum smoke soft lamp for post-8800 deepen wave (always PASS). */
#define B8808_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8808_soft(void)
{
	return B8808_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_smoke_soft_8808 - continuum smoke soft lamp.
 *
 * Always returns 1 (soft PASS). Pair with gj_continuum_dyn_soft_8809
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_continuum_smoke_soft_8808(void)
{
	(void)NULL;
	return b8808_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_smoke_soft_8808(void)
    __attribute__((alias("gj_continuum_smoke_soft_8808")));
