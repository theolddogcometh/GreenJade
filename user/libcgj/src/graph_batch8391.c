/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8391: shell product green extract (wave 8400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_8400(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       8400 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_8400  (alias)
 *   __libcgj_batch8391_marker = "libcgj-batch8391"
 *
 * Milestone 8400 exclusive continuum CREATE-ONLY (8391-8400). Unique
 * gj_shell_green_8400 surface only; no multi-def. Distinct from
 * gj_shell_green_8200 (batch8195), gj_shell_green_8100 (batch8095),
 * gj_shell_green_8000 (batch7995), and gj_libcgj_green_8400
 * (batch8392). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8391_marker[] = "libcgj-batch8391";

/* Product shell green lamp for wave 8400. */
#define B8391_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8391_green(void)
{
	return B8391_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_8400 - report product shell green lamp for wave 8400.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_8400(void)
{
	(void)NULL;
	return b8391_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_8400(void)
    __attribute__((alias("gj_shell_green_8400")));
