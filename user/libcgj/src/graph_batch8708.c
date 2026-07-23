/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8708: continuum smoke soft gate (post-8700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_smoke_soft_8708(void);
 *     - Returns 1 (continuum smoke soft PASS lamp for the post-8700
 *       continuum product deepen wave). Soft compile-time product
 *       status tag.
 *   uint32_t __gj_continuum_smoke_soft_8708  (alias)
 *   __libcgj_batch8708_marker = "libcgj-batch8708"
 *
 * Exclusive continuum CREATE-ONLY (8701-8710: post-8700 continuum
 * product deepen). Unique gj_continuum_smoke_soft_8708 surface only;
 * no multi-def. Distinct from gj_smoke_soft_8700 (batch8696),
 * gj_continuum_dyn_soft_8709, and
 * gj_continuum_bar3_stub_8707. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8708_marker[] = "libcgj-batch8708";

/* Continuum smoke soft lamp for post-8700 deepen wave (always PASS). */
#define B8708_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8708_soft(void)
{
	return B8708_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_smoke_soft_8708 - continuum smoke soft lamp.
 *
 * Always returns 1 (soft PASS). Pair with gj_continuum_dyn_soft_8709
 * for the dyn soft-band counterpart. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_continuum_smoke_soft_8708(void)
{
	(void)NULL;
	return b8708_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_smoke_soft_8708(void)
    __attribute__((alias("gj_continuum_smoke_soft_8708")));
