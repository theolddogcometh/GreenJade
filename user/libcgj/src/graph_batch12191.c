/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12191: shell product green extract (wave 12200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_12200(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       12200 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_12200  (alias)
 *   __libcgj_batch12191_marker = "libcgj-batch12191"
 *
 * Milestone 12200 exclusive continuum CREATE-ONLY (12191-12200). Unique
 * gj_shell_green_12200 surface only; no multi-def. Distinct from
 * gj_shell_green_12100 (batch12091 continuum form), gj_shell_green_12000
 * (batch11991), gj_shell_green_11900 (batch11891), and gj_libcgj_green_12200
 * (batch12192). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12191_marker[] = "libcgj-batch12191";

/* Product shell green lamp for wave 12200. */
#define B12191_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12191_green(void)
{
	return B12191_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_12200 - report product shell green lamp for wave 12200.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_12200(void)
{
	(void)NULL;
	return b12191_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_12200(void)
    __attribute__((alias("gj_shell_green_12200")));
