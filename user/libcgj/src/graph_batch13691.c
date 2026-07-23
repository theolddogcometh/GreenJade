/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13691: shell product green extract (wave 13700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_13700(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       13700 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_13700  (alias)
 *   __libcgj_batch13691_marker = "libcgj-batch13691"
 *
 * Milestone 13700 exclusive continuum CREATE-ONLY (13691-13700). Unique
 * gj_shell_green_13700 surface only; no multi-def. Distinct from
 * gj_shell_green_13600 (batch13591 continuum form), gj_shell_green_13500
 * (batch13491), gj_shell_green_13400 (batch13391), and gj_libcgj_green_13700
 * (batch13692). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13691_marker[] = "libcgj-batch13691";

/* Product shell green lamp for wave 13700. */
#define B13691_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13691_green(void)
{
	return B13691_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_13700 - report product shell green lamp for wave 13700.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_13700(void)
{
	(void)NULL;
	return b13691_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_13700(void)
    __attribute__((alias("gj_shell_green_13700")));
