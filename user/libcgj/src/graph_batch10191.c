/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10191: shell product green extract (wave 10200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_10200(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       10200 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_10200  (alias)
 *   __libcgj_batch10191_marker = "libcgj-batch10191"
 *
 * Milestone 10200 exclusive continuum CREATE-ONLY (10191-10200). Unique
 * gj_shell_green_10200 surface only; no multi-def. Distinct from
 * gj_shell_green_10100 (batch10091 continuum form), gj_shell_green_10000
 * (batch9991), gj_shell_green_9900 (batch9891), and gj_libcgj_green_10200
 * (batch10192). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10191_marker[] = "libcgj-batch10191";

/* Product shell green lamp for wave 10200. */
#define B10191_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10191_green(void)
{
	return B10191_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_10200 - report product shell green lamp for wave 10200.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_10200(void)
{
	(void)NULL;
	return b10191_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_10200(void)
    __attribute__((alias("gj_shell_green_10200")));
