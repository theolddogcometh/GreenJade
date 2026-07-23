/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10791: shell product green extract (wave 10800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_10800(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       10800 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_10800  (alias)
 *   __libcgj_batch10791_marker = "libcgj-batch10791"
 *
 * Milestone 10800 exclusive continuum CREATE-ONLY (10791-10800). Unique
 * gj_shell_green_10800 surface only; no multi-def. Distinct from
 * gj_shell_green_10700 (batch10691 continuum form), gj_shell_green_10600
 * (batch10591), gj_shell_green_10500 (batch10491), and gj_libcgj_green_10800
 * (batch10792). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10791_marker[] = "libcgj-batch10791";

/* Product shell green lamp for wave 10800. */
#define B10791_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10791_green(void)
{
	return B10791_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_10800 - report product shell green lamp for wave 10800.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_10800(void)
{
	(void)NULL;
	return b10791_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_10800(void)
    __attribute__((alias("gj_shell_green_10800")));
