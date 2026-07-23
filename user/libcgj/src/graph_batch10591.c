/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10591: shell product green extract (wave 10600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_10600(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       10600 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_10600  (alias)
 *   __libcgj_batch10591_marker = "libcgj-batch10591"
 *
 * Milestone 10600 exclusive continuum CREATE-ONLY (10591-10600). Unique
 * gj_shell_green_10600 surface only; no multi-def. Distinct from
 * gj_shell_green_10500 (batch10491 continuum form), gj_shell_green_10400
 * (batch10391), gj_shell_green_10300 (batch10291), and gj_libcgj_green_10600
 * (batch10592). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10591_marker[] = "libcgj-batch10591";

/* Product shell green lamp for wave 10600. */
#define B10591_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10591_green(void)
{
	return B10591_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_10600 - report product shell green lamp for wave 10600.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_10600(void)
{
	(void)NULL;
	return b10591_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_10600(void)
    __attribute__((alias("gj_shell_green_10600")));
