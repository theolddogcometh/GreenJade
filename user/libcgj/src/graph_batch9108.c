/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9108: continuum smoke soft lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_smoke_soft_9108(void);
 *     - Returns 1 (continuum smoke soft surface ready for the post-9100
 *       continuum product deepen wave). Soft compile-time product tag;
 *       not a live smoke harness run.
 *   uint32_t __gj_continuum_smoke_soft_9108  (alias)
 *   __libcgj_batch9108_marker = "libcgj-batch9108"
 *
 * Exclusive continuum CREATE-ONLY (9101-9110: post-9100 continuum
 * product deepen). Unique gj_continuum_smoke_soft_9108 surface only;
 * no multi-def. Distinct from gj_smoke_soft_9100 (batch9096),
 * gj_continuum_smoke_soft_9008 (batch9008),
 * gj_smoke_soft_skip_p (batch2198), and
 * gj_continuum_bar3_stub_9107. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9108_marker[] = "libcgj-batch9108";

/* Continuum smoke soft lamp. */
#define B9108_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9108_soft(void)
{
	return B9108_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_smoke_soft_9108 - continuum smoke soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_smoke_soft_9108(void)
{
	(void)NULL;
	return b9108_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_smoke_soft_9108(void)
    __attribute__((alias("gj_continuum_smoke_soft_9108")));
