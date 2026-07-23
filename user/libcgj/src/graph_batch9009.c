/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9009: continuum dyn soft lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_dyn_soft_9009(void);
 *     - Returns 1 (continuum dyn soft surface ready for the post-9000
 *       continuum product deepen wave). Soft compile-time product tag;
 *       not a live dynamic linker probe.
 *   uint32_t __gj_continuum_dyn_soft_9009  (alias)
 *   __libcgj_batch9009_marker = "libcgj-batch9009"
 *
 * Exclusive continuum CREATE-ONLY (9001-9010: post-9000 continuum
 * product deepen). Unique gj_continuum_dyn_soft_9009 surface only;
 * no multi-def. Distinct from gj_dyn_soft_9000 (batch8997),
 * gj_continuum_dyn_soft_8209, gj_continuum_smoke_soft_9008, and
 * gj_continuum_shell_ready_9005. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9009_marker[] = "libcgj-batch9009";

/* Continuum dyn soft lamp. */
#define B9009_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9009_soft(void)
{
	return B9009_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_dyn_soft_9009 - continuum dyn soft lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_dyn_soft_9009(void)
{
	(void)NULL;
	return b9009_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_dyn_soft_9009(void)
    __attribute__((alias("gj_continuum_dyn_soft_9009")));
