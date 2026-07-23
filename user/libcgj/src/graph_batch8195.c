/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8195: shell product green extract (wave 8200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_8200(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       8200 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_8200  (alias)
 *   __libcgj_batch8195_marker = "libcgj-batch8195"
 *
 * Milestone 8200 exclusive continuum CREATE-ONLY (8191-8200). Unique
 * gj_shell_green_8200 surface only; no multi-def. Distinct from
 * gj_shell_green_8000 (batch7995), gj_shell_green_7000 (batch6995),
 * gj_shell_green_6000 (batch5995), and gj_libcgj_green_8200
 * (batch8196). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8195_marker[] = "libcgj-batch8195";

/* Product shell green lamp for wave 8200. */
#define B8195_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8195_green(void)
{
	return B8195_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_8200 - report product shell green lamp for wave 8200.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_8200(void)
{
	(void)NULL;
	return b8195_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_8200(void)
    __attribute__((alias("gj_shell_green_8200")));
