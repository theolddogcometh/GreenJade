/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2301: freestanding byte-buffer capacity soft-ok
 * predicate (post-2300 buffer exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_buf_cap_ok(size_t cap);
 *     - Return 1 if cap is a usable buffer capacity (cap > 0), else 0.
 *       Soft structural check only; does not allocate or touch storage.
 *   int __gj_buf_cap_ok  (alias)
 *   __libcgj_batch2301_marker = "libcgj-batch2301"
 *
 * Post-2300 buffer exclusive wave (2301-2310). Distinct from
 * gj_buf_append capacity handling (batch431) - unique gj_buf_cap_ok
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2301_marker[] = "libcgj-batch2301";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if cap is nonzero (usable capacity). */
static int
b2301_cap_ok(size_t cbCap)
{
	if (cbCap == 0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_cap_ok - soft check that cap is a usable buffer capacity.
 *
 * cap: total capacity in bytes
 *
 * Returns 1 when cap > 0, else 0. Does not call libc.
 */
int
gj_buf_cap_ok(size_t cbCap)
{
	(void)NULL;
	return b2301_cap_ok(cbCap);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_buf_cap_ok(size_t cbCap)
    __attribute__((alias("gj_buf_cap_ok")));
