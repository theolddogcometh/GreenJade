/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6195: shell product green extract (wave 6200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_6200(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       6200 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_6200  (alias)
 *   __libcgj_batch6195_marker = "libcgj-batch6195"
 *
 * Milestone 6200 exclusive continuum CREATE-ONLY (6191-6200). Unique
 * gj_shell_green_6200 surface only; no multi-def. Distinct from
 * gj_shell_green_6000 (batch5995), gj_shell_green_5900 (batch5895),
 * gj_shell_green_5800 (batch5795), and gj_libcgj_green_6200
 * (batch6196). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6195_marker[] = "libcgj-batch6195";

/* Product shell green lamp for wave 6200. */
#define B6195_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6195_green(void)
{
	return B6195_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_6200 - report product shell green lamp for wave 6200.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_6200(void)
{
	(void)NULL;
	return b6195_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_6200(void)
    __attribute__((alias("gj_shell_green_6200")));
