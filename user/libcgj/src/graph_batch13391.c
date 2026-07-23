/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13391: shell product green extract (wave 13400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_13400(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       13400 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_13400  (alias)
 *   __libcgj_batch13391_marker = "libcgj-batch13391"
 *
 * Milestone 13400 exclusive continuum CREATE-ONLY (13391-13400). Unique
 * gj_shell_green_13400 surface only; no multi-def. Distinct from
 * gj_shell_green_13300 (batch13291 continuum form), gj_shell_green_13200
 * (batch13191), gj_shell_green_13100 (batch13091), and gj_libcgj_green_13400
 * (batch13392). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13391_marker[] = "libcgj-batch13391";

/* Product shell green lamp for wave 13400. */
#define B13391_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13391_green(void)
{
	return B13391_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_13400 - report product shell green lamp for wave 13400.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_13400(void)
{
	(void)NULL;
	return b13391_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_13400(void)
    __attribute__((alias("gj_shell_green_13400")));
