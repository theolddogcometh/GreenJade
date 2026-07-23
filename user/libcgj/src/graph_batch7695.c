/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7695: shell product green extract (wave 7700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_7700(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       7700 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_7700  (alias)
 *   __libcgj_batch7695_marker = "libcgj-batch7695"
 *
 * Milestone 7700 exclusive continuum CREATE-ONLY (7691-7700). Unique
 * gj_shell_green_7700 surface only; no multi-def. Distinct from
 * gj_shell_green_7600 (batch7595), gj_shell_green_7500 (batch7495),
 * gj_shell_green_7400 (batch7395), and gj_libcgj_green_7700
 * (batch7696). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7695_marker[] = "libcgj-batch7695";

/* Product shell green lamp for wave 7700. */
#define B7695_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7695_green(void)
{
	return B7695_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_7700 - report product shell green lamp for wave 7700.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_7700(void)
{
	(void)NULL;
	return b7695_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_7700(void)
    __attribute__((alias("gj_shell_green_7700")));
