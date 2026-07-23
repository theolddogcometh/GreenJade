/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7495: shell product green extract (wave 7500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_7500(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       7500 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_7500  (alias)
 *   __libcgj_batch7495_marker = "libcgj-batch7495"
 *
 * Milestone 7500 exclusive continuum CREATE-ONLY (7491-7500). Unique
 * gj_shell_green_7500 surface only; no multi-def. Distinct from
 * gj_shell_green_7400 (batch7395), gj_shell_green_7300 (batch7295),
 * gj_shell_green_7200 (batch7195), and gj_libcgj_green_7500
 * (batch7496). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7495_marker[] = "libcgj-batch7495";

/* Product shell green lamp for wave 7500. */
#define B7495_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7495_green(void)
{
	return B7495_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_7500 - report product shell green lamp for wave 7500.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_7500(void)
{
	(void)NULL;
	return b7495_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_7500(void)
    __attribute__((alias("gj_shell_green_7500")));
