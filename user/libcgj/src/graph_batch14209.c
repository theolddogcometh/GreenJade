/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14209: continuum dyn soft lamp (wave 42).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_dyn_soft_14209(void);
 *     - Returns 1 (continuum dyn soft surface ready for continuum
 *       product deepen wave 42). Soft compile-time product tag; not a
 *       live dynamic linker probe.
 *   uint32_t __gj_continuum_dyn_soft_14209  (alias)
 *   __libcgj_batch14209_marker = "libcgj-batch14209"
 *
 * Exclusive continuum CREATE-ONLY (14201-14210: continuum product
 * deepen wave 42, post-14200). Unique gj_continuum_dyn_soft_14209
 * surface only; no multi-def. Distinct from gj_continuum_dyn_soft_14109,
 * gj_continuum_dyn_soft_14009, and gj_continuum_smoke_soft_14208.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14209_marker[] = "libcgj-batch14209";

/* Continuum dyn soft lamp. */
#define B14209_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14209_soft(void)
{
	return B14209_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_dyn_soft_14209 - continuum dyn soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_dyn_soft_14209(void)
{
	(void)NULL;
	return b14209_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_dyn_soft_14209(void)
    __attribute__((alias("gj_continuum_dyn_soft_14209")));
