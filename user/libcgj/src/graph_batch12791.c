/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12791: shell product green extract (wave 12800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_12800(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       12800 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_12800  (alias)
 *   __libcgj_batch12791_marker = "libcgj-batch12791"
 *
 * Milestone 12800 exclusive continuum CREATE-ONLY (12791-12800). Unique
 * gj_shell_green_12800 surface only; no multi-def. Distinct from
 * gj_shell_green_12700 (batch12691 continuum form), gj_shell_green_12600
 * (batch12591), gj_shell_green_12500 (batch12491), and gj_libcgj_green_12800
 * (batch12792). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12791_marker[] = "libcgj-batch12791";

/* Product shell green lamp for wave 12800. */
#define B12791_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12791_green(void)
{
	return B12791_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_12800 - report product shell green lamp for wave 12800.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_12800(void)
{
	(void)NULL;
	return b12791_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_12800(void)
    __attribute__((alias("gj_shell_green_12800")));
