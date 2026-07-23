/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6795: shell product green extract (wave 6800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_6800(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       6800 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_6800  (alias)
 *   __libcgj_batch6795_marker = "libcgj-batch6795"
 *
 * Milestone 6800 exclusive continuum CREATE-ONLY (6791-6800). Unique
 * gj_shell_green_6800 surface only; no multi-def. Distinct from
 * gj_shell_green_6700 (batch6695), gj_shell_green_6600 (batch6595),
 * gj_shell_green_6500 (batch6495), and gj_libcgj_green_6800
 * (batch6796). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6795_marker[] = "libcgj-batch6795";

/* Product shell green lamp for wave 6800. */
#define B6795_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6795_green(void)
{
	return B6795_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_6800 - report product shell green lamp for wave 6800.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_6800(void)
{
	(void)NULL;
	return b6795_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_6800(void)
    __attribute__((alias("gj_shell_green_6800")));
