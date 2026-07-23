/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9091: shell product green extract (wave 9100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_9100(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       9100 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_9100  (alias)
 *   __libcgj_batch9091_marker = "libcgj-batch9091"
 *
 * Milestone 9100 exclusive continuum CREATE-ONLY (9091-9100). Unique
 * gj_shell_green_9100 surface only; no multi-def. Distinct from
 * gj_shell_green_9000 (batch8991), gj_shell_green_8900 (batch8891),
 * gj_shell_green_8800 (batch8791), and gj_libcgj_green_9100
 * (batch9092). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9091_marker[] = "libcgj-batch9091";

/* Product shell green lamp for wave 9100. */
#define B9091_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9091_green(void)
{
	return B9091_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_9100 - report product shell green lamp for wave 9100.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_9100(void)
{
	(void)NULL;
	return b9091_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_9100(void)
    __attribute__((alias("gj_shell_green_9100")));
