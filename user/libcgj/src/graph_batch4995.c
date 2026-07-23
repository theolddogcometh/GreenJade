/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4995: shell product green extract (wave 5000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_5000(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       5000 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_5000  (alias)
 *   __libcgj_batch4995_marker = "libcgj-batch4995"
 *
 * Milestone 5000 exclusive continuum CREATE-ONLY (4991-5000). Unique
 * gj_shell_green_5000 surface only; no multi-def. Distinct from
 * gj_shell_green_4900 (batch4895), gj_shell_green_4850 (batch4845),
 * gj_shell_green_4800 (batch4795), and gj_libcgj_green_5000
 * (batch4996). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4995_marker[] = "libcgj-batch4995";

/* Product shell green lamp for wave 5000. */
#define B4995_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4995_green(void)
{
	return B4995_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_5000 - report product shell green lamp for wave 5000.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_5000(void)
{
	(void)NULL;
	return b4995_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_5000(void)
    __attribute__((alias("gj_shell_green_5000")));
