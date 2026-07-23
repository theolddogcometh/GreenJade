/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11791: shell product green extract (wave 11800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_11800(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       11800 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_11800  (alias)
 *   __libcgj_batch11791_marker = "libcgj-batch11791"
 *
 * Milestone 11800 exclusive continuum CREATE-ONLY (11791-11800). Unique
 * gj_shell_green_11800 surface only; no multi-def. Distinct from
 * gj_shell_green_11700 (batch11691 continuum form), gj_shell_green_11600
 * (batch11591), gj_shell_green_11500 (batch11491), and gj_libcgj_green_11800
 * (batch11792). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11791_marker[] = "libcgj-batch11791";

/* Product shell green lamp for wave 11800. */
#define B11791_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11791_green(void)
{
	return B11791_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_11800 - report product shell green lamp for wave 11800.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_11800(void)
{
	(void)NULL;
	return b11791_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_11800(void)
    __attribute__((alias("gj_shell_green_11800")));
