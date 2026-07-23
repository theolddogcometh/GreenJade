/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5245: shell product green extract (wave 5250).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_5250(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       5250 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_5250  (alias)
 *   __libcgj_batch5245_marker = "libcgj-batch5245"
 *
 * Milestone 5250 exclusive continuum CREATE-ONLY (5241-5250). Unique
 * gj_shell_green_5250 surface only; no multi-def. Distinct from
 * gj_shell_green_5200 (batch5195), gj_shell_green_5100 (batch5095),
 * gj_shell_green_5000 (batch4995), and gj_libcgj_green_5250
 * (batch5246). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5245_marker[] = "libcgj-batch5245";

/* Product shell green lamp for wave 5250. */
#define B5245_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5245_green(void)
{
	return B5245_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_5250 - report product shell green lamp for wave 5250.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_5250(void)
{
	(void)NULL;
	return b5245_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_5250(void)
    __attribute__((alias("gj_shell_green_5250")));
