/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4845: shell product green extract (wave 4850).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_4850(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       4850 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_4850  (alias)
 *   __libcgj_batch4845_marker = "libcgj-batch4845"
 *
 * Milestone 4850 exclusive continuum CREATE-ONLY (4841-4850). Unique
 * gj_shell_green_4850 surface only; no multi-def. Distinct from
 * gj_shell_green_4800 (batch4795), gj_shell_green_4750 (batch4745),
 * gj_shell_green_4700 (batch4695), and gj_libcgj_green_4850
 * (batch4846). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4845_marker[] = "libcgj-batch4845";

/* Product shell green lamp for wave 4850. */
#define B4845_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4845_green(void)
{
	return B4845_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_4850 - report product shell green lamp for wave 4850.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_4850(void)
{
	(void)NULL;
	return b4845_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_4850(void)
    __attribute__((alias("gj_shell_green_4850")));
