/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14791: shell product green extract (wave 14800).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_14800(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       14800 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_14800  (alias)
 *   __libcgj_batch14791_marker = "libcgj-batch14791"
 *
 * Milestone 14800 exclusive continuum CREATE-ONLY (14791-14800). Unique surface
 * only; no multi-def. Distinct from gj_*_14700 / gj_*_14600 / gj_*_14500 milestone
 * surfaces and sibling 14800 milestone symbols. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14791_marker[] = "libcgj-batch14791";

/* Shell green lamp for wave 14800. */
#define B14791_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14791_green(void)
{
	return B14791_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_14800 - soft continuum surface
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_shell_green_14800(void)
{
	(void)NULL;
	return b14791_green();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_14800(void)
    __attribute__((alias("gj_shell_green_14800")));
