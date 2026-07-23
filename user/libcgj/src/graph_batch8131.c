/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8131: strcat src-empty / NULL-as-empty stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strcat_src_empty_u_8131(const char *src);
 *     - Return 1 if src is NULL or points at an empty C-string
 *       (first octet is NUL); else 0. Soft strcat precondition helper:
 *       empty/NULL src is a no-op append.
 *   uint32_t __gj_strcat_src_empty_u_8131  (alias)
 *   __libcgj_batch8131_marker = "libcgj-batch8131"
 *
 * Exclusive continuum CREATE-ONLY (8131-8140: strcat/strncat stubs —
 * src_empty, dst_null, n_zero, n_pos, take, cat_need, ncat_need,
 * kind_unbounded, kind_bounded, batch_id_8140).
 * Unique gj_strcat_src_empty_u_8131 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8131_marker[] = "libcgj-batch8131";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8131_src_empty(const char *src)
{
	if (src == NULL) {
		return 1u;
	}
	if (src[0] == '\0') {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcat_src_empty_u_8131 - 1 if strcat src is NULL or empty.
 *
 * src: candidate strcat source string (may be NULL)
 *
 * Returns 1 when append is a no-op (NULL or empty), else 0. Soft catalog
 * test; does not call strcat(3). No parent wires.
 */
uint32_t
gj_strcat_src_empty_u_8131(const char *src)
{
	(void)NULL;
	return b8131_src_empty(src);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strcat_src_empty_u_8131(const char *src)
    __attribute__((alias("gj_strcat_src_empty_u_8131")));
