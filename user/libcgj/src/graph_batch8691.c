/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8691: shell product green extract (wave 8700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_8700(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       8700 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_8700  (alias)
 *   __libcgj_batch8691_marker = "libcgj-batch8691"
 *
 * Milestone 8700 exclusive continuum CREATE-ONLY (8691-8700). Unique
 * gj_shell_green_8700 surface only; no multi-def. Distinct from
 * gj_shell_green_8600 (batch8591), gj_shell_green_8500 (batch8491),
 * gj_shell_green_8400 (batch8391), and gj_libcgj_green_8700
 * (batch8692). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8691_marker[] = "libcgj-batch8691";

/* Product shell green lamp for wave 8700. */
#define B8691_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8691_green(void)
{
	return B8691_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_8700 - report product shell green lamp for wave 8700.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_8700(void)
{
	(void)NULL;
	return b8691_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_8700(void)
    __attribute__((alias("gj_shell_green_8700")));
