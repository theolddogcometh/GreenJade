/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8137: strncat capacity-need stub.
 *
 * Surface (unique symbols):
 *   size_t gj_strncat_need_u_8137(size_t dst_len, size_t n);
 *     - Return octets required for a strncat result when up to n payload
 *       octets are appended, including the terminating NUL:
 *       dst_len + n + 1. Saturates at (size_t)-1 on overflow.
 *   size_t __gj_strncat_need_u_8137  (alias)
 *   __libcgj_batch8137_marker = "libcgj-batch8137"
 *
 * Exclusive continuum CREATE-ONLY (8131-8140: strcat/strncat stubs —
 * src_empty, dst_null, n_zero, n_pos, take, cat_need, ncat_need,
 * kind_unbounded, kind_bounded, batch_id_8140).
 * Unique gj_strncat_need_u_8137 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8137_marker[] = "libcgj-batch8137";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b8137_need(size_t dst_len, size_t n)
{
	size_t sum;

	/* dst_len + n + 1 with soft saturation on overflow. */
	if (dst_len > ((size_t)-1 - 1u)) {
		return (size_t)-1;
	}
	sum = dst_len + 1u;
	if (n > ((size_t)-1 - sum)) {
		return (size_t)-1;
	}
	return sum + n;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strncat_need_u_8137 - buffer octets needed for strncat result + NUL.
 *
 * dst_len: length of existing destination payload (no NUL)
 * n:       maximum payload octets that may be appended from src
 *
 * Returns dst_len + n + 1, or (size_t)-1 on overflow. Soft arithmetic;
 * does not call strncat(3). No parent wires.
 */
size_t
gj_strncat_need_u_8137(size_t dst_len, size_t n)
{
	(void)NULL;
	return b8137_need(dst_len, n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_strncat_need_u_8137(size_t dst_len, size_t n)
    __attribute__((alias("gj_strncat_need_u_8137")));
