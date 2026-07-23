/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9005: continuum shell ready lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_shell_ready_9005(void);
 *     - Returns 1 (continuum shell surface ready for the post-9000
 *       continuum product deepen wave). Soft compile-time product tag;
 *       not a live shell process or path probe.
 *   uint32_t __gj_continuum_shell_ready_9005  (alias)
 *   __libcgj_batch9005_marker = "libcgj-batch9005"
 *
 * Exclusive continuum CREATE-ONLY (9001-9010: post-9000 continuum
 * product deepen). Unique gj_continuum_shell_ready_9005 surface only;
 * no multi-def. Distinct from gj_shell_green_9000 (batch8991),
 * gj_continuum_shell_ready_8205, gj_shell_ready_p (batch2596), and
 * gj_continuum_product_deepen_score_9004. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9005_marker[] = "libcgj-batch9005";

/* Continuum shell ready lamp. */
#define B9005_SHELL_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9005_ready(void)
{
	return B9005_SHELL_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_shell_ready_9005 - continuum shell ready lamp.
 *
 * Always returns 1. Soft pure-data product tag. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_shell_ready_9005(void)
{
	(void)NULL;
	return b9005_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_shell_ready_9005(void)
    __attribute__((alias("gj_continuum_shell_ready_9005")));
