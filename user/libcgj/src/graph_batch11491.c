/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11491: shell product green extract (wave 11500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_11500(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       11500 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_11500  (alias)
 *   __libcgj_batch11491_marker = "libcgj-batch11491"
 *
 * Milestone 11500 exclusive continuum CREATE-ONLY (11491-11500). Unique
 * gj_shell_green_11500 surface only; no multi-def. Distinct from
 * gj_shell_green_11400 (batch11391 continuum form), gj_shell_green_11300
 * (batch11291), gj_shell_green_11200 (batch11191), and gj_libcgj_green_11500
 * (batch11492). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11491_marker[] = "libcgj-batch11491";

/* Product shell green lamp for wave 11500. */
#define B11491_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11491_green(void)
{
	return B11491_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_11500 - report product shell green lamp for wave 11500.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_11500(void)
{
	(void)NULL;
	return b11491_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_11500(void)
    __attribute__((alias("gj_shell_green_11500")));
