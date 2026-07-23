/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7795: shell product green extract (wave 7800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_7800(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       7800 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_7800  (alias)
 *   __libcgj_batch7795_marker = "libcgj-batch7795"
 *
 * Milestone 7800 exclusive continuum CREATE-ONLY (7791-7800). Unique
 * gj_shell_green_7800 surface only; no multi-def. Distinct from
 * gj_shell_green_7700 (batch7695), gj_shell_green_7600 (batch7595),
 * gj_shell_green_7500 (batch7495), and gj_libcgj_green_7800
 * (batch7796). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7795_marker[] = "libcgj-batch7795";

/* Product shell green lamp for wave 7800. */
#define B7795_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7795_green(void)
{
	return B7795_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_7800 - report product shell green lamp for wave 7800.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_7800(void)
{
	(void)NULL;
	return b7795_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_7800(void)
    __attribute__((alias("gj_shell_green_7800")));
