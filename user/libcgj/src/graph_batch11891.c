/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11891: shell product green extract (wave 11900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_11900(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       11900 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_11900  (alias)
 *   __libcgj_batch11891_marker = "libcgj-batch11891"
 *
 * Milestone 11900 exclusive continuum CREATE-ONLY (11891-11900). Unique
 * gj_shell_green_11900 surface only; no multi-def. Distinct from
 * gj_shell_green_11800 (batch11791 continuum form), gj_shell_green_11700
 * (batch11691), gj_shell_green_11600 (batch11591), and gj_libcgj_green_11900
 * (batch11892). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11891_marker[] = "libcgj-batch11891";

/* Product shell green lamp for wave 11900. */
#define B11891_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11891_green(void)
{
	return B11891_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_11900 - report product shell green lamp for wave 11900.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_11900(void)
{
	(void)NULL;
	return b11891_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_11900(void)
    __attribute__((alias("gj_shell_green_11900")));
