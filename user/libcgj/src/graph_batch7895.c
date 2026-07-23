/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7895: shell product green extract (wave 7900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_7900(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       7900 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_7900  (alias)
 *   __libcgj_batch7895_marker = "libcgj-batch7895"
 *
 * Milestone 7900 exclusive continuum CREATE-ONLY (7891-7900). Unique
 * gj_shell_green_7900 surface only; no multi-def. Distinct from
 * gj_shell_green_7800 (batch7795), gj_shell_green_7700 (batch7695),
 * gj_shell_green_7600 (batch7595), and gj_libcgj_green_7900
 * (batch7896). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7895_marker[] = "libcgj-batch7895";

/* Product shell green lamp for wave 7900. */
#define B7895_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7895_green(void)
{
	return B7895_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_7900 - report product shell green lamp for wave 7900.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_7900(void)
{
	(void)NULL;
	return b7895_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_7900(void)
    __attribute__((alias("gj_shell_green_7900")));
