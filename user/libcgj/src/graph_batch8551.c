/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8551: regex empty-pattern ok predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_re_empty_ok_u_8551(uint32_t len);
 *     - Return 1 if len == 0 (empty pattern length is catalog-ok for
 *       vacuous match stubs), else 0. Soft policy only; not a regex
 *       engine and does not compile or execute patterns.
 *   uint32_t __gj_re_empty_ok_u_8551  (alias)
 *   __libcgj_batch8551_marker = "libcgj-batch8551"
 *
 * Exclusive continuum CREATE-ONLY (8551-8560: regex/class id stubs —
 * empty_ok, dot_id, star_id, plus_id, qmark_id, anchor_start_id,
 * anchor_end_id, class_ok, escape_ok, batch_id_8560). Unique
 * gj_re_empty_ok_u_8551 surface only; no multi-def. No parent wires.
 * No real regex engine. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8551_marker[] = "libcgj-batch8551";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8551_empty_ok(uint32_t u32Len)
{
	return (u32Len == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_re_empty_ok_u_8551 - 1 if pattern length is empty (0).
 *
 * len: soft pattern length (bytes / units) under test
 *
 * Returns 1 when len is 0, else 0. Catalog empty-pattern policy stub
 * only; does not parse or match. No parent wires.
 */
uint32_t
gj_re_empty_ok_u_8551(uint32_t u32Len)
{
	(void)NULL;
	return b8551_empty_ok(u32Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_re_empty_ok_u_8551(uint32_t u32Len)
    __attribute__((alias("gj_re_empty_ok_u_8551")));
