/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11291: shell product green extract (wave 11300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_11300(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       11300 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_11300  (alias)
 *   __libcgj_batch11291_marker = "libcgj-batch11291"
 *
 * Milestone 11300 exclusive continuum CREATE-ONLY (11291-11300). Unique
 * gj_shell_green_11300 surface only; no multi-def. Distinct from
 * gj_shell_green_11200 (batch11191 continuum form), gj_shell_green_11100
 * (batch11091), gj_shell_green_11000 (batch10991), and gj_libcgj_green_11300
 * (batch11292). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11291_marker[] = "libcgj-batch11291";

/* Product shell green lamp for wave 11300. */
#define B11291_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11291_green(void)
{
	return B11291_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_11300 - report product shell green lamp for wave 11300.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_11300(void)
{
	(void)NULL;
	return b11291_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_11300(void)
    __attribute__((alias("gj_shell_green_11300")));
