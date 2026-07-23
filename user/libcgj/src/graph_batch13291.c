/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13291: shell product green extract (wave 13300).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_13300(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       13300 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_13300  (alias)
 *   __libcgj_batch13291_marker = "libcgj-batch13291"
 *
 * Milestone 13300 exclusive continuum CREATE-ONLY (13291-13300). Unique
 * gj_shell_green_13300 surface only; no multi-def. Distinct from
 * gj_shell_green_13200 (batch13191 continuum form), gj_shell_green_13100
 * (batch13091), gj_shell_green_13000 (batch12991), and gj_libcgj_green_13300
 * (batch13292). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13291_marker[] = "libcgj-batch13291";

/* Product shell green lamp for wave 13300. */
#define B13291_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13291_green(void)
{
	return B13291_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_13300 - report product shell green lamp for wave 13300.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_13300(void)
{
	(void)NULL;
	return b13291_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_13300(void)
    __attribute__((alias("gj_shell_green_13300")));
