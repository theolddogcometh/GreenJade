/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6395: shell product green extract (wave 6400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_6400(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       6400 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_6400  (alias)
 *   __libcgj_batch6395_marker = "libcgj-batch6395"
 *
 * Milestone 6400 exclusive continuum CREATE-ONLY (6391-6400). Unique
 * gj_shell_green_6400 surface only; no multi-def. Distinct from
 * gj_shell_green_6300 (batch6295), gj_shell_green_6200 (batch6195),
 * gj_shell_green_6100 (batch6095), and gj_libcgj_green_6400
 * (batch6396). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6395_marker[] = "libcgj-batch6395";

/* Product shell green lamp for wave 6400. */
#define B6395_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6395_green(void)
{
	return B6395_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_6400 - report product shell green lamp for wave 6400.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_6400(void)
{
	(void)NULL;
	return b6395_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_6400(void)
    __attribute__((alias("gj_shell_green_6400")));
