/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10091: shell product green extract (wave 10100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_10100(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       10100 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_10100  (alias)
 *   __libcgj_batch10091_marker = "libcgj-batch10091"
 *
 * Milestone 10100 exclusive continuum CREATE-ONLY (10091-10100). Unique
 * gj_shell_green_10100 surface only; no multi-def. Distinct from
 * gj_shell_green_10000 (batch9991 continuum form), gj_shell_green_9900
 * (batch9891), gj_shell_green_9100 (batch9091), and gj_libcgj_green_10100
 * (batch10092). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10091_marker[] = "libcgj-batch10091";

/* Product shell green lamp for wave 10100. */
#define B10091_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10091_green(void)
{
	return B10091_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_10100 - report product shell green lamp for wave 10100.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_10100(void)
{
	(void)NULL;
	return b10091_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_10100(void)
    __attribute__((alias("gj_shell_green_10100")));
