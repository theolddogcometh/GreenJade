/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8305: continuum shell ready lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_shell_ready_8305(void);
 *     - Returns 1 (continuum shell surface ready for the post-8300
 *       continuum product deepen wave). Soft compile-time product tag;
 *       not a live shell process or path probe.
 *   uint32_t __gj_continuum_shell_ready_8305  (alias)
 *   __libcgj_batch8305_marker = "libcgj-batch8305"
 *
 * Exclusive continuum CREATE-ONLY (8301-8310: post-8300 continuum
 * product deepen). Unique gj_continuum_shell_ready_8305 surface only;
 * no multi-def. Distinct from gj_continuum_shell_ready_8205,
 * gj_shell_ready_p (batch2596), and
 * gj_continuum_product_deepen_score_8304. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8305_marker[] = "libcgj-batch8305";

/* Continuum shell ready lamp. */
#define B8305_SHELL_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8305_ready(void)
{
	return B8305_SHELL_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_shell_ready_8305 - continuum shell ready lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_shell_ready_8305(void)
{
	(void)NULL;
	return b8305_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_shell_ready_8305(void)
    __attribute__((alias("gj_continuum_shell_ready_8305")));
