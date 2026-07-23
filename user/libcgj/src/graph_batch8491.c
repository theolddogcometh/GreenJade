/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8491: shell product green extract (wave 8500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_8500(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       8500 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_8500  (alias)
 *   __libcgj_batch8491_marker = "libcgj-batch8491"
 *
 * Milestone 8500 exclusive continuum CREATE-ONLY (8491-8500). Unique
 * gj_shell_green_8500 surface only; no multi-def. Distinct from
 * gj_shell_green_8300 (batch8291), gj_shell_green_8200 (batch8195),
 * gj_shell_green_8000 (batch7995), and gj_libcgj_green_8500
 * (batch8492). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8491_marker[] = "libcgj-batch8491";

/* Product shell green lamp for wave 8500. */
#define B8491_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8491_green(void)
{
	return B8491_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_8500 - report product shell green lamp for wave 8500.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_8500(void)
{
	(void)NULL;
	return b8491_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_8500(void)
    __attribute__((alias("gj_shell_green_8500")));
