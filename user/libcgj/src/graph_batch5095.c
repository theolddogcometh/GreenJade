/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5095: shell product green extract (wave 5100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_5100(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       5100 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_5100  (alias)
 *   __libcgj_batch5095_marker = "libcgj-batch5095"
 *
 * Milestone 5100 exclusive continuum CREATE-ONLY (5091-5100). Unique
 * gj_shell_green_5100 surface only; no multi-def. Distinct from
 * gj_shell_green_5000 (batch4995), gj_shell_green_4900 (batch4895),
 * gj_shell_green_4850 (batch4845), and gj_libcgj_green_5100
 * (batch5096). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5095_marker[] = "libcgj-batch5095";

/* Product shell green lamp for wave 5100. */
#define B5095_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5095_green(void)
{
	return B5095_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_5100 - report product shell green lamp for wave 5100.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_5100(void)
{
	(void)NULL;
	return b5095_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_5100(void)
    __attribute__((alias("gj_shell_green_5100")));
