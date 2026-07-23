/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9591: shell product green extract (wave 9600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_9600(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       9600 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_9600  (alias)
 *   __libcgj_batch9591_marker = "libcgj-batch9591"
 *
 * Milestone 9600 exclusive continuum CREATE-ONLY (9591-9600). Unique
 * gj_shell_green_9600 surface only; no multi-def. Distinct from
 * gj_shell_green_9500 (batch9491), gj_shell_green_9400 (batch9391),
 * gj_shell_green_9300 (batch9291), and gj_libcgj_green_9600
 * (batch9592). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9591_marker[] = "libcgj-batch9591";

/* Product shell green lamp for wave 9600. */
#define B9591_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9591_green(void)
{
	return B9591_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_9600 - report product shell green lamp for wave 9600.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_9600(void)
{
	(void)NULL;
	return b9591_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_9600(void)
    __attribute__((alias("gj_shell_green_9600")));
