/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4745: shell product green extract (wave 4750).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_4750(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       4750 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_4750  (alias)
 *   __libcgj_batch4745_marker = "libcgj-batch4745"
 *
 * Milestone 4750 exclusive continuum CREATE-ONLY (4741-4750). Unique
 * gj_shell_green_4750 surface only; no multi-def. Distinct from
 * gj_shell_green_4700 (batch4695), gj_shell_green_4650 (batch4645),
 * gj_shell_green_4600 (batch4595), and gj_libcgj_green_4750
 * (batch4746). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4745_marker[] = "libcgj-batch4745";

/* Product shell green lamp for wave 4750. */
#define B4745_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4745_green(void)
{
	return B4745_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_4750 - report product shell green lamp for wave 4750.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_4750(void)
{
	(void)NULL;
	return b4745_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_4750(void)
    __attribute__((alias("gj_shell_green_4750")));
