/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4895: shell product green extract (wave 4900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_4900(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       4900 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_4900  (alias)
 *   __libcgj_batch4895_marker = "libcgj-batch4895"
 *
 * Milestone 4900 exclusive continuum CREATE-ONLY (4891-4900). Unique
 * gj_shell_green_4900 surface only; no multi-def. Distinct from
 * gj_shell_green_4850 (batch4845), gj_shell_green_4800 (batch4795),
 * gj_shell_green_4750 (batch4745), and gj_libcgj_green_4900
 * (batch4896). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4895_marker[] = "libcgj-batch4895";

/* Product shell green lamp for wave 4900. */
#define B4895_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4895_green(void)
{
	return B4895_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_4900 - report product shell green lamp for wave 4900.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_4900(void)
{
	(void)NULL;
	return b4895_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_4900(void)
    __attribute__((alias("gj_shell_green_4900")));
