/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8891: shell product green extract (wave 8900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_8900(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       8900 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_8900  (alias)
 *   __libcgj_batch8891_marker = "libcgj-batch8891"
 *
 * Milestone 8900 exclusive continuum CREATE-ONLY (8891-8900). Unique
 * gj_shell_green_8900 surface only; no multi-def. Distinct from
 * gj_shell_green_8800 (batch8791), gj_shell_green_8700 (batch8691),
 * gj_shell_green_8600 (batch8591), and gj_libcgj_green_8900
 * (batch8892). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8891_marker[] = "libcgj-batch8891";

/* Product shell green lamp for wave 8900. */
#define B8891_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8891_green(void)
{
	return B8891_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_8900 - report product shell green lamp for wave 8900.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_8900(void)
{
	(void)NULL;
	return b8891_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_8900(void)
    __attribute__((alias("gj_shell_green_8900")));
