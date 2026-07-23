/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8909: continuum dyn soft lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_dyn_soft_8909(void);
 *     - Returns 1 (continuum dyn soft surface ready for the post-8900
 *       continuum product deepen wave). Soft compile-time product tag;
 *       not a live dynamic linker probe.
 *   uint32_t __gj_continuum_dyn_soft_8909  (alias)
 *   __libcgj_batch8909_marker = "libcgj-batch8909"
 *
 * Exclusive continuum CREATE-ONLY (8901-8910: post-8900 continuum
 * product deepen). Unique gj_continuum_dyn_soft_8909 surface only;
 * no multi-def. Distinct from gj_dyn_soft_8900 (batch8897),
 * gj_continuum_dyn_soft_8809, and
 * gj_continuum_smoke_soft_8908. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8909_marker[] = "libcgj-batch8909";

/* Continuum dyn soft lamp. */
#define B8909_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8909_soft(void)
{
	return B8909_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_dyn_soft_8909 - continuum dyn soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_dyn_soft_8909(void)
{
	(void)NULL;
	return b8909_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_dyn_soft_8909(void)
    __attribute__((alias("gj_continuum_dyn_soft_8909")));
