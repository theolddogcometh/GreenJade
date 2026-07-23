/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9908: continuum smoke soft lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_smoke_soft_9908(void);
 *     - Returns 1 (continuum smoke soft surface ready for the post-9900
 *       continuum product deepen wave). Soft compile-time product tag;
 *       not a live smoke harness run.
 *   uint32_t __gj_continuum_smoke_soft_9908  (alias)
 *   __libcgj_batch9908_marker = "libcgj-batch9908"
 *
 * Exclusive continuum CREATE-ONLY (9901-9910: post-9900 continuum
 * product deepen toward 10000). Unique gj_continuum_smoke_soft_9908
 * surface only; no multi-def. Distinct from gj_smoke_soft_9900
 * (batch9896), gj_continuum_smoke_soft_9808, and
 * gj_continuum_bar3_stub_9907. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9908_marker[] = "libcgj-batch9908";

/* Continuum smoke soft lamp. */
#define B9908_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9908_soft(void)
{
	return B9908_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_smoke_soft_9908 - continuum smoke soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_smoke_soft_9908(void)
{
	(void)NULL;
	return b9908_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_smoke_soft_9908(void)
    __attribute__((alias("gj_continuum_smoke_soft_9908")));
