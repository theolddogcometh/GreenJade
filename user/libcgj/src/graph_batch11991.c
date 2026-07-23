/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11991: shell product green extract (wave 12000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_12000(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       12000 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_12000  (alias)
 *   __libcgj_batch11991_marker = "libcgj-batch11991"
 *
 * Milestone 12000 exclusive continuum CREATE-ONLY (11991-12000). Unique
 * gj_shell_green_12000 surface only; no multi-def. Distinct from
 * gj_shell_green_11900 (batch11891 continuum form), gj_shell_green_11800
 * (batch11791), gj_shell_green_11700 (batch11691), and gj_libcgj_green_12000
 * (batch11992). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11991_marker[] = "libcgj-batch11991";

/* Product shell green lamp for wave 12000. */
#define B11991_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11991_green(void)
{
	return B11991_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_12000 - report product shell green lamp for wave 12000.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_12000(void)
{
	(void)NULL;
	return b11991_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_12000(void)
    __attribute__((alias("gj_shell_green_12000")));
