/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7365: fallocate FALLOC_FL_KEEP_SIZE presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_falloc_has_keep_size_u_7365(uint32_t mode);
 *     - Return 1 if mode has FALLOC_FL_KEEP_SIZE (0x01) set, else 0.
 *   uint32_t __gj_falloc_has_keep_size_u_7365  (alias)
 *   __libcgj_batch7365_marker = "libcgj-batch7365"
 *
 * Exclusive continuum CREATE-ONLY (7361-7370: fallocate mode stubs —
 * keep_size_id, punch_hole_id, collapse_range_id, zero_range_id,
 * has_keep_size, has_punch_hole, modes_ok, modes_pack, modes_errorish,
 * batch_id_7370).
 * Unique gj_falloc_has_keep_size_u_7365 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7365_marker[] = "libcgj-batch7365";

/* FALLOC_FL_KEEP_SIZE bit (0x01). */
#define B7365_FALLOC_FL_KEEP_SIZE ((uint32_t)0x01u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7365_falloc_has_keep_size(uint32_t u32Mode)
{
	return ((u32Mode & B7365_FALLOC_FL_KEEP_SIZE) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_falloc_has_keep_size_u_7365 - 1 if mode includes KEEP_SIZE.
 *
 * mode: fallocate(2) mode argument
 *
 * Returns 1 when bit 0x01 is set, else 0. Soft bit test only; does not
 * call fallocate. No parent wires.
 */
uint32_t
gj_falloc_has_keep_size_u_7365(uint32_t u32Mode)
{
	(void)NULL;
	return b7365_falloc_has_keep_size(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_falloc_has_keep_size_u_7365(uint32_t u32Mode)
    __attribute__((alias("gj_falloc_has_keep_size_u_7365")));
