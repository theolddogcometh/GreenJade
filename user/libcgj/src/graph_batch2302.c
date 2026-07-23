/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2302: freestanding byte-buffer length soft-ok
 * predicate (post-2300 buffer exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_buf_len_ok(size_t len, size_t cap);
 *     - Return 1 if len is a valid used length for a buffer of capacity
 *       cap (len <= cap), else 0. Soft structural check only.
 *   int __gj_buf_len_ok  (alias)
 *   __libcgj_batch2302_marker = "libcgj-batch2302"
 *
 * Post-2300 buffer exclusive wave (2301-2310). Distinct from
 * gj_buf_cap_ok (batch2301) - unique gj_buf_len_ok surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2302_marker[] = "libcgj-batch2302";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if len fits inside cap (len <= cap). */
static int
b2302_len_ok(size_t cbLen, size_t cbCap)
{
	if (cbLen > cbCap) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_len_ok - soft check that len is valid for capacity cap.
 *
 * len: used length in bytes
 * cap: total capacity in bytes
 *
 * Returns 1 when len <= cap, else 0. Does not call libc.
 */
int
gj_buf_len_ok(size_t cbLen, size_t cbCap)
{
	(void)NULL;
	return b2302_len_ok(cbLen, cbCap);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_buf_len_ok(size_t cbLen, size_t cbCap)
    __attribute__((alias("gj_buf_len_ok")));
