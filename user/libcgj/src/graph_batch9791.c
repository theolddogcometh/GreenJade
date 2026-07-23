/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9791: shell product green extract (wave 9800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_9800(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       9800 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_9800  (alias)
 *   __libcgj_batch9791_marker = "libcgj-batch9791"
 *
 * Milestone 9800 exclusive continuum CREATE-ONLY (9791-9800). Unique
 * gj_shell_green_9800 surface only; no multi-def. Distinct from
 * gj_shell_green_9700 (batch9691), gj_shell_green_9600 (batch9591),
 * gj_shell_green_9500 (batch9491), and gj_libcgj_green_9800
 * (batch9792). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9791_marker[] = "libcgj-batch9791";

/* Product shell green lamp for wave 9800. */
#define B9791_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9791_green(void)
{
	return B9791_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_9800 - report product shell green lamp for wave 9800.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_9800(void)
{
	(void)NULL;
	return b9791_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_9800(void)
    __attribute__((alias("gj_shell_green_9800")));
