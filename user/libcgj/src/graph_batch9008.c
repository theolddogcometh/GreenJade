/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9008: continuum smoke soft lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_smoke_soft_9008(void);
 *     - Returns 1 (continuum smoke soft surface ready for the post-9000
 *       continuum product deepen wave). Soft compile-time product tag;
 *       not a live smoke harness run.
 *   uint32_t __gj_continuum_smoke_soft_9008  (alias)
 *   __libcgj_batch9008_marker = "libcgj-batch9008"
 *
 * Exclusive continuum CREATE-ONLY (9001-9010: post-9000 continuum
 * product deepen). Unique gj_continuum_smoke_soft_9008 surface only;
 * no multi-def. Distinct from gj_smoke_soft_9000 (batch8996),
 * gj_continuum_smoke_soft_8208, gj_smoke_soft_skip_p (batch2198), and
 * gj_continuum_bar3_stub_9007. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9008_marker[] = "libcgj-batch9008";

/* Continuum smoke soft lamp. */
#define B9008_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9008_soft(void)
{
	return B9008_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_smoke_soft_9008 - continuum smoke soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_smoke_soft_9008(void)
{
	(void)NULL;
	return b9008_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_smoke_soft_9008(void)
    __attribute__((alias("gj_continuum_smoke_soft_9008")));
