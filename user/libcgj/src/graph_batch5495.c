/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5495: shell product green extract (wave 5500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_5500(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       5500 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_5500  (alias)
 *   __libcgj_batch5495_marker = "libcgj-batch5495"
 *
 * Milestone 5500 exclusive continuum CREATE-ONLY (5491-5500). Unique
 * gj_shell_green_5500 surface only; no multi-def. Distinct from
 * gj_shell_green_5450 (batch5445), gj_shell_green_5400 (batch5395),
 * gj_shell_green_5350 (batch5345), and gj_libcgj_green_5500
 * (batch5496). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5495_marker[] = "libcgj-batch5495";

/* Product shell green lamp for wave 5500. */
#define B5495_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5495_green(void)
{
	return B5495_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_5500 - report product shell green lamp for wave 5500.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_5500(void)
{
	(void)NULL;
	return b5495_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_5500(void)
    __attribute__((alias("gj_shell_green_5500")));
