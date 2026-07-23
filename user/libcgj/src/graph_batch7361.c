/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7361: fallocate FALLOC_FL_KEEP_SIZE mode id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_falloc_keep_size_id_7361(void);
 *     - Return soft FALLOC_FL_KEEP_SIZE constant (0x01).
 *   uint32_t __gj_falloc_keep_size_id_7361  (alias)
 *   __libcgj_batch7361_marker = "libcgj-batch7361"
 *
 * Exclusive continuum CREATE-ONLY (7361-7370: fallocate mode stubs —
 * keep_size_id, punch_hole_id, collapse_range_id, zero_range_id,
 * has_keep_size, has_punch_hole, modes_ok, modes_pack, modes_errorish,
 * batch_id_7370).
 * Unique gj_falloc_keep_size_id_7361 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7361_marker[] = "libcgj-batch7361";

/* FALLOC_FL_KEEP_SIZE: do not extend file size (0x01). */
#define B7361_FALLOC_FL_KEEP_SIZE ((uint32_t)0x01u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7361_falloc_keep_size_id(void)
{
	return B7361_FALLOC_FL_KEEP_SIZE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_falloc_keep_size_id_7361 - soft FALLOC_FL_KEEP_SIZE mode constant.
 *
 * Always returns 0x01 (Linux FALLOC_FL_KEEP_SIZE). Catalog id only;
 * does not call fallocate. No parent wires.
 */
uint32_t
gj_falloc_keep_size_id_7361(void)
{
	(void)NULL;
	return b7361_falloc_keep_size_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_falloc_keep_size_id_7361(void)
    __attribute__((alias("gj_falloc_keep_size_id_7361")));
