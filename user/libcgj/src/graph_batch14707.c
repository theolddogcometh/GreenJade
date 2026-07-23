/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14707: dyn soft lamp (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14707(void);
 *     - Returns the compile-time graph batch number for this TU (14707).
 *   uint32_t gj_dyn_soft_14707(void);
 *     - Returns 1 (dyn soft surface ready for the milestone 14725
 *       continuum). Soft compile-time product tag; not a live dyn probe.
 *   uint32_t __gj_batch_id_14707  (alias)
 *   uint32_t __gj_dyn_soft_14707  (alias)
 *   __libcgj_batch14707_marker = "libcgj-batch14707"
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

const char __libcgj_batch14707_marker[] = "libcgj-batch14707";

/* Dyn soft ready lamp for wave 14725. */
#define B14707_ID        14707u
#define B14707_DYN_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14707_id(void)
{
	return B14707_ID;
}

static uint32_t
b14707_soft(void)
{
	return B14707_DYN_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14707 - report this TU's graph batch number.
 *
 * Always returns 14707.
 */
uint32_t
gj_batch_id_14707(void)
{
	(void)NULL;
	return b14707_id();
}

/*
 * gj_dyn_soft_14707 - soft continuum surface
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_dyn_soft_14707(void)
{
	return b14707_soft();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14707(void)
    __attribute__((alias("gj_batch_id_14707")));

uint32_t __gj_dyn_soft_14707(void)
    __attribute__((alias("gj_dyn_soft_14707")));
