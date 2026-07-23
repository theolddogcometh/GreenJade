/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9491: shell product green extract (wave 9500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_9500(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       9500 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_9500  (alias)
 *   __libcgj_batch9491_marker = "libcgj-batch9491"
 *
 * Milestone 9500 exclusive continuum CREATE-ONLY (9491-9500). Unique
 * gj_shell_green_9500 surface only; no multi-def. Distinct from
 * gj_shell_green_9400 (batch9391), gj_shell_green_9300 (batch9291),
 * gj_shell_green_9200 (batch9191), and gj_libcgj_green_9500
 * (batch9492). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9491_marker[] = "libcgj-batch9491";

/* Product shell green lamp for wave 9500. */
#define B9491_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9491_green(void)
{
	return B9491_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_9500 - report product shell green lamp for wave 9500.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_9500(void)
{
	(void)NULL;
	return b9491_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_9500(void)
    __attribute__((alias("gj_shell_green_9500")));
