/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6095: shell product green extract (wave 6100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_6100(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       6100 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_6100  (alias)
 *   __libcgj_batch6095_marker = "libcgj-batch6095"
 *
 * Milestone 6100 exclusive continuum CREATE-ONLY (6091-6100). Unique
 * gj_shell_green_6100 surface only; no multi-def. Distinct from
 * gj_shell_green_6000 (batch5995), gj_shell_green_5900 (batch5895),
 * gj_shell_green_5800 (batch5795), and gj_libcgj_green_6100
 * (batch6096). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6095_marker[] = "libcgj-batch6095";

/* Shell product green lamp for wave 6100. */
#define B6095_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6095_green(void)
{
	return B6095_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_6100 - report shell product green lamp for wave 6100.
 *
 * Always returns 1 (shell green/ready). Soft pure-data only; not a
 * runtime probe of greenjade-shell. Does not call libc. No parent wires.
 */
uint32_t
gj_shell_green_6100(void)
{
	(void)NULL;
	return b6095_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_6100(void)
    __attribute__((alias("gj_shell_green_6100")));
