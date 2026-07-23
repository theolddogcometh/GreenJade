/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8113: strcpy empty-source need id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strcpy_empty_need_id_8113(void);
 *     - Return soft byte-need for strcpy of an empty source (1: just NUL).
 *   uint32_t __gj_strcpy_empty_need_id_8113  (alias)
 *   __libcgj_batch8113_marker = "libcgj-batch8113"
 *
 * Exclusive continuum CREATE-ONLY (8111-8120: strcpy bounds stubs —
 * min_dst_id, nul_bytes_id, empty_need_id, dst_is_zero, need_fits,
 * dst_ok, need_ok, need_from_len, need_errorish, batch_id_8120).
 * Unique gj_strcpy_empty_need_id_8113 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8113_marker[] = "libcgj-batch8113";

/* Soft need for strcpy("") including the terminating NUL. */
#define B8113_EMPTY_NEED  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8113_empty_need_id(void)
{
	return B8113_EMPTY_NEED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcpy_empty_need_id_8113 - soft need for empty-source strcpy.
 *
 * Always returns 1 (write a single NUL). Catalog id only; does not call
 * strcpy(3). No parent wires.
 */
uint32_t
gj_strcpy_empty_need_id_8113(void)
{
	(void)NULL;
	return b8113_empty_need_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strcpy_empty_need_id_8113(void)
    __attribute__((alias("gj_strcpy_empty_need_id_8113")));
