/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4545: shell product green extract (wave 4550).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_4550(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       4550 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_4550  (alias)
 *   __libcgj_batch4545_marker = "libcgj-batch4545"
 *
 * Milestone 4550 exclusive continuum CREATE-ONLY (4541-4550). Unique
 * gj_shell_green_4550 surface only; no multi-def. Distinct from
 * gj_shell_green_4500 (batch4495), gj_shell_green_4400 (batch4395),
 * gj_shell_green_4300 (batch4295), and gj_libcgj_green_4550
 * (batch4546). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4545_marker[] = "libcgj-batch4545";

/* Product shell green lamp for wave 4550. */
#define B4545_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4545_green(void)
{
	return B4545_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_4550 - report product shell green lamp for wave 4550.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_4550(void)
{
	(void)NULL;
	return b4545_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_4550(void)
    __attribute__((alias("gj_shell_green_4550")));
