/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5795: shell product green extract (wave 5800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_5800(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       5800 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_5800  (alias)
 *   __libcgj_batch5795_marker = "libcgj-batch5795"
 *
 * Milestone 5800 exclusive continuum CREATE-ONLY (5791-5800). Unique
 * gj_shell_green_5800 surface only; no multi-def. Distinct from
 * gj_shell_green_5700 (batch5695), gj_shell_green_5650 (batch5645),
 * gj_shell_green_5600 (batch5595), and gj_libcgj_green_5800
 * (batch5796). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5795_marker[] = "libcgj-batch5795";

/* Product shell green lamp for wave 5800. */
#define B5795_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5795_green(void)
{
	return B5795_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_5800 - report product shell green lamp for wave 5800.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_5800(void)
{
	(void)NULL;
	return b5795_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_5800(void)
    __attribute__((alias("gj_shell_green_5800")));
