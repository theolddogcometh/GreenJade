/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5895: shell product green extract (wave 5900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_5900(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       5900 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_5900  (alias)
 *   __libcgj_batch5895_marker = "libcgj-batch5895"
 *
 * Milestone 5900 exclusive continuum CREATE-ONLY (5891-5900). Unique
 * gj_shell_green_5900 surface only; no multi-def. Distinct from
 * gj_shell_green_5800 (batch5795), gj_shell_green_5700 (batch5695),
 * gj_shell_green_5650 (batch5645), and gj_libcgj_green_5900
 * (batch5896). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5895_marker[] = "libcgj-batch5895";

/* Product shell green lamp for wave 5900. */
#define B5895_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5895_green(void)
{
	return B5895_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_5900 - report product shell green lamp for wave 5900.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_5900(void)
{
	(void)NULL;
	return b5895_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_5900(void)
    __attribute__((alias("gj_shell_green_5900")));
