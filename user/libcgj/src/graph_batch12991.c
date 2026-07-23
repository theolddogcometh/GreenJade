/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12991: shell product green extract (wave 13000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_13000(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       13000 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_13000  (alias)
 *   __libcgj_batch12991_marker = "libcgj-batch12991"
 *
 * Milestone 13000 exclusive continuum CREATE-ONLY (12991-13000). Unique
 * gj_shell_green_13000 surface only; no multi-def. Distinct from
 * gj_shell_green_12900 (batch12891 continuum form), gj_shell_green_12800
 * (batch12791), gj_shell_green_12700 (batch12691), and gj_libcgj_green_13000
 * (batch12992). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12991_marker[] = "libcgj-batch12991";

/* Product shell green lamp for wave 13000. */
#define B12991_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12991_green(void)
{
	return B12991_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_13000 - report product shell green lamp for wave 13000.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_13000(void)
{
	(void)NULL;
	return b12991_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_13000(void)
    __attribute__((alias("gj_shell_green_13000")));
