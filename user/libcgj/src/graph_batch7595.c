/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7595: shell product green extract (wave 7600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_7600(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       7600 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_7600  (alias)
 *   __libcgj_batch7595_marker = "libcgj-batch7595"
 *
 * Milestone 7600 exclusive continuum CREATE-ONLY (7591-7600). Unique
 * gj_shell_green_7600 surface only; no multi-def. Distinct from
 * gj_shell_green_7500 (batch7495), gj_shell_green_7400 (batch7395),
 * gj_shell_green_7300 (batch7295), and gj_libcgj_green_7600
 * (batch7596). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7595_marker[] = "libcgj-batch7595";

/* Product shell green lamp for wave 7600. */
#define B7595_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7595_green(void)
{
	return B7595_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_7600 - report product shell green lamp for wave 7600.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_7600(void)
{
	(void)NULL;
	return b7595_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_7600(void)
    __attribute__((alias("gj_shell_green_7600")));
