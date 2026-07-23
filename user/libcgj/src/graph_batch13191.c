/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13191: shell product green extract (wave 13200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_13200(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       13200 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_13200  (alias)
 *   __libcgj_batch13191_marker = "libcgj-batch13191"
 *
 * Milestone 13200 exclusive continuum CREATE-ONLY (13191-13200). Unique
 * gj_shell_green_13200 surface only; no multi-def. Distinct from
 * gj_shell_green_13100 (batch13091 continuum form), gj_shell_green_13000
 * (batch12991), gj_shell_green_12900 (batch12891), and gj_libcgj_green_13200
 * (batch13192). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13191_marker[] = "libcgj-batch13191";

/* Product shell green lamp for wave 13200. */
#define B13191_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13191_green(void)
{
	return B13191_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_13200 - report product shell green lamp for wave 13200.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_13200(void)
{
	(void)NULL;
	return b13191_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_13200(void)
    __attribute__((alias("gj_shell_green_13200")));
