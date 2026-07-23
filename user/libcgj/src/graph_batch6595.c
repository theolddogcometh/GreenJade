/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6595: shell product green extract (wave 6600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_6600(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       6600 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_6600  (alias)
 *   __libcgj_batch6595_marker = "libcgj-batch6595"
 *
 * Milestone 6600 exclusive continuum CREATE-ONLY (6591-6600). Unique
 * gj_shell_green_6600 surface only; no multi-def. Distinct from
 * gj_shell_green_6500 (batch6495), gj_shell_green_6400 (batch6395),
 * gj_shell_green_6300 (batch6295), and gj_libcgj_green_6600
 * (batch6596). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6595_marker[] = "libcgj-batch6595";

/* Product shell green lamp for wave 6600. */
#define B6595_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6595_green(void)
{
	return B6595_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_6600 - report product shell green lamp for wave 6600.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_6600(void)
{
	(void)NULL;
	return b6595_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_6600(void)
    __attribute__((alias("gj_shell_green_6600")));
