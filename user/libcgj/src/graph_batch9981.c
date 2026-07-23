/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9981: shell product green extract (wave 10000).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_10000(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       10000 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_10000  (alias)
 *   __libcgj_batch9981_marker = "libcgj-batch9981"
 *
 * Milestone 10000 exclusive continuum CREATE-ONLY (9981-9990). Unique
 * gj_shell_green_10000 surface only; no multi-def. Distinct from
 * gj_shell_green_9900 (batch9891), gj_shell_green_9800 (batch9791),
 * gj_shell_green_9700 (batch9691), and gj_libcgj_green_10000
 * (batch9982). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9981_marker[] = "libcgj-batch9981";

/* Product shell green lamp for wave 10000. */
#define B9981_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9981_green(void)
{
	return B9981_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_10000 - report product shell green lamp for wave 10000.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_10000(void)
{
	(void)NULL;
	return b9981_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_10000(void)
    __attribute__((alias("gj_shell_green_10000")));
