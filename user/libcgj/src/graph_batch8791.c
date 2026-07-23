/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8791: shell product green extract (wave 8800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_8800(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       8800 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_8800  (alias)
 *   __libcgj_batch8791_marker = "libcgj-batch8791"
 *
 * Milestone 8800 exclusive continuum CREATE-ONLY (8791-8800). Unique
 * gj_shell_green_8800 surface only; no multi-def. Distinct from
 * gj_shell_green_8700 (batch8691), gj_shell_green_8600 (batch8591),
 * gj_shell_green_8500 (batch8491), and gj_libcgj_green_8800
 * (batch8792). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8791_marker[] = "libcgj-batch8791";

/* Product shell green lamp for wave 8800. */
#define B8791_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8791_green(void)
{
	return B8791_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_8800 - report product shell green lamp for wave 8800.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_8800(void)
{
	(void)NULL;
	return b8791_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_8800(void)
    __attribute__((alias("gj_shell_green_8800")));
