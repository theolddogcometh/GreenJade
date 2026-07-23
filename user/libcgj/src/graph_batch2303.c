/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2303: freestanding byte-buffer remaining room
 * (post-2300 buffer exclusive surface).
 *
 * Surface (unique symbols):
 *   size_t gj_buf_room(size_t len, size_t cap);
 *     - Return remaining free bytes (cap - len) when len <= cap.
 *       If len > cap (invalid watermark), return 0.
 *   size_t __gj_buf_room  (alias)
 *   __libcgj_batch2303_marker = "libcgj-batch2303"
 *
 * Post-2300 buffer exclusive wave (2301-2310). Distinct from
 * gj_buf_len_ok (batch2302) - unique gj_buf_room surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2303_marker[] = "libcgj-batch2303";

/* ---- freestanding helpers ---------------------------------------------- */

/* Remaining free bytes; 0 when len is past cap. */
static size_t
b2303_room(size_t cbLen, size_t cbCap)
{
	if (cbLen > cbCap) {
		return 0u;
	}
	return cbCap - cbLen;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_room - free bytes remaining in a capacity-bounded buffer.
 *
 * len: used length in bytes
 * cap: total capacity in bytes
 *
 * Returns cap - len when len <= cap, else 0. Does not call libc.
 */
size_t
gj_buf_room(size_t cbLen, size_t cbCap)
{
	(void)NULL;
	return b2303_room(cbLen, cbCap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_buf_room(size_t cbLen, size_t cbCap)
    __attribute__((alias("gj_buf_room")));
