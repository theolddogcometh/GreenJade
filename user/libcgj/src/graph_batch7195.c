/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7195: shell product green extract (wave 7200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_7200(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       7200 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_7200  (alias)
 *   __libcgj_batch7195_marker = "libcgj-batch7195"
 *
 * Milestone 7200 exclusive continuum CREATE-ONLY (7191-7200). Unique
 * gj_shell_green_7200 surface only; no multi-def. Distinct from
 * gj_shell_green_7100 (batch7095), gj_shell_green_7000 (batch6995),
 * gj_shell_green_6000 (batch5995), and gj_libcgj_green_7200
 * (batch7196). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7195_marker[] = "libcgj-batch7195";

/* Product shell green lamp for wave 7200. */
#define B7195_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7195_green(void)
{
	return B7195_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_7200 - report product shell green lamp for wave 7200.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_7200(void)
{
	(void)NULL;
	return b7195_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_7200(void)
    __attribute__((alias("gj_shell_green_7200")));
