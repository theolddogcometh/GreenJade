/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5595: shell product green extract (wave 5600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_5600(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       5600 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_5600  (alias)
 *   __libcgj_batch5595_marker = "libcgj-batch5595"
 *
 * Milestone 5600 exclusive continuum CREATE-ONLY (5591-5600). Unique
 * gj_shell_green_5600 surface only; no multi-def. Distinct from
 * gj_shell_green_5550 (batch5545), gj_shell_green_5500 (batch5495),
 * gj_shell_green_5450 (batch5445), and gj_libcgj_green_5600
 * (batch5596). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5595_marker[] = "libcgj-batch5595";

/* Product shell green lamp for wave 5600. */
#define B5595_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5595_green(void)
{
	return B5595_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_5600 - report product shell green lamp for wave 5600.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_5600(void)
{
	(void)NULL;
	return b5595_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_5600(void)
    __attribute__((alias("gj_shell_green_5600")));
