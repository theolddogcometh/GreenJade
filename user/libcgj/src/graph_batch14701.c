/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14701: shell product green extract (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14701(void);
 *     - Returns the compile-time graph batch number for this TU (14701).
 *   uint32_t gj_shell_green_14701(void);
 *     - Returns 1 (product shell green/ready lamp for the milestone
 *       14725 continuum). Soft compile-time product status tag; not a
 *       runtime probe of greenjade-shell.
 *   uint32_t __gj_batch_id_14701  (alias)
 *   uint32_t __gj_shell_green_14701  (alias)
 *   __libcgj_batch14701_marker = "libcgj-batch14701"
 *
 * Milestone 14725 exclusive continuum CREATE-ONLY (14701-14725). Unique
 * surface only; no multi-def. Distinct from gj_*_14700 / gj_*_14600 /
 * gj_*_14500 milestone surfaces and sibling 14725 milestone symbols.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14701_marker[] = "libcgj-batch14701";

/* Shell green lamp for wave 14725. */
#define B14701_ID           14701u
#define B14701_SHELL_GREEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14701_id(void)
{
	return B14701_ID;
}

static uint32_t
b14701_green(void)
{
	return B14701_SHELL_GREEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14701 - report this TU's graph batch number.
 *
 * Always returns 14701.
 */
uint32_t
gj_batch_id_14701(void)
{
	(void)NULL;
	return b14701_id();
}

/*
 * gj_shell_green_14701 - soft continuum surface
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_shell_green_14701(void)
{
	return b14701_green();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14701(void)
    __attribute__((alias("gj_batch_id_14701")));

uint32_t __gj_shell_green_14701(void)
    __attribute__((alias("gj_shell_green_14701")));
