/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13791: shell product green extract (wave 13800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_13800(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       13800 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_13800  (alias)
 *   __libcgj_batch13791_marker = "libcgj-batch13791"
 *
 * Milestone 13800 exclusive continuum CREATE-ONLY (13791-13800). Unique
 * gj_shell_green_13800 surface only; no multi-def. Distinct from
 * gj_shell_green_13700 (batch13691 continuum form), gj_shell_green_13600
 * (batch13591), gj_shell_green_13500 (batch13491), and gj_libcgj_green_13800
 * (batch13792). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13791_marker[] = "libcgj-batch13791";

/* Product shell green lamp for wave 13800. */
#define B13791_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13791_green(void)
{
	return B13791_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_13800 - report product shell green lamp for wave 13800.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_13800(void)
{
	(void)NULL;
	return b13791_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_13800(void)
    __attribute__((alias("gj_shell_green_13800")));
