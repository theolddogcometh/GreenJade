/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6695: shell product green extract (wave 6700).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_6700(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       6700 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_6700  (alias)
 *   __libcgj_batch6695_marker = "libcgj-batch6695"
 *
 * Milestone 6700 exclusive continuum CREATE-ONLY (6691-6700). Unique
 * gj_shell_green_6700 surface only; no multi-def. Distinct from
 * gj_shell_green_6600 (batch6595), gj_shell_green_6500 (batch6495),
 * gj_shell_green_6400 (batch6395), and gj_libcgj_green_6700
 * (batch6696). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6695_marker[] = "libcgj-batch6695";

/* Product shell green lamp for wave 6700. */
#define B6695_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6695_green(void)
{
	return B6695_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_6700 - report product shell green lamp for wave 6700.
 *
 * Always returns 1 (shell green/ready). Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_6700(void)
{
	(void)NULL;
	return b6695_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_6700(void)
    __attribute__((alias("gj_shell_green_6700")));
