/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2304: freestanding byte-buffer full predicate
 * (post-2300 buffer exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_buf_full_p(size_t len, size_t cap);
 *     - Return 1 if the buffer is full (len >= cap), else 0.
 *       Treats overfull watermarks (len > cap) as full.
 *   int __gj_buf_full_p  (alias)
 *   __libcgj_batch2304_marker = "libcgj-batch2304"
 *
 * Post-2300 buffer exclusive wave (2301-2310). Distinct from
 * gj_buf_room (batch2303) - unique gj_buf_full_p surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2304_marker[] = "libcgj-batch2304";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if no free room remains (len >= cap). */
static int
b2304_full_p(size_t cbLen, size_t cbCap)
{
	if (cbLen >= cbCap) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_full_p - true when a capacity-bounded buffer has no free room.
 *
 * len: used length in bytes
 * cap: total capacity in bytes
 *
 * Returns 1 when len >= cap, else 0. Does not call libc.
 */
int
gj_buf_full_p(size_t cbLen, size_t cbCap)
{
	(void)NULL;
	return b2304_full_p(cbLen, cbCap);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_buf_full_p(size_t cbLen, size_t cbCap)
    __attribute__((alias("gj_buf_full_p")));
