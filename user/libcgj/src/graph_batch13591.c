/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13591: shell product green extract (wave 13600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_13600(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       13600 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_13600  (alias)
 *   __libcgj_batch13591_marker = "libcgj-batch13591"
 *
 * Milestone 13600 exclusive continuum CREATE-ONLY (13591-13600). Unique
 * gj_shell_green_13600 surface only; no multi-def. Distinct from
 * gj_shell_green_13500 (batch13491 continuum form), gj_shell_green_13400
 * (batch13391), gj_shell_green_13300 (batch13291), and gj_libcgj_green_13600
 * (batch13592). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13591_marker[] = "libcgj-batch13591";

/* Product shell green lamp for wave 13600. */
#define B13591_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13591_green(void)
{
	return B13591_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_13600 - report product shell green lamp for wave 13600.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_13600(void)
{
	(void)NULL;
	return b13591_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_13600(void)
    __attribute__((alias("gj_shell_green_13600")));
