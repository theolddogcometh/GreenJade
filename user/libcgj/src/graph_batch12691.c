/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12691: shell product green extract (wave 12700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_12700(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       12700 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_12700  (alias)
 *   __libcgj_batch12691_marker = "libcgj-batch12691"
 *
 * Milestone 12700 exclusive continuum CREATE-ONLY (12691-12700). Unique
 * gj_shell_green_12700 surface only; no multi-def. Distinct from
 * gj_shell_green_12600 (batch12591 continuum form), gj_shell_green_12500
 * (batch12491), gj_shell_green_12400 (batch12391), and gj_libcgj_green_12700
 * (batch12692). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12691_marker[] = "libcgj-batch12691";

/* Product shell green lamp for wave 12700. */
#define B12691_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12691_green(void)
{
	return B12691_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_12700 - report product shell green lamp for wave 12700.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_12700(void)
{
	(void)NULL;
	return b12691_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_12700(void)
    __attribute__((alias("gj_shell_green_12700")));
