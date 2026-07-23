/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9891: shell product green extract (wave 9900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_9900(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       9900 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_9900  (alias)
 *   __libcgj_batch9891_marker = "libcgj-batch9891"
 *
 * Milestone 9900 exclusive continuum CREATE-ONLY (9891-9900). Unique
 * gj_shell_green_9900 surface only; no multi-def. Distinct from
 * gj_shell_green_9800 (batch9791), gj_shell_green_9700 (batch9691),
 * gj_shell_green_9600 (batch9591), and gj_libcgj_green_9900
 * (batch9892). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9891_marker[] = "libcgj-batch9891";

/* Product shell green lamp for wave 9900. */
#define B9891_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9891_green(void)
{
	return B9891_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_9900 - report product shell green lamp for wave 9900.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_9900(void)
{
	(void)NULL;
	return b9891_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_9900(void)
    __attribute__((alias("gj_shell_green_9900")));
