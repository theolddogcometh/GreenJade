/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4195: shell product green extract (wave 4200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_4200(void);
 *     - Returns 1: milestone 4200 continuum product shell is green.
 *       Soft compile-time product tag; not a runtime probe of
 *       greenjade-shell.
 *   uint32_t __gj_shell_green_4200  (alias)
 *   __libcgj_batch4195_marker = "libcgj-batch4195"
 *
 * Milestone 4200 exclusive continuum CREATE-ONLY (4191-4200). Unique
 * gj_shell_green_4200 surface only; no multi-def. Distinct from
 * gj_shell_green_4100 (batch4095), gj_shell_green_3950 (batch3945),
 * gj_shell_green_3900 (batch3895), and gj_libcgj_green_4200
 * (batch4196). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4195_marker[] = "libcgj-batch4195";

/* Product shell green lamp for wave 4200. */
#define B4195_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4195_green(void)
{
	return B4195_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_4200 - report milestone-4200 product shell green status.
 *
 * Always returns 1 (shell green). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_4200(void)
{
	(void)NULL;
	return b4195_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_4200(void)
    __attribute__((alias("gj_shell_green_4200")));
