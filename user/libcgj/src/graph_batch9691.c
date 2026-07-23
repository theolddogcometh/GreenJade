/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9691: shell product green extract (wave 9700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_9700(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       9700 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_9700  (alias)
 *   __libcgj_batch9691_marker = "libcgj-batch9691"
 *
 * Milestone 9700 exclusive continuum CREATE-ONLY (9691-9700). Unique
 * gj_shell_green_9700 surface only; no multi-def. Distinct from
 * gj_shell_green_9600 (batch9591), gj_shell_green_9500 (batch9491),
 * gj_shell_green_9400 (batch9391), and gj_libcgj_green_9700
 * (batch9692). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9691_marker[] = "libcgj-batch9691";

/* Product shell green lamp for wave 9700. */
#define B9691_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9691_green(void)
{
	return B9691_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_9700 - report product shell green lamp for wave 9700.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_9700(void)
{
	(void)NULL;
	return b9691_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_9700(void)
    __attribute__((alias("gj_shell_green_9700")));
