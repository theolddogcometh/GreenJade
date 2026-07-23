/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5695: shell product green extract (wave 5700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_5700(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       5700 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_5700  (alias)
 *   __libcgj_batch5695_marker = "libcgj-batch5695"
 *
 * Milestone 5700 exclusive continuum CREATE-ONLY (5691-5700). Unique
 * gj_shell_green_5700 surface only; no multi-def. Distinct from
 * gj_shell_green_5650 (batch5645), gj_shell_green_5600 (batch5595),
 * gj_shell_green_5550 (batch5545), and gj_libcgj_green_5700
 * (batch5696). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5695_marker[] = "libcgj-batch5695";

/* Product shell green lamp for wave 5700. */
#define B5695_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5695_green(void)
{
	return B5695_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_5700 - report product shell green lamp for wave 5700.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_5700(void)
{
	(void)NULL;
	return b5695_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_5700(void)
    __attribute__((alias("gj_shell_green_5700")));
