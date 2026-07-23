/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2305: freestanding byte-buffer empty predicate
 * (post-2300 buffer exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_buf_empty_p(size_t len);
 *     - Return 1 if the buffer is empty (len == 0), else 0.
 *   int __gj_buf_empty_p  (alias)
 *   __libcgj_batch2305_marker = "libcgj-batch2305"
 *
 * Post-2300 buffer exclusive wave (2301-2310). Distinct from
 * gj_buf_full_p (batch2304) - unique gj_buf_empty_p surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2305_marker[] = "libcgj-batch2305";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if used length is zero. */
static int
b2305_empty_p(size_t cbLen)
{
	if (cbLen == 0u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_empty_p - true when a buffer has zero used length.
 *
 * len: used length in bytes
 *
 * Returns 1 when len == 0, else 0. Does not call libc.
 */
int
gj_buf_empty_p(size_t cbLen)
{
	(void)NULL;
	return b2305_empty_p(cbLen);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_buf_empty_p(size_t cbLen)
    __attribute__((alias("gj_buf_empty_p")));
