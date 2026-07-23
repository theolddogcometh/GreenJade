/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4795: shell product green extract (wave 4800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_4800(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       4800 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_4800  (alias)
 *   __libcgj_batch4795_marker = "libcgj-batch4795"
 *
 * Milestone 4800 exclusive continuum CREATE-ONLY (4791-4800). Unique
 * gj_shell_green_4800 surface only; no multi-def. Distinct from
 * gj_shell_green_4750 (batch4745), gj_shell_green_4700 (batch4695),
 * gj_shell_green_4650 (batch4645), and gj_libcgj_green_4800
 * (batch4796). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4795_marker[] = "libcgj-batch4795";

/* Product shell green lamp for wave 4800. */
#define B4795_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4795_green(void)
{
	return B4795_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_4800 - report product shell green lamp for wave 4800.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_4800(void)
{
	(void)NULL;
	return b4795_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_4800(void)
    __attribute__((alias("gj_shell_green_4800")));
