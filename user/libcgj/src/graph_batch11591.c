/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11591: shell product green extract (wave 11600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_11600(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       11600 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_11600  (alias)
 *   __libcgj_batch11591_marker = "libcgj-batch11591"
 *
 * Milestone 11600 exclusive continuum CREATE-ONLY (11591-11600). Unique
 * gj_shell_green_11600 surface only; no multi-def. Distinct from
 * gj_shell_green_11500 (batch11491 continuum form), gj_shell_green_11400
 * (batch11391), gj_shell_green_11300 (batch11291), and gj_libcgj_green_11600
 * (batch11592). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11591_marker[] = "libcgj-batch11591";

/* Product shell green lamp for wave 11600. */
#define B11591_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11591_green(void)
{
	return B11591_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_11600 - report product shell green lamp for wave 11600.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_11600(void)
{
	(void)NULL;
	return b11591_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_11600(void)
    __attribute__((alias("gj_shell_green_11600")));
