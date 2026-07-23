/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11091: shell product green extract (wave 11100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_11100(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       11100 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_11100  (alias)
 *   __libcgj_batch11091_marker = "libcgj-batch11091"
 *
 * Milestone 11100 exclusive continuum CREATE-ONLY (11091-11100). Unique
 * gj_shell_green_11100 surface only; no multi-def. Distinct from
 * gj_shell_green_11000 (batch10991 continuum form), gj_shell_green_10900
 * (batch10891), gj_shell_green_10800 (batch10791), and gj_libcgj_green_11100
 * (batch11092). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11091_marker[] = "libcgj-batch11091";

/* Product shell green lamp for wave 11100. */
#define B11091_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11091_green(void)
{
	return B11091_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_11100 - report product shell green lamp for wave 11100.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_11100(void)
{
	(void)NULL;
	return b11091_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_11100(void)
    __attribute__((alias("gj_shell_green_11100")));
