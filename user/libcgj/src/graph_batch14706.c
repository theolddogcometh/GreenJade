/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14706: smoke soft lamp (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14706(void);
 *     - Returns the compile-time graph batch number for this TU (14706).
 *   uint32_t gj_smoke_soft_14706(void);
 *     - Returns 1 (smoke soft surface ready for the milestone 14725
 *       continuum). Soft compile-time product tag; not a live smoke run.
 *   uint32_t __gj_batch_id_14706  (alias)
 *   uint32_t __gj_smoke_soft_14706  (alias)
 *   __libcgj_batch14706_marker = "libcgj-batch14706"
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

const char __libcgj_batch14706_marker[] = "libcgj-batch14706";

/* Smoke soft ready lamp for wave 14725. */
#define B14706_ID          14706u
#define B14706_SMOKE_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14706_id(void)
{
	return B14706_ID;
}

static uint32_t
b14706_soft(void)
{
	return B14706_SMOKE_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14706 - report this TU's graph batch number.
 *
 * Always returns 14706.
 */
uint32_t
gj_batch_id_14706(void)
{
	(void)NULL;
	return b14706_id();
}

/*
 * gj_smoke_soft_14706 - soft continuum surface
 *
 * Always returns 1u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_smoke_soft_14706(void)
{
	return b14706_soft();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14706(void)
    __attribute__((alias("gj_batch_id_14706")));

uint32_t __gj_smoke_soft_14706(void)
    __attribute__((alias("gj_smoke_soft_14706")));
