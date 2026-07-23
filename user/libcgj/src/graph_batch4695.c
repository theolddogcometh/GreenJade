/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4695: shell product green extract (wave 4700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_4700(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       4700 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_4700  (alias)
 *   __libcgj_batch4695_marker = "libcgj-batch4695"
 *
 * Milestone 4700 exclusive continuum CREATE-ONLY (4691-4700). Unique
 * gj_shell_green_4700 surface only; no multi-def. Distinct from
 * gj_shell_green_4650 (batch4645), gj_shell_green_4600 (batch4595),
 * gj_shell_green_4550 (batch4545), and gj_libcgj_green_4700
 * (batch4696). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4695_marker[] = "libcgj-batch4695";

/* Product shell green lamp for wave 4700. */
#define B4695_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4695_green(void)
{
	return B4695_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_4700 - report product shell green lamp for wave 4700.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_4700(void)
{
	(void)NULL;
	return b4695_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_4700(void)
    __attribute__((alias("gj_shell_green_4700")));
