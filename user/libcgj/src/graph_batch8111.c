/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8111: strcpy minimum destination size id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strcpy_min_dst_id_8111(void);
 *     - Return soft minimum destination size for strcpy (1: empty + NUL).
 *   uint32_t __gj_strcpy_min_dst_id_8111  (alias)
 *   __libcgj_batch8111_marker = "libcgj-batch8111"
 *
 * Exclusive continuum CREATE-ONLY (8111-8120: strcpy bounds stubs —
 * min_dst_id, nul_bytes_id, empty_need_id, dst_is_zero, need_fits,
 * dst_ok, need_ok, need_from_len, need_errorish, batch_id_8120).
 * Unique gj_strcpy_min_dst_id_8111 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8111_marker[] = "libcgj-batch8111";

/* Soft minimum strcpy destination capacity (empty string + NUL). */
#define B8111_MIN_DST  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8111_min_dst_id(void)
{
	return B8111_MIN_DST;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcpy_min_dst_id_8111 - soft minimum destination size for strcpy.
 *
 * Always returns 1 (room for a single terminating NUL). Catalog id only;
 * does not call strcpy(3). No parent wires.
 */
uint32_t
gj_strcpy_min_dst_id_8111(void)
{
	(void)NULL;
	return b8111_min_dst_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strcpy_min_dst_id_8111(void)
    __attribute__((alias("gj_strcpy_min_dst_id_8111")));
