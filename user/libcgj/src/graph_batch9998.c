/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9998: continuum smoke soft lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_smoke_soft_10000(void);
 *     - Returns 1 (continuum smoke soft surface ready for the MILESTONE
 *       10000 continuum gates wave). Soft compile-time product tag;
 *       not a live smoke harness run.
 *   uint32_t __gj_continuum_smoke_soft_10000  (alias)
 *   __libcgj_batch9998_marker = "libcgj-batch9998"
 *
 * Exclusive continuum CREATE-ONLY (9991-10000: MILESTONE 10000
 * continuum gates). Unique gj_continuum_smoke_soft_10000 surface only;
 * no multi-def. Distinct from gj_continuum_smoke_soft_8208,
 * gj_smoke_soft_9900 (batch9896), gj_smoke_soft_skip_p (batch2198), and
 * gj_continuum_bar3_stub_10000. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9998_marker[] = "libcgj-batch9998";

/* Continuum smoke soft lamp. */
#define B9998_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9998_soft(void)
{
	return B9998_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_smoke_soft_10000 - continuum smoke soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_smoke_soft_10000(void)
{
	(void)NULL;
	return b9998_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_smoke_soft_10000(void)
    __attribute__((alias("gj_continuum_smoke_soft_10000")));
