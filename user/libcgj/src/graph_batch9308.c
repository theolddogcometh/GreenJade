/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9308: continuum smoke soft lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_smoke_soft_9308(void);
 *     - Returns 1 (continuum smoke soft surface ready for the post-9300
 *       continuum product deepen wave). Soft compile-time product tag;
 *       not a live smoke harness run.
 *   uint32_t __gj_continuum_smoke_soft_9308  (alias)
 *   __libcgj_batch9308_marker = "libcgj-batch9308"
 *
 * Exclusive continuum CREATE-ONLY (9301-9310: post-9300 continuum
 * product deepen). Unique gj_continuum_smoke_soft_9308 surface only;
 * no multi-def. Distinct from gj_smoke_soft_9300 (batch9296),
 * gj_continuum_smoke_soft_9208 (batch9208),
 * gj_smoke_soft_skip_p (batch2198), and
 * gj_continuum_bar3_stub_9307. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9308_marker[] = "libcgj-batch9308";

/* Continuum smoke soft lamp. */
#define B9308_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9308_soft(void)
{
	return B9308_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_smoke_soft_9308 - continuum smoke soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_smoke_soft_9308(void)
{
	(void)NULL;
	return b9308_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_smoke_soft_9308(void)
    __attribute__((alias("gj_continuum_smoke_soft_9308")));
