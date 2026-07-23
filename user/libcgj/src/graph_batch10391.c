/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10391: shell product green extract (wave 10400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_10400(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       10400 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_10400  (alias)
 *   __libcgj_batch10391_marker = "libcgj-batch10391"
 *
 * Milestone 10400 exclusive continuum CREATE-ONLY (10391-10400). Unique
 * gj_shell_green_10400 surface only; no multi-def. Distinct from
 * gj_shell_green_10300 (batch10291 continuum form), gj_shell_green_10200
 * (batch10191), gj_shell_green_10100 (batch10091), and gj_libcgj_green_10400
 * (batch10392). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10391_marker[] = "libcgj-batch10391";

/* Product shell green lamp for wave 10400. */
#define B10391_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10391_green(void)
{
	return B10391_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_10400 - report product shell green lamp for wave 10400.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_10400(void)
{
	(void)NULL;
	return b10391_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_10400(void)
    __attribute__((alias("gj_shell_green_10400")));
