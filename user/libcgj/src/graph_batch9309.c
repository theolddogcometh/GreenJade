/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9309: continuum dyn soft lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_dyn_soft_9309(void);
 *     - Returns 1 (continuum dyn soft surface ready for the post-9300
 *       continuum product deepen wave). Soft compile-time product tag;
 *       not a live dynamic linker probe.
 *   uint32_t __gj_continuum_dyn_soft_9309  (alias)
 *   __libcgj_batch9309_marker = "libcgj-batch9309"
 *
 * Exclusive continuum CREATE-ONLY (9301-9310: post-9300 continuum
 * product deepen). Unique gj_continuum_dyn_soft_9309 surface only;
 * no multi-def. Distinct from gj_dyn_soft_9300 (batch9297),
 * gj_continuum_dyn_soft_9209 (batch9209),
 * gj_continuum_smoke_soft_9308, and
 * gj_continuum_shell_ready_9305. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9309_marker[] = "libcgj-batch9309";

/* Continuum dyn soft lamp. */
#define B9309_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9309_soft(void)
{
	return B9309_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_dyn_soft_9309 - continuum dyn soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_dyn_soft_9309(void)
{
	(void)NULL;
	return b9309_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_dyn_soft_9309(void)
    __attribute__((alias("gj_continuum_dyn_soft_9309")));
