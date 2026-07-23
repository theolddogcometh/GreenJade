/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5145: shell product green extract (wave 5150).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_5150(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       5150 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_5150  (alias)
 *   __libcgj_batch5145_marker = "libcgj-batch5145"
 *
 * Milestone 5150 exclusive continuum CREATE-ONLY (5141-5150). Unique
 * gj_shell_green_5150 surface only; no multi-def. Distinct from
 * gj_shell_green_5100 (batch5095), gj_shell_green_5000 (batch4995),
 * gj_shell_green_4900 (batch4895), and gj_libcgj_green_5150
 * (batch5146). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5145_marker[] = "libcgj-batch5145";

/* Product shell green lamp for wave 5150. */
#define B5145_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5145_green(void)
{
	return B5145_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_5150 - report product shell green lamp for wave 5150.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_5150(void)
{
	(void)NULL;
	return b5145_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_5150(void)
    __attribute__((alias("gj_shell_green_5150")));
