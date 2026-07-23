/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5045: shell product green extract (wave 5050).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_5050(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       5050 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_5050  (alias)
 *   __libcgj_batch5045_marker = "libcgj-batch5045"
 *
 * Milestone 5050 exclusive continuum CREATE-ONLY (5041-5050). Unique
 * gj_shell_green_5050 surface only; no multi-def. Distinct from
 * gj_shell_green_5000 (batch4995), gj_shell_green_4900 (batch4895),
 * gj_shell_green_4850 (batch4845), and gj_libcgj_green_5050
 * (batch5046). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5045_marker[] = "libcgj-batch5045";

/* Product shell green lamp for wave 5050. */
#define B5045_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5045_green(void)
{
	return B5045_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_5050 - report product shell green lamp for wave 5050.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_5050(void)
{
	(void)NULL;
	return b5045_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_5050(void)
    __attribute__((alias("gj_shell_green_5050")));
