/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13091: shell product green extract (wave 13100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_13100(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       13100 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_13100  (alias)
 *   __libcgj_batch13091_marker = "libcgj-batch13091"
 *
 * Milestone 13100 exclusive continuum CREATE-ONLY (13091-13100). Unique
 * gj_shell_green_13100 surface only; no multi-def. Distinct from
 * gj_shell_green_13000 (batch12991 continuum form), gj_shell_green_12900
 * (batch12891), gj_shell_green_12800 (batch12791), and gj_libcgj_green_13100
 * (batch13092). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13091_marker[] = "libcgj-batch13091";

/* Product shell green lamp for wave 13100. */
#define B13091_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13091_green(void)
{
	return B13091_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_13100 - report product shell green lamp for wave 13100.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_13100(void)
{
	(void)NULL;
	return b13091_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_13100(void)
    __attribute__((alias("gj_shell_green_13100")));
