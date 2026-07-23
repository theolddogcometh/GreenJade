/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5295: shell product green extract (wave 5300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_5300(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       5300 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_5300  (alias)
 *   __libcgj_batch5295_marker = "libcgj-batch5295"
 *
 * Milestone 5300 exclusive continuum CREATE-ONLY (5291-5300). Unique
 * gj_shell_green_5300 surface only; no multi-def. Distinct from
 * gj_shell_green_5200 (batch5195), gj_shell_green_5100 (batch5095),
 * gj_shell_green_5000 (batch4995), and gj_libcgj_green_5300
 * (batch5296). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5295_marker[] = "libcgj-batch5295";

/* Product shell green lamp for wave 5300. */
#define B5295_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5295_green(void)
{
	return B5295_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_5300 - report product shell green lamp for wave 5300.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_5300(void)
{
	(void)NULL;
	return b5295_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_5300(void)
    __attribute__((alias("gj_shell_green_5300")));
