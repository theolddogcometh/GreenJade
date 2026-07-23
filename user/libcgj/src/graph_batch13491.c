/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13491: shell product green extract (wave 13500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_13500(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       13500 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_13500  (alias)
 *   __libcgj_batch13491_marker = "libcgj-batch13491"
 *
 * Milestone 13500 exclusive continuum CREATE-ONLY (13491-13500). Unique
 * gj_shell_green_13500 surface only; no multi-def. Distinct from
 * gj_shell_green_13400 (batch13391 continuum form), gj_shell_green_13300
 * (batch13291), gj_shell_green_13200 (batch13191), and gj_libcgj_green_13500
 * (batch13492). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13491_marker[] = "libcgj-batch13491";

/* Product shell green lamp for wave 13500. */
#define B13491_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13491_green(void)
{
	return B13491_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_13500 - report product shell green lamp for wave 13500.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_13500(void)
{
	(void)NULL;
	return b13491_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_13500(void)
    __attribute__((alias("gj_shell_green_13500")));
