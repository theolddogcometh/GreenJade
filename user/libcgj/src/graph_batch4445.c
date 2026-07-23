/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4445: shell product green extract (wave 4450).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_4450(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       4450 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_4450  (alias)
 *   __libcgj_batch4445_marker = "libcgj-batch4445"
 *
 * Milestone 4450 exclusive continuum CREATE-ONLY (4441-4450). Unique
 * gj_shell_green_4450 surface only; no multi-def. Distinct from
 * gj_shell_green_4400 (batch4395), gj_shell_green_4300 (batch4295),
 * gj_shell_green_4100 (batch4095), and gj_libcgj_green_4450
 * (batch4446). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4445_marker[] = "libcgj-batch4445";

/* Product shell green lamp for wave 4450. */
#define B4445_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4445_green(void)
{
	return B4445_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_4450 - report product shell green lamp for wave 4450.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_4450(void)
{
	(void)NULL;
	return b4445_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_4450(void)
    __attribute__((alias("gj_shell_green_4450")));
