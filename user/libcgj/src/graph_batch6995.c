/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6995: shell product green extract (wave 7000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_7000(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       7000 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_7000  (alias)
 *   __libcgj_batch6995_marker = "libcgj-batch6995"
 *
 * Milestone 7000 exclusive continuum CREATE-ONLY (6991-7000). Unique
 * gj_shell_green_7000 surface only; no multi-def. Distinct from
 * gj_shell_green_6000 (batch5995), gj_shell_green_5900 (batch5895),
 * gj_shell_green_5800 (batch5795), and gj_libcgj_green_7000
 * (batch6996). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6995_marker[] = "libcgj-batch6995";

/* Product shell green lamp for wave 7000. */
#define B6995_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6995_green(void)
{
	return B6995_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_7000 - report product shell green lamp for wave 7000.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_7000(void)
{
	(void)NULL;
	return b6995_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_7000(void)
    __attribute__((alias("gj_shell_green_7000")));
