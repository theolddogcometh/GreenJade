/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5995: shell product green extract (wave 6000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_6000(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       6000 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_6000  (alias)
 *   __libcgj_batch5995_marker = "libcgj-batch5995"
 *
 * Milestone 6000 exclusive continuum CREATE-ONLY (5991-6000). Unique
 * gj_shell_green_6000 surface only; no multi-def. Distinct from
 * gj_shell_green_5900 (batch5895), gj_shell_green_5800 (batch5795),
 * gj_shell_green_5700 (batch5695), and gj_libcgj_green_6000
 * (batch5996). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5995_marker[] = "libcgj-batch5995";

/* Product shell green lamp for wave 6000. */
#define B5995_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5995_green(void)
{
	return B5995_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_6000 - report product shell green lamp for wave 6000.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_6000(void)
{
	(void)NULL;
	return b5995_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_6000(void)
    __attribute__((alias("gj_shell_green_6000")));
