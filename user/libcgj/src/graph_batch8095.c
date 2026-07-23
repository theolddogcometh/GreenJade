/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8095: shell product green extract (wave 8100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_8100(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       8100 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_8100  (alias)
 *   __libcgj_batch8095_marker = "libcgj-batch8095"
 *
 * Milestone 8100 exclusive continuum CREATE-ONLY (8091-8100). Unique
 * gj_shell_green_8100 surface only; no multi-def. Distinct from
 * gj_shell_green_8000 (batch7995), gj_shell_green_7000 (batch6995),
 * gj_shell_green_6000 (batch5995), and gj_libcgj_green_8100
 * (batch8096). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8095_marker[] = "libcgj-batch8095";

/* Product shell green lamp for wave 8100. */
#define B8095_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8095_green(void)
{
	return B8095_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_8100 - report product shell green lamp for wave 8100.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_8100(void)
{
	(void)NULL;
	return b8095_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_8100(void)
    __attribute__((alias("gj_shell_green_8100")));
