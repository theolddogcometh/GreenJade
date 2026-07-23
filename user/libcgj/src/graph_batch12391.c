/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12391: shell product green extract (wave 12400).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_12400(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       12400 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_12400  (alias)
 *   __libcgj_batch12391_marker = "libcgj-batch12391"
 *
 * Milestone 12400 exclusive continuum CREATE-ONLY (12391-12400). Unique
 * gj_shell_green_12400 surface only; no multi-def. Distinct from
 * gj_shell_green_12300 (batch12291 continuum form), gj_shell_green_12200
 * (batch12191), gj_shell_green_12100 (batch12091), and gj_libcgj_green_12400
 * (batch12392). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12391_marker[] = "libcgj-batch12391";

/* Product shell green lamp for wave 12400. */
#define B12391_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12391_green(void)
{
	return B12391_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_12400 - report product shell green lamp for wave 12400.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_12400(void)
{
	(void)NULL;
	return b12391_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_12400(void)
    __attribute__((alias("gj_shell_green_12400")));
