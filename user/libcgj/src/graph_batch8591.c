/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8591: shell product green extract (wave 8600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_8600(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       8600 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_8600  (alias)
 *   __libcgj_batch8591_marker = "libcgj-batch8591"
 *
 * Milestone 8600 exclusive continuum CREATE-ONLY (8591-8600). Unique
 * gj_shell_green_8600 surface only; no multi-def. Distinct from
 * gj_shell_green_8500 (batch8491), gj_shell_green_8400 (batch8391),
 * gj_shell_green_8300 (batch8291), and gj_libcgj_green_8600
 * (batch8592). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8591_marker[] = "libcgj-batch8591";

/* Product shell green lamp for wave 8600. */
#define B8591_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8591_green(void)
{
	return B8591_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_8600 - report product shell green lamp for wave 8600.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_8600(void)
{
	(void)NULL;
	return b8591_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_8600(void)
    __attribute__((alias("gj_shell_green_8600")));
