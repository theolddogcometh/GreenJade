/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8136: strcat capacity-need stub.
 *
 * Surface (unique symbols):
 *   size_t gj_strcat_need_u_8136(size_t dst_len, size_t src_len);
 *     - Return octets required for a full strcat result including the
 *       terminating NUL: dst_len + src_len + 1. Saturates at
 *       (size_t)-1 on overflow.
 *   size_t __gj_strcat_need_u_8136  (alias)
 *   __libcgj_batch8136_marker = "libcgj-batch8136"
 *
 * Exclusive continuum CREATE-ONLY (8131-8140: strcat/strncat stubs —
 * src_empty, dst_null, n_zero, n_pos, take, cat_need, ncat_need,
 * kind_unbounded, kind_bounded, batch_id_8140).
 * Unique gj_strcat_need_u_8136 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8136_marker[] = "libcgj-batch8136";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b8136_need(size_t dst_len, size_t src_len)
{
	size_t sum;

	/* dst_len + src_len + 1 with soft saturation on overflow. */
	if (dst_len > ((size_t)-1 - 1u)) {
		return (size_t)-1;
	}
	sum = dst_len + 1u;
	if (src_len > ((size_t)-1 - sum)) {
		return (size_t)-1;
	}
	return sum + src_len;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strcat_need_u_8136 - buffer octets needed for strcat result + NUL.
 *
 * dst_len: length of existing destination payload (no NUL)
 * src_len: length of source payload (no NUL)
 *
 * Returns dst_len + src_len + 1, or (size_t)-1 on overflow. Soft
 * arithmetic; does not call strcat(3). No parent wires.
 */
size_t
gj_strcat_need_u_8136(size_t dst_len, size_t src_len)
{
	(void)NULL;
	return b8136_need(dst_len, src_len);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_strcat_need_u_8136(size_t dst_len, size_t src_len)
    __attribute__((alias("gj_strcat_need_u_8136")));
