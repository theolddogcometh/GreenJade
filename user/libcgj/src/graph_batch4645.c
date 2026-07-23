/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4645: shell product green extract (wave 4650).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_4650(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       4650 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_4650  (alias)
 *   __libcgj_batch4645_marker = "libcgj-batch4645"
 *
 * Milestone 4650 exclusive continuum CREATE-ONLY (4641-4650). Unique
 * gj_shell_green_4650 surface only; no multi-def. Distinct from
 * gj_shell_green_4600 (batch4595), gj_shell_green_4550 (batch4545),
 * gj_shell_green_4500 (batch4495), and gj_libcgj_green_4650
 * (batch4646). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4645_marker[] = "libcgj-batch4645";

/* Product shell green lamp for wave 4650. */
#define B4645_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4645_green(void)
{
	return B4645_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_4650 - report product shell green lamp for wave 4650.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_4650(void)
{
	(void)NULL;
	return b4645_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_4650(void)
    __attribute__((alias("gj_shell_green_4650")));
