/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6895: shell product green extract (wave 6900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_6900(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       6900 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_6900  (alias)
 *   __libcgj_batch6895_marker = "libcgj-batch6895"
 *
 * Milestone 6900 exclusive continuum CREATE-ONLY (6891-6900). Unique
 * gj_shell_green_6900 surface only; no multi-def. Distinct from
 * gj_shell_green_6800 (batch6795), gj_shell_green_6700 (batch6695),
 * gj_shell_green_6600 (batch6595), and gj_libcgj_green_6900
 * (batch6896). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6895_marker[] = "libcgj-batch6895";

/* Product shell green lamp for wave 6900. */
#define B6895_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6895_green(void)
{
	return B6895_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_6900 - report product shell green lamp for wave 6900.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_6900(void)
{
	(void)NULL;
	return b6895_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_6900(void)
    __attribute__((alias("gj_shell_green_6900")));
