/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14703: product bar3 readiness gate (wave 14725).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_14703(void);
 *     - Returns the compile-time graph batch number for this TU (14703).
 *   uint32_t gj_bar3_ready_14703(void);
 *     - Returns 0 (bar3 readiness not complete for the milestone 14725
 *       continuum; still open). Soft pure-data readiness lamp for
 *       Steam/Deck/Top50.
 *   uint32_t __gj_batch_id_14703  (alias)
 *   uint32_t __gj_bar3_ready_14703  (alias)
 *   __libcgj_batch14703_marker = "libcgj-batch14703"
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

const char __libcgj_batch14703_marker[] = "libcgj-batch14703";

/* Bar3 readiness lamp for wave 14725 (still open). */
#define B14703_ID          14703u
#define B14703_BAR3_READY  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14703_id(void)
{
	return B14703_ID;
}

static uint32_t
b14703_ready(void)
{
	return B14703_BAR3_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_14703 - report this TU's graph batch number.
 *
 * Always returns 14703.
 */
uint32_t
gj_batch_id_14703(void)
{
	(void)NULL;
	return b14703_id();
}

/*
 * gj_bar3_ready_14703 - soft continuum surface
 *
 * Always returns 0u. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_bar3_ready_14703(void)
{
	return b14703_ready();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_14703(void)
    __attribute__((alias("gj_batch_id_14703")));

uint32_t __gj_bar3_ready_14703(void)
    __attribute__((alias("gj_bar3_ready_14703")));
