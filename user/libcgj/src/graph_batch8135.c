/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8135: strncat take-count (min n, srclen) stub.
 *
 * Surface (unique symbols):
 *   size_t gj_strncat_take_u_8135(size_t n, size_t src_len);
 *     - Return the payload octet count strncat would copy: min(n,
 *       src_len). Soft arithmetic only; no buffer access.
 *   size_t __gj_strncat_take_u_8135  (alias)
 *   __libcgj_batch8135_marker = "libcgj-batch8135"
 *
 * Exclusive continuum CREATE-ONLY (8131-8140: strcat/strncat stubs —
 * src_empty, dst_null, n_zero, n_pos, take, cat_need, ncat_need,
 * kind_unbounded, kind_bounded, batch_id_8140).
 * Unique gj_strncat_take_u_8135 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8135_marker[] = "libcgj-batch8135";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b8135_take(size_t n, size_t src_len)
{
	if (n < src_len) {
		return n;
	}
	return src_len;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strncat_take_u_8135 - payload octets strncat would copy.
 *
 * n:       maximum payload octets from src
 * src_len: length of src excluding the terminating NUL
 *
 * Returns min(n, src_len). Soft arithmetic; does not call strncat(3).
 * No parent wires.
 */
size_t
gj_strncat_take_u_8135(size_t n, size_t src_len)
{
	(void)NULL;
	return b8135_take(n, src_len);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_strncat_take_u_8135(size_t n, size_t src_len)
    __attribute__((alias("gj_strncat_take_u_8135")));
