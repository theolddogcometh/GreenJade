/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7095: shell product green extract (wave 7100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_7100(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       7100 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_7100  (alias)
 *   __libcgj_batch7095_marker = "libcgj-batch7095"
 *
 * Milestone 7100 exclusive continuum CREATE-ONLY (7091-7100). Unique
 * gj_shell_green_7100 surface only; no multi-def. Distinct from
 * gj_shell_green_7000 (batch6995), gj_shell_green_6000 (batch5995),
 * gj_shell_green_5900 (batch5895), and gj_libcgj_green_7100
 * (batch7096). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7095_marker[] = "libcgj-batch7095";

/* Product shell green lamp for wave 7100. */
#define B7095_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7095_green(void)
{
	return B7095_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_7100 - report product shell green lamp for wave 7100.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_7100(void)
{
	(void)NULL;
	return b7095_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_7100(void)
    __attribute__((alias("gj_shell_green_7100")));
