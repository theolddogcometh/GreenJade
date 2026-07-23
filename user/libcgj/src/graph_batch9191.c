/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9191: shell product green extract (wave 9200).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_9200(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       9200 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_9200  (alias)
 *   __libcgj_batch9191_marker = "libcgj-batch9191"
 *
 * Milestone 9200 exclusive continuum CREATE-ONLY (9191-9200). Unique
 * gj_shell_green_9200 surface only; no multi-def. Distinct from
 * gj_shell_green_9100 (batch9091), gj_shell_green_9000 (batch8991),
 * gj_shell_green_8900 (batch8891), and gj_libcgj_green_9200
 * (batch9192). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9191_marker[] = "libcgj-batch9191";

/* Product shell green lamp for wave 9200. */
#define B9191_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9191_green(void)
{
	return B9191_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_9200 - report product shell green lamp for wave 9200.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_9200(void)
{
	(void)NULL;
	return b9191_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_9200(void)
    __attribute__((alias("gj_shell_green_9200")));
