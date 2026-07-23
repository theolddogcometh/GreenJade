/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13891: shell product green extract (wave 13900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_13900(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       13900 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_13900  (alias)
 *   __libcgj_batch13891_marker = "libcgj-batch13891"
 *
 * Milestone 13900 exclusive continuum CREATE-ONLY (13891-13900). Unique
 * gj_shell_green_13900 surface only; no multi-def. Distinct from
 * gj_shell_green_13800 (batch13791 continuum form), gj_shell_green_13700
 * (batch13691), gj_shell_green_13600 (batch13591), and gj_libcgj_green_13900
 * (batch13892). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13891_marker[] = "libcgj-batch13891";

/* Product shell green lamp for wave 13900. */
#define B13891_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13891_green(void)
{
	return B13891_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_13900 - report product shell green lamp for wave 13900.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_13900(void)
{
	(void)NULL;
	return b13891_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_13900(void)
    __attribute__((alias("gj_shell_green_13900")));
